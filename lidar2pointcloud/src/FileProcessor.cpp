
#include "FileProcessor.hpp"
#include "lidar2pointcloud.hpp"
#include "StringUtils.hpp"
#include "FieldUtils.hpp"
#include "PointCloudUtils.hpp"
#include "ExportUtils.hpp"
#include "GroundModelUtils.hpp"
#include "MathUtils.hpp"
#include "PointCloudSaver.hpp"

#include <cbdf/PointCloudSerializer.hpp>

#include <cbdf/BlockDataFileExceptions.hpp>
#include <cbdf/ExperimentSerializer.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <optional>

using namespace pointcloud;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id);

namespace
{
    template<typename T>
    T to_value(std::optional<T> value, T default_value)
    {
        if (value.has_value())
            return value.value();

        return default_value;
    }
}

cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id)
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{}

void cFileProcessor::saveCompactPointCloud(bool compact)
{
    mSaveCompactPointCloud = compact;
}

void cFileProcessor::saveFrameIds(bool save)
{
    mSaveFrameIds = save;
}

void cFileProcessor::savePixelInfo(bool save)
{
    mSavePixelInfo = save;
}

void cFileProcessor::savePlyFiles(bool savePlys)
{
    mSavePlyFiles = savePlys;
}

void cFileProcessor::plyUseBinaryFormat(bool binaryFormat)
{
    mPlyUseBinaryFormat = binaryFormat;
}

void cFileProcessor::setDefaults(const cLidarMapConfigDefaults& defaults)
{
    mDefaults = defaults;
}

void cFileProcessor::setParameters(const cLidarMapConfigScan& parameters)
{
    mParameters = parameters;
}

