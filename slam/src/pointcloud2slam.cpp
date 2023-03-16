
#include "pointcloud2slam.hpp"
#include "PointCloudTypes.hpp"

#include "LidarPoint.h"
#include "Utilities.h"

#include <iostream>


//#define USE_BINARY


cPointCloud2Slam::cPointCloud2Slam() : cPointCloudParser()
{
}

cPointCloud2Slam::~cPointCloud2Slam()
{
}

void cPointCloud2Slam::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cPointCloud2Slam::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) {}
void cPointCloud2Slam::onImuData(pointcloud::imu_data_t data) {}

void cPointCloud2Slam::onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<float3>   vertices;
    std::vector<uint32_t> ranges;
    std::vector<uint3>    returns;
    std::vector<uint16_t> frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        float3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        uint3 data;
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


    ++mFrameCount;
}

void cPointCloud2Slam::onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    if (mResyncTimestamp)
    {
        mStartTimestamp_ns = timestamp_ns;
        mResyncTimestamp = false;
    }

    double time_sec = static_cast<double>(timestamp_ns - mStartTimestamp_ns) / 1'000'000'000.0;

    auto cloud_data = pointCloud.data();

    pcl::PointCloud<LidarSlam::LidarPoint> cloudS;

    cloudS.reserve(cloud_data.size());

    // Copy pointcloud metadata
    //BAF Utils::CopyPointCloudMetadata(cloudO, cloudS);

    // Check wether to use custom laser ID mapping or leave it untouched
    //BAF bool useLaserIdMapping = !this->LaserIdMapping.empty();

    // Helper to estimate frameAdvancement in case time field is invalid
//    Utils::SpinningFrameAdvancementEstimator frameAdvancementEstimator;

    // Build SLAM pointcloud
    for (const auto& ousterPoint : cloud_data)
    {
        ousterPoint.X_m
        // Remove no return points
        if (ousterPoint.getVector3fMap().norm() < 1e-3)
            continue;

        LidarSlam::LidarPoint slamPoint;
        slamPoint.x = ousterPoint.X_m;
        slamPoint.y = ousterPoint.Y_m;
        slamPoint.z = ousterPoint.Z_m;
        slamPoint.intensity = ousterPoint.reflectivity;
        slamPoint.laser_id = useLaserIdMapping ? this->LaserIdMapping[ousterPoint.ring] : ousterPoint.ring;
        slamPoint.device_id = mDeviceId;

        // Build approximate point-wise timestamp from azimuth angle
        // 'frameAdvancement' is 0 for first point, and should match 1 for last point
        // for a 360 degrees scan at ideal spinning frequency.
        // 'time' is the offset to add to 'header.stamp' to get approximate point-wise timestamp.
        // By default, 'header.stamp' is the timestamp of the last Veloydne packet,
        // but user can choose the first packet timestamp using parameter 'timestamp_first_packet'.
        double frameAdvancement = frameAdvancementEstimator(slamPoint);
        slamPoint.time = (this->TimestampFirstPacket ? frameAdvancement : frameAdvancement - 1) / this->Rpm * 60.;

        cloudS.push_back(slamPoint);
    }
}

void cPointCloud2Slam::onPointCloudData(cPointCloud pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<float3>   vertices;
    std::vector<uint32_t> ranges;
    std::vector<uint3>    returns;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        float3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        uint3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());

    ++mFrameCount;
}

void cPointCloud2Slam::onPosition(spidercam::sPosition_1_t pos)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = pos.X_mm / 1000.0;
    xyz.y = pos.Y_mm / 1000.0;
    xyz.z = pos.Z_mm / 1000.0;
    xyz.s = pos.speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

