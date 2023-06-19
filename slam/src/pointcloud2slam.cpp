
#include "pointcloud2slam.hpp"
#include "PointCloudTypes.hpp"

#include "LidarPoint.h"
#include "Utilities.h"

#include <iostream>
#include <memory>


//#define USE_BINARY
using namespace LidarSlam;

namespace
{
    inline bool IsZero(const pointcloud::sCloudPoint_t& point)
    {
        return (point.X_m == 0.0) && (point.Y_m == 0.0) && (point.Z_m == 0.0);
    }
}

cPointCloud2Slam::cPointCloud2Slam() : cPointCloudParser()
{
    // ***************************************************************************
    // Init SLAM state
    // Get SLAM params
    setSlamParameters();
    setSlamInitialState();
}

cPointCloud2Slam::~cPointCloud2Slam()
{
//    mLidarSlam.SaveMapsToPCD("McGrath_Soy1_");
}

void cPointCloud2Slam::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

//------------------------------------------------------------------------------
void cPointCloud2Slam::setSlamParameters()
{
#define SetSlamParam(type, rosParam, slamParam) { type val; if (this->PrivNh.getParam(rosParam, val)) this->LidarSlam.Set##slamParam(val); }
    // General
    mLidarSlam.SetTwoDMode(false);
    mLidarSlam.SetVerbosity(2);
    mLidarSlam.SetNbThreads(4);
    mLidarSlam.SetLoggingTimeout(4);
    mLidarSlam.SetLogOnlyKeyframes(true);

    auto egoMotion = LidarSlam::EgoMotionMode::MOTION_EXTRAPOLATION;
    mLidarSlam.SetEgoMotion(egoMotion);


    mLidarSlam.SetUndistortion(LidarSlam::UndistortionMode::REFINED);

    mLidarSlam.SetLoggingStorage(LidarSlam::PointCloudStorageType::PCL_CLOUD);

    // Frame Ids
    mLidarSlam.SetWorldFrameId("world");
    mLidarSlam.SetBaseFrameId("base");

    // Multi-LiDAR devices

    // Single LiDAR device
    auto ke = std::make_shared<LidarSlam::SpinningSensorKeypointExtractor>();

    ke->SetAzimuthalResolution_rad(0.0);
    ke->SetNbThreads(1);
    ke->SetMinNeighNb(4);
    ke->SetMinNeighRadius(0.05);
    ke->SetMinDistanceToSensor_m(1.5);
    ke->SetMinBeamSurfaceAngle_deg(10);
    ke->SetMinAzimuth_deg(0.0);
    ke->SetMaxAzimuth_deg(360.0);
    ke->SetPlaneSinAngleThreshold(0.5);
    ke->SetEdgeSinAngleThreshold(0.86);
    ke->SetEdgeDepthGapThreshold_m(0.5);
    ke->SetEdgeNbGapPoints(5);
    ke->SetEdgeIntensityGapThreshold(50.0);
    ke->SetMaxPoints(INT_MAX);
    ke->SetVoxelResolution_m(0.1);
    ke->SetInputSamplingRatio(1.0f);

    mLidarSlam.SetKeyPointsExtractor(ke);
    mLidarSlam.EnableKeypointType(LidarSlam::Keypoint::EDGE, true);
    mLidarSlam.EnableKeypointType(LidarSlam::Keypoint::INTENSITY_EDGE, true);
    mLidarSlam.EnableKeypointType(LidarSlam::Keypoint::PLANE, true);
    mLidarSlam.EnableKeypointType(LidarSlam::Keypoint::BLOB, true);

/*
    // Ego motion
    mLidarSlam.SetEgoMotionICPMaxIter();
    mLidarSlam.SetEgoMotionLMMaxIter();
    mLidarSlam.SetEgoMotionMaxNeighborsDistance();
    mLidarSlam.SetEgoMotionEdgeNbNeighbors();
    mLidarSlam.SetEgoMotionEdgeMinNbNeighbors();
    mLidarSlam.SetEgoMotionEdgeMaxModelError();
    mLidarSlam.SetEgoMotionPlaneNbNeighbors();
    mLidarSlam.SetEgoMotionPlanarityThreshold();
    mLidarSlam.SetEgoMotionPlaneMaxModelError();
    mLidarSlam.SetEgoMotionInitSaturationDistance();
    mLidarSlam.SetEgoMotionFinalSaturationDistance();

    // Localization
    mLidarSlam.SetLocalizationICPMaxIter();
    mLidarSlam.SetLocalizationLMMaxIter();
    mLidarSlam.SetLocalizationMaxNeighborsDistance();
    mLidarSlam.SetLocalizationEdgeNbNeighbors();
    mLidarSlam.SetLocalizationEdgeMinNbNeighbors();
    mLidarSlam.SetLocalizationEdgeMaxModelError();
    mLidarSlam.SetLocalizationPlaneNbNeighbors();
    mLidarSlam.SetLocalizationPlanarityThreshold();
    mLidarSlam.SetLocalizationPlaneMaxModelError();
    mLidarSlam.SetLocalizationBlobNbNeighbors();
    mLidarSlam.SetLocalizationInitSaturationDistance();
    mLidarSlam.SetLocalizationFinalSaturationDistance();

    // External sensors
    mLidarSlam.SetSensorMaxMeasures();
    mLidarSlam.SetSensorTimeThreshold();
    mLidarSlam.SetLandmarkWeight();
    mLidarSlam.SetLandmarkSaturationDistance();
    mLidarSlam.SetLandmarkPositionOnly();
    //this->LidarTimePosix = this->PrivNh.param("external_sensors/landmark_detector/lidar_is_posix", true);

    // Graph parameters
    mLidarSlam.SetG2oFileName();
    mLidarSlam.SetFixFirstVertex();
    mLidarSlam.SetFixLastVertex();
    mLidarSlam.SetCovarianceScale();
    mLidarSlam.SetNbGraphIterations();
*/

    // Confidence estimators
    // Overlap
//    mLidarSlam.SetOverlapSamplingRatio();

    // Motion limitations (hard constraints to detect failure();
    mLidarSlam.SetAccelerationLimit_mps2(2.0);
    mLidarSlam.SetRotationAccelLimit_dps2(10.0);

    mLidarSlam.SetVelocityLimit_mps(2.0);
    mLidarSlam.SetRotationRateLimit_dps(10.0);

    mLidarSlam.SetTimeWindowDuration_sec(1.0);

    // Keyframes
//    mLidarSlam.SetKfDistanceThreshold();
//    mLidarSlam.SetKfAngleThreshold();

/*
    // Maps
    int mapUpdateMode;
    if (this->PrivNh.getParam("slam/voxel_grid/update_maps", mapUpdateMode))
    {
        LidarSlam::MappingMode mapUpdate = static_cast<LidarSlam::MappingMode>(mapUpdateMode);
        if (mapUpdate != LidarSlam::MappingMode::NONE &&
            mapUpdate != LidarSlam::MappingMode::ADD_KPTS_TO_FIXED_MAP &&
            mapUpdate != LidarSlam::MappingMode::UPDATE)
        {
            ROS_ERROR_STREAM("Invalid map update mode (" << mapUpdateMode << "). Setting it to 'UPDATE'.");
            mapUpdate = LidarSlam::MappingMode::UPDATE;
        }
        mLidarSlam.SetMapUpdate(mapUpdate);
    }
    double size = 0.0;
    if (this->PrivNh.getParam("slam/voxel_grid/leaf_size/edges", size) && mLidarSlam.KeypointTypeEnabled(LidarSlam::EDGE))
        mLidarSlam.SetVoxelGridLeafSize(LidarSlam::EDGE, size);
    if (this->PrivNh.getParam("slam/voxel_grid/leaf_size/intensity_edges", size) && mLidarSlam.KeypointTypeEnabled(LidarSlam::INTENSITY_EDGE))
        mLidarSlam.SetVoxelGridLeafSize(LidarSlam::INTENSITY_EDGE, size);
    if (this->PrivNh.getParam("slam/voxel_grid/leaf_size/planes", size) && mLidarSlam.KeypointTypeEnabled(LidarSlam::PLANE))
        mLidarSlam.SetVoxelGridLeafSize(LidarSlam::PLANE, size);
    if (this->PrivNh.getParam("slam/voxel_grid/leaf_size/blobs", size) && mLidarSlam.KeypointTypeEnabled(LidarSlam::BLOB))
        mLidarSlam.SetVoxelGridLeafSize(LidarSlam::BLOB, size);

    mLidarSlam.SetVoxelGridResolution();
    mLidarSlam.SetVoxelGridSize();
    mLidarSlam.SetVoxelGridDecayingThreshold();
    mLidarSlam.SetVoxelGridMinFramesPerVoxel();
*/

    for (auto k : LidarSlam::KeypointTypes)
    {
        if (!mLidarSlam.KeypointTypeEnabled(k))
           continue;

        int samplingMode;

        // if (this->PrivNh.getParam("slam/voxel_grid/sampling_mode/" + LidarSlam::KeypointTypeNames.at(k), samplingMode))
        if (false)
        {
            LidarSlam::SamplingMode sampling = static_cast<LidarSlam::SamplingMode>(samplingMode);
            if (sampling != LidarSlam::SamplingMode::FIRST &&
                    sampling != LidarSlam::SamplingMode::LAST &&
                    sampling != LidarSlam::SamplingMode::MAX_INTENSITY &&
                    sampling != LidarSlam::SamplingMode::CENTER_POINT &&
                    sampling != LidarSlam::SamplingMode::CENTROID)
            {
               sampling = LidarSlam::SamplingMode::MAX_INTENSITY;
            }
            mLidarSlam.SetVoxelGridSamplingMode(k, sampling);
        }
    }
}

