
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
	mHasBlockTag = true;
}

void cPointCloudInfoLoader::onEndPointCloudBlock()
{
	mInfo->addPointCloudInfo(mBlock);
	mBlock.clear();
}

void cPointCloudInfoLoader::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system)
{
	mBlock.coordinate_system = coordinate_system;
}

void cPointCloudInfoLoader::onKinematicModel(pointcloud::eKINEMATIC_MODEL model)
{
	mBlock.kinematic_model = model;
}

void cPointCloudInfoLoader::onDistanceWindow(double min_dist_m, double max_dist_m) 
{
	sDistanceWindow_t window = { min_dist_m, max_dist_m };
	mBlock.distanceWindow = window;
}

void cPointCloudInfoLoader::onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg)
{
	sAzimuthWindow_t window = { min_azimuth_deg, max_azimuth_deg };
	mBlock.azimuthWindow = window;
}

void cPointCloudInfoLoader::onAltitudeWindow(double min_altitude_deg, double max_altitude_deg)
{
	sAltitudeWindow_t window = { min_altitude_deg, max_altitude_deg };
	mBlock.altitudeWindow = window;
}

void cPointCloudInfoLoader::onReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm)
{
	sRappPoint_t point = { x_mm, y_mm, z_mm };
	mBlock.referencePoint = point;
}

void cPointCloudInfoLoader::onDimensions(double x_min_m, double x_max_m,
	double y_min_m, double y_max_m,
	double z_min_m, double z_max_m) 
{
	sPointCloudDimensions_t dimensions = { x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m };
	mBlock.dimensions = dimensions;
}

void cPointCloudInfoLoader::onBeginSensorKinematics()
{
	mBlock.path.clear();
}

void cPointCloudInfoLoader::onEndSensorKinematics()
{}

void cPointCloudInfoLoader::onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point)
{
	mBlock.path.push_back(point);
}

void cPointCloudInfoLoader::onPointCloudData(cPointCloud pointCloud)
{
	mBlock.pointCloud = pointCloud;

	if (!mHasBlockTag)
	{
		mInfo->addPointCloudInfo(mBlock);
		mBlock.clear();
	}
}

void cPointCloudInfoLoader::onPointCloudData(cPointCloud_FrameId pointCloud)
{
	mBlock.pointCloud = pointCloud;

	if (!mHasBlockTag)
	{
		mInfo->addPointCloudInfo(mBlock);
		mBlock.clear();
	}
}

void cPointCloudInfoLoader::onPointCloudData(cPointCloud_SensorInfo pointCloud)
{
	mBlock.pointCloud = pointCloud;

	if (!mHasBlockTag)
	{
		mInfo->addPointCloudInfo(mBlock);
		mBlock.clear();
	}
}

void cPointCloudInfoLoader::onBeginPointCloudList()
{
	mHasListTag = true;
	mInfo->clearPointClouds();
}

void cPointCloudInfoLoader::onEndPointCloudList()
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

