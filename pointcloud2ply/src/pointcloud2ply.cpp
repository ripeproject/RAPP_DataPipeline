
#include "pointcloud2ply.hpp"
#include "PointCloudTypes.hpp"

#include <tinyply.h>

#include <algorithm>
#include <numbers>
#include <cmath>
#include <valarray>
#include <iostream>


cPointCloud2Ply::cPointCloud2Ply() : cPointCloudParser()
{}

cPointCloud2Ply::~cPointCloud2Ply()
{}

void cPointCloud2Ply::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cPointCloud2Ply::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) {}
void cPointCloud2Ply::onImuData(pointcloud::imu_data_t data) {}
void cPointCloud2Ply::onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) {}

void cPointCloud2Ply::onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    mFrameID = frameID;
    auto cloud_data = pointCloud.data();

    mVertices.clear();
    mRanges.clear();
    mReturns.clear();
    mFrameIDs.clear();

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        float3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        mVertices.push_back(xyz);

        mRanges.push_back(point.range_mm);

        uint3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        mReturns.push_back(data);

        mFrameIDs.push_back(mFrameID);
    }

    writePlyFile();
    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(cPointCloud pointCloud) {}

void cPointCloud2Ply::writePlyFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = std::to_string(mFrameCount);
    ext += ".ply";

    filename.replace_extension(ext);

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

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::UINT32, mRanges.size(), reinterpret_cast<uint8_t*>(mRanges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::UINT16, mReturns.size(), reinterpret_cast<uint8_t*>(mReturns.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "frame_id" },
        Type::UINT16, mFrameIDs.size(), reinterpret_cast<uint8_t*>(mFrameIDs.data()), Type::INVALID, 0);

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