//------------------------------------------------------------------------------
void cPointCloud2Slam::setSlamInitialState()
{
    // Load initial SLAM maps if requested
    // mLidarSlam.LoadMapsFromPCD("");

    // Load initial Landmarks poses if requested
    std::string lmpath = "";
    if (!lmpath.empty())
    {
        //LoadLandmarks(lmpath);
        mLidarSlam.SetLandmarkConstraintLocal(false);
    }
    else
        mLidarSlam.SetLandmarkConstraintLocal(true);

    // Set initial SLAM pose if requested
    std::vector<double> initialPose;
    if (initialPose.size() == 6)
    {
        Eigen::Isometry3d poseTransform = LidarSlam::Utils::XYZRPYtoIsometry(initialPose.data());
        mLidarSlam.SetWorldTransformFromGuess(poseTransform);
    }
}

//==============================================================================
//   Utilities
//==============================================================================

//------------------------------------------------------------------------------
bool cPointCloud2Slam::updateBaseToLidarOffset(uint32_t lidarFrameId, uint8_t lidarDeviceId)
{
    // If tracking frame is different from input frame, get TF from LiDAR to BASE
    if (lidarFrameId != mTrackingFrameId)
    {
        // We expect a static transform between BASE and LIDAR, so we don't care
        // about timestamp and get only the latest transform
        Eigen::Isometry3d baseToLidar;
//        if (Utils::Tf2LookupTransform(baseToLidar, this->TfBuffer, mTrackingFrameId, lidarFrameId))
        if (false)
            mLidarSlam.SetBaseToLidarOffset(baseToLidar, lidarDeviceId);
        else
            return false;
    }
    return true;
}

