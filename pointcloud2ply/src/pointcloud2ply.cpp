
#include "pointcloud2ply.hpp"
#include "PointCloudTypes.hpp"

#include <tinyply.h>

#include <iostream>


//#define USE_BINARY

bool cPointCloud2Ply::mIndividualPlyFiles = false;
bool cPointCloud2Ply::mSaveDollyPositions = false;


cPointCloud2Ply::cPointCloud2Ply() : cPointCloudParser()
{
    uint8_t R = static_cast<uint8_t>(rand() % 255);
    uint8_t G = static_cast<uint8_t>(rand() % 255);
    uint8_t B = static_cast<uint8_t>(rand() % 255);

    mColor = { R, G, B };
}

cPointCloud2Ply::~cPointCloud2Ply()
{
    if (!mVertices.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext;

        if (mIndividualPlyFiles)
            ext += std::to_string(mFrameCount);

        ext += ".ply";

        filename += ext;
//        filename.replace_extension(ext);

        writePointcloud(filename);
    }


    if (!mPositions.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = ".position.ply";

        filename.replace_extension(ext);

        writePosition(filename);
    }
}

void cPointCloud2Ply::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cPointCloud2Ply::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) {}
void cPointCloud2Ply::onKinematicModel(pointcloud::eKINEMATIC_MODEL model) {}
void cPointCloud2Ply::onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) {}
void cPointCloud2Ply::onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) {}

void cPointCloud2Ply::onDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m) {}

void cPointCloud2Ply::onImuData(pointcloud::imu_data_t data) {}

void cPointCloud2Ply::onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3>  vertices;
    std::vector<range_t>    ranges;
    std::vector<returns3>   returns;
    std::vector<uint16_t>   frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext;

        if (mIndividualPlyFiles)
        {
           ext = std::to_string(mFrameCount);
           ext += ".";
        }

        ext += "ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    if (mResyncTimestamp)
    {
        mStartTimestamp_ns = timestamp_ns;
        mResyncTimestamp = false;
    }

    double time_sec = static_cast<double>(timestamp_ns - mStartTimestamp_ns) / 1'000'000'000.0;

    auto cloud_data = pointCloud.data();

    std::vector<position3>  vertices;
    std::vector<range_t>    ranges;
    std::vector<returns3>   returns;
    std::vector<uint16_t>   frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());


    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(cPointCloud pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPosition(spidercam::sPosition_1_t pos)
{
    mResyncTimestamp = true;

    if (mSaveDollyPositions)
    {
        position4 xyz;
        xyz.x = pos.X_mm / 1000.0;
        xyz.y = pos.Y_mm / 1000.0;
        xyz.z = pos.Z_mm / 1000.0;
        xyz.s = pos.speed_mmps / 1000.0;

        mPositions.push_back(xyz);
        mColors.push_back(mColor);
    }
}

void cPointCloud2Ply::writePosition(std::filesystem::path filename)
{
    using namespace tinyply;

#ifdef USE_BINARY
    std::filebuf binary_buffer;
    binary_buffer.open(filename, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&binary_buffer);
    if (outstream_binary.fail())
        throw std::runtime_error("failed to open " + filename.string());
#else
    std::filebuf fb_ascii;
    fb_ascii.open(filename, std::ios::out);
    std::ostream outstream_ascii(&fb_ascii);
    if (outstream_ascii.fail())
        throw std::runtime_error("failed to open " + filename.string());
#endif

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z", "s" },
        Type::FLOAT32, mPositions.size(), reinterpret_cast<uint8_t*>(mPositions.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "red", "green", "blue" },
        Type::UINT8, mColors.size(), reinterpret_cast<uint8_t*>(mColors.data()), Type::INVALID, 0);

#ifdef USE_BINARY
    // Write a binary file
    ply_file.write(outstream_binary, true);
#else
    // Write an ASCII file
    ply_file.write(outstream_ascii, false);
#endif

    mPositions.clear();
}

void cPointCloud2Ply::writePointcloud(std::filesystem::path filename)
{
    using namespace tinyply;

#ifdef USE_BINARY
    std::filebuf binary_buffer;
    binary_buffer.open(filename, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&binary_buffer);
    if (outstream_binary.fail())
        throw std::runtime_error("failed to open " + filename.string());
#else
    std::filebuf fb_ascii;
    fb_ascii.open(filename, std::ios::out);
    std::ostream outstream_ascii(&fb_ascii);
    if (outstream_ascii.fail()) 
        throw std::runtime_error("failed to open " + filename.string());
#endif

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
        Type::FLOAT32, mVertices.size(), reinterpret_cast<uint8_t*>(mVertices.data()), Type::INVALID, 0);

#ifdef USE_FLOATS

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::FLOAT32, mRanges.size(), reinterpret_cast<uint8_t*>(mRanges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::FLOAT32, mReturns.size(), reinterpret_cast<uint8_t*>(mReturns.data()), Type::INVALID, 0);

#else

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::UINT32, mRanges.size(), reinterpret_cast<uint8_t*>(mRanges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::UINT16, mReturns.size(), reinterpret_cast<uint8_t*>(mReturns.data()), Type::INVALID, 0);

#endif

    if (!mFrameIDs.empty())
    {
        ply_file.add_properties_to_element("vertex", { "frame_id" },
            Type::UINT16, mFrameIDs.size(), reinterpret_cast<uint8_t*>(mFrameIDs.data()), Type::INVALID, 0);
    }

#ifdef USE_BINARY
    // Write a binary file
    ply_file.write(outstream_binary, true);
#else
    // Write an ASCII file
    ply_file.write(outstream_ascii, false);
#endif

    mVertices.clear();
    mRanges.clear();
    mReturns.clear();
    mFrameIDs.clear();
}

