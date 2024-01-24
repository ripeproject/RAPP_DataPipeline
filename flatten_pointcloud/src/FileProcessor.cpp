
#include "FileProcessor.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include "FieldTopography.hpp"
#include "DataModelUtils.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <algorithm>


extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id);

namespace
{
    template<typename POINT>
    void save_flatten_to_pointcloud(const cRappPointCloud& in, cBasePointCloud<POINT>& out)
    {
        assert(in.size() == out.size());

        double x_min_m = in.minX_mm() * nConstants::MM_TO_M;
        double x_max_m = in.maxX_mm() * nConstants::MM_TO_M;
        double y_min_m = in.minY_mm() * nConstants::MM_TO_M;
        double y_max_m = in.maxY_mm() * nConstants::MM_TO_M;
        double z_min_m = in.minZ_mm() * nConstants::MM_TO_M;
        double z_max_m = in.maxZ_mm() * nConstants::MM_TO_M;

        out.setExtents(x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m);

//        std::cout << "Minimum z is " << z_min_m << std::endl;

        auto n = out.size();
        for (std::size_t i = 0; i < n; ++i)
        {
            auto& p1 = out[i];
            auto p2 = in[i];

            p1.X_m = p2.x_mm * nConstants::MM_TO_M;
            p1.Y_m = p2.y_mm * nConstants::MM_TO_M;
            p1.Z_m = p2.z_mm * nConstants::MM_TO_M;
        }
    }
}


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id)
{
    mInputFile = in;
    mOutputFile = out;

    mPointCloudSerializer.setVersion(1, 0);
}

cFileProcessor::~cFileProcessor()
{
    mPointCloud.clear();
    mFileWriter.close();
    mFileReader.close();
}

bool cFileProcessor::open()
{
    if (std::filesystem::exists(mOutputFile))
    {
        return false;
    }

    mFileWriter.open(mOutputFile.string());
    mFileReader.open(mInputFile.string());

    mFileSize = mFileReader.file_size();

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open())
    {
        new_file_progress(mID, mInputFile.string());

        run();
    }
}

void cFileProcessor::run()
{
	if (!mFileReader.isOpen())
	{
        throw std::logic_error("No file is open for reading.");
	}

    int last_file_pos_pct = 0;

    mFileReader.attach(this);

    mInfoSerializer.attach(&mFileWriter);
    mPointCloudSerializer.attach(&mFileWriter);

    mInfoSerializer.write("flatten_pointcloud", processing_info::ePROCESSING_TYPE::FLAT_POINT_CLOUD_GENERATION);

    try
    {
	    mFileReader.registerCallback([this](const cBlockID& id){ this->processBlock(id); });
	    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len){ this->processBlock(id, buf, len); });

        update_prefix_progress(mID, "Scanning...              ", 0);

        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                mFileWriter.close();

                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            mFilePos = 100.0 * (file_pos / mFileSize);

            if (static_cast<int>(mFilePos) != last_file_pos_pct)
            {
                update_progress(mID, static_cast<int>(mFilePos));
                last_file_pos_pct = static_cast<int>(mFilePos);
            }
        }
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = "Stream Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = "CRC Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = "Unexpected EOF: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const std::exception& e)
    {
        std::string msg = "Unknown Exception: ";
        msg += e.what();
        console_message(msg);
    }

    update_prefix_progress(mID, "       Complete          ", 100);
    complete_file_progress(mID);
}

void cFileProcessor::processBlock(const cBlockID& id)
{
	mFileWriter.writeBlock(id);
}

void cFileProcessor::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
	mFileWriter.writeBlock(id, buf, len);
}


void cFileProcessor::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param)
{
    mPointCloudSerializer.write(config_param);
}

void cFileProcessor::onKinematicModel(pointcloud::eKINEMATIC_MODEL model)
{
    mPointCloudSerializer.write(model);
}

void cFileProcessor::onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) 
{
    mPointCloudSerializer.writeSensorAngles(pitch_deg, roll_deg, yaw_deg);
}