/*----------------------------------------------------------------------------
 *  Called by cPointCloudParser
 *---------------------------------------------------------------------------*/
void cPointCloud2Slam::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) {}
void cPointCloud2Slam::onKinematicModel(pointcloud::eKINEMATIC_MODEL model) {}
void cPointCloud2Slam::onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) {}
void cPointCloud2Slam::onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) {}

void cPointCloud2Slam::onDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m) {}

void cPointCloud2Slam::onImuData(pointcloud::imu_data_t data) {}

void cPointCloud2Slam::onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
}

void cPointCloud2Slam::onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    static double delta_t_us = 100'000.0 / 1024.0;

    if (mResyncTimestamp)
    {
        mStartTimestamp_ns = timestamp_ns;
        mResyncTimestamp = false;
    }

    std::cout << "Frame ID: " << mLidarFrameId << "\n";

    double time_us = static_cast<double>(timestamp_ns - mStartTimestamp_ns) / 1'000.0;

    auto channelsPerColumn = pointCloud.channelsPerColumn();
    auto columnsPerFrame = pointCloud.columnsPerFrame();

    auto frame = std::make_shared<CloudS>();

    frame->reserve(pointCloud.size());
    frame->header.seq = mLidarFrameId++;
    frame->header.stamp = time_us;
    frame->header.frame_id = std::to_string(frameID);

    // Build SLAM pointcloud
    for(std::size_t c = 0; c < columnsPerFrame; ++c)
    {
        double time_sec = (time_us + c * delta_t_us) / 1'000'000.0;
        auto column = pointCloud.channels(c);

        for (std::size_t n = 0; n < channelsPerColumn; ++n)
        {
            auto ousterPoint = column[n];

            // Remove no return points
            if (IsZero(ousterPoint))
                continue;

            LidarSlam::LidarPoint slamPoint;
            slamPoint.x = ousterPoint.X_m;
            slamPoint.y = ousterPoint.Y_m;
            slamPoint.z = ousterPoint.Z_m;
            slamPoint.intensity = ousterPoint.reflectivity;
            slamPoint.laser_id = n;
            slamPoint.device_id = mDeviceId;
            slamPoint.time = time_sec;

            frame->push_back(slamPoint);
        }
    }


    // Update TF from BASE to LiDAR
