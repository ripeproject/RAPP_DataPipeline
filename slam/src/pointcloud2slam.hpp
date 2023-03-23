
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloud.hpp"
#include "Slam.h"

#include <cbdf/SpidercamParser.hpp>

#include <pcl/PCLHeader.h>

#include <filesystem>
#include <string>
#include <fstream>
#include <vector>


class cPointCloud2Slam : public cPointCloudParser, public cSpidercamParser
{
    using PointS = LidarSlam::Slam::Point;
    using CloudS = pcl::PointCloud<PointS>;  ///< Pointcloud needed by SLAM

public:
	cPointCloud2Slam();
	~cPointCloud2Slam();

    void setOutputPath(std::filesystem::path out);

private:
    //----------------------------------------------------------------------------
    /*!
     * @brief Get and fill Slam parameters from ROS parameters server.
     */
    void setSlamParameters();

    //----------------------------------------------------------------------------
    /*!
     * @brief Fill the SLAM initial state with the given initial maps, pose and
     *        landmarks.
     */
    void setSlamInitialState();


    //----------------------------------------------------------------------------
    /*!
     * @brief     Update transform offset between BASE and LIDAR using TF2
     * @param[in] lidarFrameId The input LiDAR pointcloud frame_id.
     * @param[in] lidarDeviceId The numerical identifier of the LiDAR sensor.
     */
    bool updateBaseToLidarOffset(uint32_t lidarFrameId, uint8_t lidarDeviceId);

private:
    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onImuData(pointcloud::imu_data_t data) override;
    void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(cPointCloud pointCloud) override;

    void onPosition(spidercam::sPosition_1_t pos) override;

private:
    std::filesystem::path mOutputPath;

    ///< LiDAR device identifier to set for each point.
    int mDeviceId = 0;
    uint32_t mLidarFrameId = 0;
    uint32_t mTrackingFrameId = 0;

    // Useful variables for approximate point-wise timestamps computation

    ///< Spinning speed of sensor [rpm]
    double mRpm = 600.0;

    ///< Wether timestamping is based on the first or last packet of each scan
    bool mTimestampFirstPacket = false;

    LidarSlam::Slam mLidarSlam;
//    PointS
//    pcl::PointCloud<LidarSlam::LidarPoint> mFrame;
    std::vector<CloudS::Ptr> mFrames;



    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };
    struct color3 { uint8_t r, g, b; };

    std::vector<float4>   mPositions;

    std::vector<float3>   mVertices;
    std::vector<uint32_t> mRanges;
    std::vector<uint3>    mReturns;
    std::vector<uint16_t> mFrameIDs;

    bool mResyncTimestamp = true;
	uint64_t mStartTimestamp_ns = 0;
};
