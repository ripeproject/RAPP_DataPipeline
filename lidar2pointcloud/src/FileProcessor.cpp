
#include "FileProcessor.hpp"
#include "lidar2pointcloud.hpp"
#include "StringUtils.hpp"
#include "FieldUtils.hpp"
#include "ExportUtils.hpp"
#include "GroundModelUtils.hpp"
#include "PointCloudSaver.hpp"

#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>
#include <cbdf/ExperimentSerializer.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id);


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

void cFileProcessor::setDefaults(const nConfigFileData::sParameters_t& defaults)
{
    mDefaults = defaults;
}

void cFileProcessor::process_file()
{
    mHasGroundData = is_ground_data_loaded();

    std::unique_ptr<cLidar2PointCloud> converter = std::make_unique<cLidar2PointCloud>(mID);

    converter->setValidRange_m(mDefaults.minDistance_m, mDefaults.maxDistance_m);
    converter->setAzimuthWindow_deg(mDefaults.minAzimuth_deg, mDefaults.maxAzimuth_deg);
    converter->setAltitudeWindow_deg(mDefaults.minAltitude_deg, mDefaults.maxAltitude_deg);

    converter->setInitialPosition_m(mDefaults.startX_m, mDefaults.startY_m, mDefaults.startZ_m);
    converter->setFinalPosition_m(mDefaults.endX_m, mDefaults.endY_m, mDefaults.endZ_m);

    converter->setDollySpeed(mDefaults.Vx_mmps, mDefaults.Vy_mmps, mDefaults.Vz_mmps);

    converter->setSensorMountOrientation(mDefaults.sensorMountYaw_deg, mDefaults.sensorMountPitch_deg, mDefaults.sensorMountRoll_deg);

    if ((mDefaults.startPitchOffset_deg == mDefaults.endPitchOffset_deg)
        && (mDefaults.startRollOffset_deg == mDefaults.endRollOffset_deg)
        && (mDefaults.startYawOffset_deg == mDefaults.endYawOffset_deg))
    {
        converter->setOrientationOffset_deg(mDefaults.sensorYawOffset_deg, mDefaults.sensorPitchOffset_deg, mDefaults.sensorRollOffset_deg);
    }
    else
    {
        converter->setInitialOffset_deg(mDefaults.startYawOffset_deg, mDefaults.startPitchOffset_deg, mDefaults.startRollOffset_deg);
        converter->setFinalOffset_deg(mDefaults.endYawOffset_deg, mDefaults.endPitchOffset_deg, mDefaults.endRollOffset_deg);
    }

    converter->enableTranslateToGround(mHasGroundData && mDefaults.translateToGround, mDefaults.translateThreshold_pct);
    converter->enableRotateToGround(mHasGroundData && mDefaults.rotateToGround, mDefaults.rotateThreshold_pct);

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

    cRappPointCloud pointCloud = converter->getPointCloud();
    
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
        saver->setRangeWindow_m(mDefaults.minDistance_m, mDefaults.maxDistance_m);
        saver->setAzimuthWindow_deg(mDefaults.minAzimuth_deg, mDefaults.maxAzimuth_deg);
        saver->setAltitudeWindow_deg(mDefaults.minAltitude_deg, mDefaults.maxAltitude_deg);

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

    cExperimentSerializer       experimentSerializer;
    cProcessingInfoSerializer   processInfoSerializer;
    cPointCloudSerializer 	    pointCloudSerializer;

    experimentSerializer.setBufferCapacity(4096 * 1024);
    processInfoSerializer.setBufferCapacity(4096);

    experimentSerializer.attach(&fileWriter);
    processInfoSerializer.attach(&fileWriter);
    pointCloudSerializer.attach(&fileWriter);

    fileWriter.open(filename);
    if (!fileWriter.isOpen())
        return;

    writeProcessingInfo(*processingInfo, processInfoSerializer);
    update_progress(mID, 33);

    writeExperimentInfo(*experimentInfo, experimentSerializer);
    update_progress(mID, 67);

    pointCloudSerializer.writeBeginPointCloudBlock();

    pointCloudSerializer.write(pointcloud::eCOORDINATE_SYSTEM::SENSOR_SEU);

    pointCloudSerializer.write(data.getKinematicModel());

    pointCloudSerializer.writeDistanceWindow(mDefaults.minDistance_m, mDefaults.maxDistance_m);
    pointCloudSerializer.writeAzimuthWindow(mDefaults.minAzimuth_deg, mDefaults.maxAzimuth_deg);
    pointCloudSerializer.writeAltitudeWindow(mDefaults.minAltitude_deg, mDefaults.maxAltitude_deg);

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

    if (!info.title().empty())
        serializer.writeTitle(info.title());

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

