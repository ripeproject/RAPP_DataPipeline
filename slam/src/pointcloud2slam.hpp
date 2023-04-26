
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloud.hpp"
#include "Slam.h"

#include <cbdf/SpidercamParser.hpp>
#include <cbdf/SsnxParser.hpp>

#include <pcl/PCLHeader.h>

#include <filesystem>
#include <string>
#include <fstream>
#include <vector>


class cPointCloud2Slam : 
    public cPointCloudParser,   // <-- Read pointcloud data from ceres file
    public cSpidercamParser,    // <-- Read SpiderCam control/status data from ceres file
    public cSsnxParser          // <-- Read SSNX (gps) data from ceres file
{
    using PointS = LidarSlam::Slam::Point;
    using CloudS = pcl::PointCloud<PointS>;  ///< Pointcloud needed by SLAM

public:
	cPointCloud2Slam();
	~cPointCloud2Slam();

    void setOutputPath(std::filesystem::path out);

    uint32_t mLidarFrameId = 0;

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
    /*----------------------------------------------------------------------------
     *  Called by cPointCloudParser
     */
    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onImuData(pointcloud::imu_data_t data) override;
    void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(cPointCloud pointCloud) override;

    /*----------------------------------------------------------------------------
     *  Called by cSpidercamParser
     */
    void onPosition(spidercam::sPosition_1_t pos) override;

    /*----------------------------------------------------------------------------
     *  Called by cSsnxParser
     */
    void onPVT_Cartesian(ssnx::gps::PVT_Cartesian_1_t pos) override;
    void onPVT_Cartesian(ssnx::gps::PVT_Cartesian_2_t pos) override;
    void onPVT_Geodetic(ssnx::gps::PVT_Geodetic_1_t pos) override;
    void onPVT_Geodetic(ssnx::gps::PVT_Geodetic_2_t pos) override;
    void onPosCovGeodetic(ssnx::gps::PosCovGeodetic_1_t cov) override;
    void onVelCovGeodetic(ssnx::gps::VelCovGeodetic_1_t cov) override;
    void onDOP(ssnx::gps::DOP_1_t dop) override;
    void onPVT_Residuals(ssnx::gps::PVT_Residuals_1_t residuals) override;
    void onRAIMStatistics(ssnx::gps::RAIMStatistics_1_t raim) override;
    void onPOS_Local(ssnx::gps::POS_Local_1_t pos) override;
    void onPOS_Projected(ssnx::gps::POS_Projected_1_t pos) override;
    void onReceiverTime(ssnx::gps::ReceiverTime_1_t time) override;
    void onRtcmDatum(ssnx::gps::RtcmDatum_1_t datum) override;

private:
    std::filesystem::path mOutputPath;

    ///< LiDAR device identifier to set for each point.
    int mDeviceId = 0;
    //uint32_t mLidarFrameId = 0;
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