//    if (!updateBaseToLidarOffset(cloudS_ptr->header.frame_id, cloudS_ptr->front().device_id))
//        return;

    // Set the SLAM main input frame at first position
//    mFrames.insert(mFrames.begin(), pFrame);

    // Run SLAM : register new frame and update localization and map.
    mLidarSlam.AddFrame(frame);
}

void cPointCloud2Slam::onPointCloudData(cPointCloud pointCloud)
{
}

/*----------------------------------------------------------------------------
 *  Called by cSpidercamParser
 *---------------------------------------------------------------------------*/
void cPointCloud2Slam::onPosition(spidercam::sPosition_1_t pos)
{
//    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = pos.X_mm / 1000.0;
    xyz.y = pos.Y_mm / 1000.0;
    xyz.z = pos.Z_mm / 1000.0;
    xyz.s = pos.speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

/*----------------------------------------------------------------------------
 *  Called by cSsnxParser
 *---------------------------------------------------------------------------*/
void cPointCloud2Slam::onPVT_Cartesian(ssnx::gps::PVT_Cartesian_1_t pos) {}
void cPointCloud2Slam::onPVT_Cartesian(ssnx::gps::PVT_Cartesian_2_t pos) {}
void cPointCloud2Slam::onPVT_Geodetic(ssnx::gps::PVT_Geodetic_1_t pos) {}

void cPointCloud2Slam::onPVT_Geodetic(ssnx::gps::PVT_Geodetic_2_t pos) 
{
    if (!pos.dataValid) return;

    auto n = pos.Ve_mps;
}

void cPointCloud2Slam::onPosCovGeodetic(ssnx::gps::PosCovGeodetic_1_t cov)
{
    if (!cov.dataValid) return;

    auto n = cov.Cov_latlat_m2;
}

void cPointCloud2Slam::onVelCovGeodetic(ssnx::gps::VelCovGeodetic_1_t cov)
{
    if (!cov.dataValid) return;

    auto n = cov.Cov_VnVn_m2ps2;
}

void cPointCloud2Slam::onDOP(ssnx::gps::DOP_1_t dop) {}

void cPointCloud2Slam::onPVT_Residuals(ssnx::gps::PVT_Residuals_1_t residuals) 
{
    if (!residuals.dataValid) return;
}

void cPointCloud2Slam::onRAIMStatistics(ssnx::gps::RAIMStatistics_1_t raim) {}
void cPointCloud2Slam::onPOS_Local(ssnx::gps::POS_Local_1_t pos) {}
void cPointCloud2Slam::onPOS_Projected(ssnx::gps::POS_Projected_1_t pos) {}
void cPointCloud2Slam::onReceiverTime(ssnx::gps::ReceiverTime_1_t time) {}
void cPointCloud2Slam::onRtcmDatum(ssnx::gps::RtcmDatum_1_t datum) {}

