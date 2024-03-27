
#include "PointCloudInfoLoader.hpp"
#include "PointCloudInfo.hpp"


cPointCloudInfoLoader::cPointCloudInfoLoader(std::weak_ptr<cPointCloudInfo> info)
{
    mInfo = info.lock();
}

cPointCloudInfoLoader::~cPointCloudInfoLoader()
{}

void cPointCloudInfoLoader::onBeginPointCloudBlock()
{

}

void cPointCloudInfoLoader::onEndPointCloudBlock()
{

}

void cPointCloudInfoLoader::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param)
{}

void cPointCloudInfoLoader::onKinematicModel(pointcloud::eKINEMATIC_MODEL model)
{}

void cPointCloudInfoLoader::onDistanceWindow(double min_dist_m, double max_dist_m) 
{}

void cPointCloudInfoLoader::onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg)
{}

void cPointCloudInfoLoader::onAltitudeWindow(double min_altitude_deg, double max_altitude_deg)
{}

void cPointCloudInfoLoader::onDimensions(double x_min_m, double x_max_m,
	double y_min_m, double y_max_m,
	double z_min_m, double z_max_m) 
{}

void cPointCloudInfoLoader::onBeginSensorKinematics()
{}

void cPointCloudInfoLoader::onEndSensorKinematics()
{}

void cPointCloudInfoLoader::onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point)
{}

void cPointCloudInfoLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
	cReducedPointCloudByFrame pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
	cReducedPointCloudByFrame_FrameId pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
	cReducedPointCloudByFrame_SensorInfo pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
	cSensorPointCloudByFrame pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
	cSensorPointCloudByFrame_FrameId pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
	cSensorPointCloudByFrame_SensorInfo pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(cPointCloud pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(cPointCloud_FrameId pointCloud)
{}

void cPointCloudInfoLoader::onPointCloudData(cPointCloud_SensorInfo pointCloud)
{}