void cFileProcessor::process_file()
{
    mHasGroundData = is_ground_data_loaded();

    std::unique_ptr<cLidar2PointCloud> converter = std::make_unique<cLidar2PointCloud>(mID);


    const auto& limits = mDefaults.getSensorLimits();

    converter->setValidRange_m(to_value(mParameters.getMinDistance_m(), limits.getMinDistance_m()),
        to_value(mParameters.getMaxDistance_m(), limits.getMaxDistance_m()));

    converter->setAzimuthWindow_deg(to_value(mParameters.getMinAzimuth_deg(), limits.getMinAzimuth_deg()),
        to_value(mParameters.getMaxAzimuth_deg(), limits.getMaxAzimuth_deg()));

    converter->setAltitudeWindow_deg(to_value(mParameters.getMinAltitude_deg(), limits.getMinAltitude_deg()),
        to_value(mParameters.getMaxAltitude_deg(), limits.getMaxAltitude_deg()));

    double startX_m = mParameters.getStart_X_m().has_value() ? mParameters.getStart_X_m().value() : -10.0;
    double startY_m = mParameters.getStart_Y_m().has_value() ? mParameters.getStart_Y_m().value() : -10.0;
    double startZ_m = mParameters.getStart_Z_m().has_value() ? mParameters.getStart_Z_m().value() : -10.0;

    double endX_m = mParameters.getEnd_X_m().has_value() ? mParameters.getEnd_X_m().value() : startX_m;
    double endY_m = mParameters.getEnd_Y_m().has_value() ? mParameters.getEnd_Y_m().value() : startY_m;
    double endZ_m = mParameters.getEnd_Z_m().has_value() ? mParameters.getEnd_Z_m().value() : startZ_m;

    const auto& height = mDefaults.getDollyHeight();

    switch (height.getHeightAxis())
    {
    case cLidarMapConfigDefaults_DollyHeight::eHeightAxis::X:
        if (startX_m < -5.0) startX_m = height.getHeight_m();
        if (endX_m < -5.0) endX_m = height.getHeight_m();
        break;
    case cLidarMapConfigDefaults_DollyHeight::eHeightAxis::Y:
        if (startY_m < -5.0) startY_m = height.getHeight_m();
        if (endY_m < -5.0) endY_m = height.getHeight_m();
        break;
    case cLidarMapConfigDefaults_DollyHeight::eHeightAxis::Z:
        if (startZ_m < -5.0) startZ_m = height.getHeight_m();
        if (endZ_m < -5.0) endZ_m = height.getHeight_m();
        break;
    }

    converter->setInitialPosition_m(startX_m, startY_m, startZ_m);
    converter->setFinalPosition_m(endX_m, endY_m, endZ_m);

    const auto& speeds = mDefaults.getDollySpeeds();

    converter->setDollySpeed(to_value<double>(mParameters.getVx_mmps(), speeds.getVx_mmps()), 
        to_value<double>(mParameters.getVy_mmps(), speeds.getVy_mmps()),
        to_value<double>(mParameters.getVz_mmps(), speeds.getVz_mmps()));

    const auto& flatten = mDefaults.getFlattening();

    mFlattenPointCloud = flatten.getFlatteningPointCloud();
    mMaxAngle_deg      = flatten.getMaxAngle_deg();
    mThreshold_pct     = flatten.getThreshold_pct();

    auto om = mParameters.getOrientationModel();
    if (om.has_value())
    {
        switch (om.value())
        {
        case eOrientationModel::CONSTANT:
            converter->setOrientationOffset_deg(to_value(mParameters.getSensorYawOffset_deg(), 0.0),
                to_value(mParameters.getSensorPitchOffset_deg(), 0.0), 
                to_value(mParameters.getSensorRollOffset_deg(), 0.0));
            break;

        case eOrientationModel::LINEAR:
            converter->setInitialOffset_deg(to_value(mParameters.getStartYawOffset_deg(), 0.0),
                to_value(mParameters.getStartPitchOffset_deg(), 0.0), 
                to_value(mParameters.getStartRollOffset_deg(), 0.0));

            converter->setFinalOffset_deg(to_value(mParameters.getEndYawOffset_deg(), 0.0),
                to_value(mParameters.getEndPitchOffset_deg(), 0.0), 
                to_value(mParameters.getEndRollOffset_deg(), 0.0));

            break;

        case eOrientationModel::INTREP_CURVE:
            converter->setOrientationInterpTable(mParameters.getOrientationTable());
            break;

        case eOrientationModel::IMU:
            break;
        }
    }
    else
    {
        if ((mParameters.getEndPitchOffset_deg() == mParameters.getEndPitchOffset_deg())
            && (mParameters.getEndRollOffset_deg() == mParameters.getEndRollOffset_deg())
            && (mParameters.getEndYawOffset_deg() == mParameters.getEndYawOffset_deg()))
        {
            converter->setOrientationOffset_deg(to_value(mParameters.getSensorYawOffset_deg(), 0.0),
                to_value(mParameters.getSensorPitchOffset_deg(), 0.0),
                to_value(mParameters.getSensorRollOffset_deg(), 0.0));
        }
        else
        {
            converter->setInitialOffset_deg(to_value(mParameters.getStartYawOffset_deg(), 0.0),
                to_value(mParameters.getStartPitchOffset_deg(), 0.0),
                to_value(mParameters.getStartRollOffset_deg(), 0.0));

            converter->setFinalOffset_deg(to_value(mParameters.getEndYawOffset_deg(), 0.0),
                to_value(mParameters.getEndPitchOffset_deg(), 0.0),
                to_value(mParameters.getEndRollOffset_deg(), 0.0));
        }
    }

    const auto& orientation = mDefaults.getSensorOrientation();

    converter->setSensorMountOrientation(to_value(mParameters.getSensorMountYaw_deg(), orientation.getYaw_deg()),
        to_value(mParameters.getSensorMountPitch_deg(), orientation.getPitch_deg()),
        to_value(mParameters.getSensorMountRoll_deg(), orientation.getRoll_deg()));

    const auto& options = mDefaults.getOptions();

    eTranslateToGroundModel translateModel = eTranslateToGroundModel::NONE;

    if (mHasGroundData)
    {
        if (mParameters.getTranslateToGroundModel().has_value())
        {
            translateModel = mParameters.getTranslateToGroundModel().value();
        }
        else
        {
            if (options.getTranslateToGround())
                translateModel = eTranslateToGroundModel::FIT;
        }


        converter->setTranslateToGroundModel(translateModel);

        switch (translateModel)
        {
        case eTranslateToGroundModel::CONSTANT:
            converter->setTranslateDistance_m(to_value(mParameters.getTranslateDistance_m(), 0.0));
            break;
        case eTranslateToGroundModel::FIT:
            converter->setTranslateThreshold_pct(to_value(mParameters.getTranslateThreshold_pct(), options.getTranslationThreshold_pct()));
            break;
        case eTranslateToGroundModel::INTREP_CURVE:
            converter->setTranslateInterpTable(mParameters.getTranslateTable());
            break;
        }

        eRotateToGroundModel rotateModel = eRotateToGroundModel::NONE;
        
        if (mParameters.getRotateToGroundModel().has_value())
        {
            rotateModel = mParameters.getRotateToGroundModel().value();
        }
        else
        {
            if (options.getRotateToGround())
                rotateModel = eRotateToGroundModel::FIT;
        }


        converter->setRotationToGroundModel(rotateModel);

        switch (rotateModel)
        {
        case eRotateToGroundModel::CONSTANT:
            converter->setRotationAngles_deg(to_value(mParameters.getRotatePitch_deg(), 0.0), to_value(mParameters.getRotateRoll_deg(), 0.0));
            break;
        case eRotateToGroundModel::FIT:
            converter->setRotationThreshold_pct(to_value(mParameters.getRotateThreshold_pct(), options.getRotationThreshold_pct()));
            break;
        case eRotateToGroundModel::INTREP_CURVE:
            converter->setRotateInterpTable(mParameters.getRotateTable());
            break;
        }
    }
    else
    {
        converter->setTranslateToGroundModel(eTranslateToGroundModel::NONE);
        converter->setRotationToGroundModel(eRotateToGroundModel::NONE);
    }

    // Start by loading the field scan data into memory
    new_file_progress(mID, mInputFile.string());

    converter->loadFieldScanData(mInputFile.string());

    // Compute the Dolly Movement
    if (!converter->computeDollyMovement())
    {
        std::string msg = "Failed to compute the dolly movement: ";
        msg += mInputFile.string();
        console_message(msg);
        return;
    }

    // Compute the Dolly Orientation
    if (!converter->computeDollyOrientation())
    {
        std::string msg = "Failed to compute the dolly orientation: ";
        msg += mInputFile.string();
        console_message(msg);
        return;
    }

    if (!converter->computePointCloud())
    {
        std::string msg = "Failed to compute point cloud: ";
        msg += mInputFile.string();
        console_message(msg);
        return;
    }

    converter->clearLidarData();

    cRappPointCloud& pointCloud = converter->getPointCloudRef();
    
    if (mSaveFrameIds)
        pointCloud.enableFrameIDs();
    else
        pointCloud.disableFrameIDs();

    if (mSavePixelInfo)
        pointCloud.enablePixelInfo();
    else
        pointCloud.disablePixelInfo();

    if (mFlattenPointCloud)
    {
        update_prefix_progress(mID, "Flattening Point Cloud...", 0);

        shiftPointCloudToAGL(mID, pointCloud);
        pointCloud.recomputeBounds();

        sPitchAndRoll_t result = computePcToGroundMeshRotationUsingGrid_deg(pointCloud, mThreshold_pct);

        result.pitch_deg = nMathUtils::bound(result.pitch_deg, -mMaxAngle_deg, mMaxAngle_deg);
        result.roll_deg = nMathUtils::bound(result.roll_deg, -mMaxAngle_deg, mMaxAngle_deg);

        if (result.valid)
        {
            pointCloud.rotate(0.0, result.pitch_deg, result.roll_deg);
        }
    }

    // Shift the point cloud to match the reference point
    auto ref_point = mParameters.getReferencePoint();
    if (ref_point.has_value())
    {
        auto rp1 = ref_point.value();
        
        if (pointCloud.referenceValid())
        {
            auto rp2 = pointCloud.referencePoint();

            auto dx_mm = rp2.x_mm - rp1.x_mm;
            auto dy_mm = rp2.y_mm - rp1.y_mm;
            auto dz_mm = rp2.z_mm - rp1.z_mm;

            if (translateModel != eTranslateToGroundModel::INTREP_CURVE)
            {
                dz_mm = 0;
            }

            pointCloud.translate(dx_mm, dy_mm, dz_mm);
        }
    }

    update_prefix_progress(mID, "Saving Point Cloud...", 0);

    if (mSaveCompactPointCloud)
    {
        savePointCloudFile(*converter, pointCloud);
    }
    else
    {
        std::unique_ptr<cPointCloudSaver> saver = std::make_unique<cPointCloudSaver>(mID, pointCloud);

        saver->setInputFile(mInputFile.string());
        saver->setOutputFile(mOutputFile.string());

        auto dolly_path = converter->getComputedDollyPath();
        saver->setKinematicModel(converter->getKinematicModel(), dolly_path);

        const auto& limits = mDefaults.getSensorLimits();

        saver->setRangeWindow_m(to_value(mParameters.getMinDistance_m(), limits.getMinDistance_m()),
            to_value(mParameters.getMaxDistance_m(), limits.getMaxDistance_m()));

        saver->setAzimuthWindow_deg(to_value(mParameters.getMinAzimuth_deg(), limits.getMinAzimuth_deg()),
            to_value(mParameters.getMaxAzimuth_deg(), limits.getMaxAzimuth_deg()));

        saver->setAltitudeWindow_deg(to_value(mParameters.getMinAltitude_deg(), limits.getMinAltitude_deg()),
            to_value(mParameters.getMaxAltitude_deg(), limits.getMaxAltitude_deg()));

        saver->save(mFlattenPointCloud);
    }

    if (mSavePlyFiles)
    {
        update_prefix_progress(mID, "Exporting Point Cloud...", 0);
        savePlyFiles(pointCloud);
    }

    update_prefix_progress(mID, "Finished", 100);
    complete_file_progress(mID);
}