void cFileProcessor::onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) 
{
    mPointCloudSerializer.writeKinematicSpeed(vx_mps, vy_mps, vz_mps);
}

void cFileProcessor::onDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m) 
{
}

void cFileProcessor::onImuData(pointcloud::imu_data_t data)
{
    mPointCloudSerializer.write(data);
}

void cFileProcessor::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
/*
    cPointCloudParser* parser = static_cast<cPointCloudParser*>(this);
    parser->blockID().majorVersion();
    parser->blockID().minorVersion();
*/

    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(cPointCloud pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(cPointCloud_FrameId pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

void cFileProcessor::onPointCloudData(cPointCloud_SensorInfo pointCloud)
{
    mPointCloud = cRappPointCloud(pointCloud);
    flattenPointCloud();

    save_flatten_to_pointcloud(mPointCloud, pointCloud);

    mPointCloudSerializer.writeDimensions(pointCloud.minX_m(), pointCloud.maxX_m(),
        pointCloud.minY_m(), pointCloud.maxY_m(), pointCloud.minZ_m(), pointCloud.maxZ_m());

    mPointCloudSerializer.write(pointCloud);
}

//-----------------------------------------------------------------------------
void cFileProcessor::flattenPointCloud()
{
    std::size_t i = 0;
    std::size_t n = mPointCloud.size();
    int last_pct = 0;

    // First, for each point in the point cloud, find the corresponding ground point

    update_prefix_progress(mID, "Filling Ground Points    ", 0);
    for (auto& point : mPointCloud)
    {
        ++i;

        int pos_pct = static_cast<int>((100.0 * i) / n);

        if (pos_pct > last_pct)
        {
            update_progress(mID, pos_pct);
            last_pct = pos_pct;
        }

        double h = getMeshHeight_mm(point.x_mm, point.y_mm);
        point.h_mm = h;
    }

    // Second, use lower 5% of the point cloud to translate the cloud to the ground
    translatePcToGroundMesh(5.0);

    // Third, use lower 5% of the point cloud to rotate the cloud to match the ground
    rotatePcToGroundMesh(5.0);

    // Fourth, use lower 5% of the point cloud to translate the cloud to the ground
    translatePcToGroundMesh(5.0);

    // Fifth, use lower 5% of the point cloud to rotate the cloud to match the ground
    rotatePcToGroundMesh(5.0);

    // Fourth, use lower 5% of the point cloud to translate the cloud to the ground
    translatePcToGroundMesh(50.0);

    // Sixth, adjust each point so that the height is AGL
    i = 0;
    n = mPointCloud.size();
    last_pct = 0;

    update_prefix_progress(mID, "Flattening Point Cloud   ", 0);
    for (auto& point : mPointCloud)
    {
        ++i;

        int pos_pct = static_cast<int>((100.0 * i) / n);

        if (pos_pct > last_pct)
        {
            update_progress(mID, pos_pct);
            last_pct = pos_pct;
        }

        if (point.h_mm == rfm::INVALID_HEIGHT)
            continue;

        point.z_mm -= point.h_mm;
    }

    update_prefix_progress(mID, "Scanning...              ", static_cast<int>(mFilePos));
}

//-----------------------------------------------------------------------------
void cFileProcessor::translatePcToGroundMesh(double threashold_pct)
{
    update_prefix_progress(mID, "Translating Point Cloud  ", 0);

    double offset = computePcToGroundMeshDistance_mm(mPointCloud, threashold_pct);

    update_progress(mID, 50);

    mPointCloud.translate(0, 0, offset);

    update_progress(mID, 100);
}

//-----------------------------------------------------------------------------
void cFileProcessor::rotatePcToGroundMesh(double threashold_pct)
{
    update_prefix_progress(mID, "Rotating Point Cloud     ", 0);
    auto angles = computePcToGroundMeshRotation_deg(mPointCloud, threashold_pct);

    mPointCloud.rotate(0, angles.pitch_deg, angles.roll_deg);
}
