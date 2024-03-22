
#include "PointCloudLoader.hpp"


cPointCloudLoader::cPointCloudLoader(std::weak_ptr<cPointCloudInfo> info)
{
    mInfo = info.lock();
}

cPointCloudLoader::~cPointCloudLoader()
{}

void cPointCloudLoader::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system)
{
    mInfo->setCoordinateSystem(coordinate_system);
}

void cPointCloudLoader::onKinematicModel(pointcloud::eKINEMATIC_MODEL model)
{
    mInfo->setKinematicModel(model);
}

void cPointCloudLoader::onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg)
{
    mInfo->setSensorAngles(pitch_deg, roll_deg, yaw_deg);
}

void cPointCloudLoader::onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps)
{
    mInfo->setKinematicSpeed(vx_mps, vy_mps, vz_mps);
}

void cPointCloudLoader::onDistanceWindow(double min_dist_m, double max_dist_m)
{
    mInfo->setDistanceWindow(min_dist_m, max_dist_m);
}

void cPointCloudLoader::onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg)
{
    mInfo->setAzimuthWindow(min_azimuth_deg, max_azimuth_deg);
}

void cPointCloudLoader::onAltitudeWindow(double min_altitude_deg, double max_altitude_deg)
{
    mInfo->setAltitudeWindow(min_altitude_deg, max_altitude_deg);
}

void cPointCloudLoader::onDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m)
{
}

void cPointCloudLoader::onImuData(pointcloud::imu_data_t data)
{
    mInfo->setImuData(data);
}

void cPointCloudLoader::onBeginSensorKinematics()
{
    mInfo->clearSensorKinematics();
}

void cPointCloudLoader::onEndSensorKinematics()
{}

void cPointCloudLoader::onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point)
{
    mInfo->addSensorKinematicPoint(point);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
    mInfo->addPointCloudData(frameID, timestamp_ns, pointCloud);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud)
{
    mInfo->addPointCloudData(frameID, timestamp_ns, pointCloud);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud)
{
    mInfo->addPointCloudData(frameID, timestamp_ns, pointCloud);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    mInfo->addPointCloudData(frameID, timestamp_ns, pointCloud);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud)
{
    mInfo->addPointCloudData(frameID, timestamp_ns, pointCloud);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud)
{
    mInfo->addPointCloudData(frameID, timestamp_ns, pointCloud);
}

void cPointCloudLoader::onPointCloudData(cPointCloud pointCloud)
{
    mInfo->addPointCloudData(pointCloud);
}

void cPointCloudLoader::onPointCloudData(cPointCloud_FrameId pointCloud)
{
    mInfo->addPointCloudData(pointCloud);
}

void cPointCloudLoader::onPointCloudData(cPointCloud_SensorInfo pointCloud)
{
    mInfo->addPointCloudData(pointCloud);
}