//-----------------------------------------------------------------------------
void cFileProcessor::savePointCloudFile(const cLidar2PointCloud& data, const cRappPointCloud& pc)
{
    auto processingInfo = data.getProcessingInfo();
    auto experimentInfo = data.getExperimentInfo();

    std::string filename = mOutputFile.string();

    cBlockDataFileWriter fileWriter;

    cPointCloudSerializer 	    pointCloudSerializer;

    pointCloudSerializer.attach(&fileWriter);

    fileWriter.open(filename);
    if (!fileWriter.isOpen())
        return;

    {
        cProcessingInfoSerializer   processInfoSerializer;
        processInfoSerializer.setBufferCapacity(4096);
        processInfoSerializer.attach(&fileWriter);

        writeProcessingInfo(*processingInfo, processInfoSerializer);

        processInfoSerializer.detach();
    }
    update_progress(mID, 33);

    {
        cExperimentSerializer       experimentSerializer;
        experimentSerializer.setBufferCapacity(4096 * 1024);
        experimentSerializer.attach(&fileWriter);

        writeExperimentInfo(*experimentInfo, experimentSerializer);

        experimentSerializer.detach();
    }
    update_progress(mID, 67);

    pointCloudSerializer.writeBeginPointCloudBlock();

    pointCloudSerializer.write(pointcloud::eCOORDINATE_SYSTEM::SENSOR_SEU);

    pointCloudSerializer.write(data.getKinematicModel());


    const auto& limits = mDefaults.getSensorLimits();

    pointCloudSerializer.writeDistanceWindow(to_value(mParameters.getMinDistance_m(), limits.getMinDistance_m()), 
        to_value(mParameters.getMaxDistance_m(), limits.getMaxDistance_m()));

    pointCloudSerializer.writeAzimuthWindow(to_value(mParameters.getMinAzimuth_deg(), limits.getMinAzimuth_deg()),
        to_value(mParameters.getMaxAzimuth_deg(), limits.getMaxAzimuth_deg()));

    pointCloudSerializer.writeAltitudeWindow(to_value(mParameters.getMinAltitude_deg(), limits.getMinAltitude_deg()),
        to_value(mParameters.getMaxAltitude_deg(), limits.getMaxAltitude_deg()));

    const auto& path = data.getComputedDollyPath();
    if (!path.empty())
    {
        pointCloudSerializer.writeBeginSensorKinematics();

        for (const auto& point : path)
        {
            pointCloudSerializer.writeSensorKinematicInfo(to_sensor_kinematics(point));
        }

        pointCloudSerializer.writeEndSensorKinematics();
    }

    if (pc.hasPixelInfo())
    {
        cPointCloud_SensorInfo point_cloud;
        point_cloud.resize(pc.size());
        to_pointcloud(pc, point_cloud);

        pointCloudSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
            point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

        pointCloudSerializer.write(point_cloud);
    }
    else if (pc.hasFrameIDs())
    {
        cPointCloud_FrameId point_cloud;
        point_cloud.resize(pc.size());
        to_pointcloud(pc, point_cloud);

        pointCloudSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
            point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

        pointCloudSerializer.write(point_cloud);
    }
    else
    {
        cPointCloud point_cloud;
        point_cloud.resize(pc.size());
        to_pointcloud(pc, point_cloud);

        pointCloudSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
            point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

        pointCloudSerializer.write(point_cloud);
    }

    pointCloudSerializer.writeEndPointCloudBlock();

    fileWriter.close();
    update_progress(mID, 100);
}

