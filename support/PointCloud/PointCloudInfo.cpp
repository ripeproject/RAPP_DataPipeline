
#include "PointCloudInfo.hpp"

void sPointCloudBlock::clear()
{
	coordinate_system = pointcloud::eCOORDINATE_SYSTEM::SENSOR_SEU;
	kinematic_model = pointcloud::eKINEMATIC_MODEL::UNKNOWN;

	distanceWindow.reset();
	azimuthWindow.reset();
	altitudeWindow.reset();

	dimensions.reset();

	path.clear();
	pointCloud.clear();

	vegetationOnly = false;
}

void cPointCloudInfo::clear()
{
	mPointClouds.clear();
}

bool cPointCloudInfo::empty() const
{
	return mPointClouds.empty();
}

std::size_t cPointCloudInfo::size() const
{
	return mPointClouds.size();
}

cPointCloudInfo::const_reference cPointCloudInfo::front() const
{
	return mPointClouds.front();
}

cPointCloudInfo::const_iterator cPointCloudInfo::begin() const
{
	return mPointClouds.begin();
}

cPointCloudInfo::const_iterator cPointCloudInfo::end() const
{
	return mPointClouds.end();
}

void cPointCloudInfo::clearPointClouds()
{
	mPointClouds.clear();
}

void cPointCloudInfo::addPointCloudInfo(const sPointCloudBlock& info)
{
	mPointClouds.push_back(info);
}

