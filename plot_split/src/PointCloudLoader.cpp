
#include "PointCloudLoader.hpp"


cPointCloudLoader::cPointCloudLoader(std::list<cPointCloudInfo>& infoList)
    : mInfoList(infoList)
{
}

cPointCloudLoader::~cPointCloudLoader()
{}

void cPointCloudLoader::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system)
{
    mInfo.setCoordinateSystem(coordinate_system);
}

void cPointCloudLoader::onKinematicModel(pointcloud::eKINEMATIC_MODEL model)
{
    mInfo.setKinematicModel(model);
}

void cPointCloudLoader::onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg)
{
    mInfo.setSensorAngles(pitch_deg, roll_deg, yaw_deg);
}

void cPointCloudLoader::onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps)
{
    mInfo.setKinematicSpeed(vx_mps, vy_mps, vz_mps);
}

void cPointCloudLoader::onDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m)
{
}

void cPointCloudLoader::onImuData(pointcloud::imu_data_t data)
{
    mInfo.setImuData(data);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
    mInfo.setPointCloudData(frameID, timestamp_ns, pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud)
{
    mInfo.setPointCloudData(frameID, timestamp_ns, pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud)
{
    mInfo.setPointCloudData(frameID, timestamp_ns, pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    mInfo.setPointCloudData(frameID, timestamp_ns, pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud)
{
    mInfo.setPointCloudData(frameID, timestamp_ns, pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud)
{
    mInfo.setPointCloudData(frameID, timestamp_ns, pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(cPointCloud pointCloud)
{
    mInfo.setPointCloudData(pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(cPointCloud_FrameId pointCloud)
{
    mInfo.setPointCloudData(pointCloud);

    mInfoList.push_back(mInfo);
}

void cPointCloudLoader::onPointCloudData(cPointCloud_SensorInfo pointCloud)
{
    mInfo.setPointCloudData(pointCloud);

    mInfoList.push_back(mInfo);
}