//-----------------------------------------------------------------------------
void cFileProcessor::savePlyFiles(const cRappPointCloud& pc)
{
    auto fe = nStringUtils::splitFilename(mOutputFile.string());
    std::string filename = fe.filename;
    filename += ".ply";
    exportPointcloud2Ply(mID, filename, pc, mPlyUseBinaryFormat);
}

//-----------------------------------------------------------------------------
void cFileProcessor::writeProcessingInfo(const cProcessingInfo& info, cProcessingInfoSerializer& serializer)
{
    if (mFlattenPointCloud)
    {
        serializer.write("Lidar2PointCloud", processing_info::ePROCESSING_TYPE::FLAT_POINT_CLOUD_GENERATION);
    }
    else
    {
        serializer.write("Lidar2PointCloud", processing_info::ePROCESSING_TYPE::POINT_CLOUD_GENERATION);
    }

    for (auto it = info.begin(); it != info.end(); ++it)
    {
        serializer.write(*it);
    }
}

//-----------------------------------------------------------------------------
void cFileProcessor::writeExperimentInfo(const cExperimentInfo& info, cExperimentSerializer& serializer)
{
    serializer.writeBeginHeader();

    if (!info.experimentTitle().empty())
        serializer.writeExperimentTitle(info.experimentTitle());

    if (!info.measurementTitle().empty())
        serializer.writeMeasurementTitle(info.measurementTitle());

    if (!info.experimentDoc().empty())
        serializer.writeExperimentDoc(info.experimentDoc());

    if (!info.comments().empty())
        serializer.writeComments(info.comments());

    if (!info.principalInvestigator().empty())
        serializer.writePrincipalInvestigator(info.principalInvestigator());

    if (!info.researchers().empty())
        serializer.writeResearchers(info.researchers());

    if (!info.species().empty())
        serializer.writeSpecies(info.species());

    if (!info.cultivar().empty())
        serializer.writeCultivar(info.cultivar());

    if (!info.construct().empty())
        serializer.writeConstructName(info.construct());

    if (!info.eventNumbers().empty())
        serializer.writeEventNumbers(info.eventNumbers());

    if (!info.treatments().empty())
        serializer.writeTreatments(info.treatments());

    if (!info.fieldDesign().empty())
        serializer.writeFieldDesign(info.fieldDesign());

    if (!info.permit().empty())
        serializer.writePermitInfo(info.permit());

    if (info.plantingDate().has_value())
    {
        nExpTypes::sDateDoy_t date = info.plantingDate().value();
        serializer.writePlantingDate(date.year, date.month, date.day, date.doy);
    }

    if (info.harvestDate().has_value())
    {
        nExpTypes::sDateDoy_t date = info.harvestDate().value();
        serializer.writeHarvestDate(date.year, date.month, date.day, date.doy);
    }

    if (info.fileDate().has_value())
    {
        nExpTypes::sDate_t date = info.fileDate().value();
        serializer.writeFileDate(date.year, date.month, date.day);
    }

    if (info.fileTime().has_value())
    {
        nExpTypes::sTime_t time = info.fileTime().value();
        serializer.writeFileTime(time.hour, time.minute, time.seconds);
    }

    if (info.dayOfYear().has_value())
        serializer.writeDayOfYear(info.dayOfYear().value());

    serializer.writeEndOfHeader();
}

