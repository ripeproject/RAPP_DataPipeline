
#include "lidar2pointcloud.hpp"

#include <ouster/simple_blas.h>

#include <algorithm>
#include <numbers>

double cLidar2PointCloud::mMinDistance_m = 0.0;
double cLidar2PointCloud::mMaxDistance_m = 1000.0;
ouster::cRotationMatrix<double> cLidar2PointCloud::mSensorToENU;

void cLidar2PointCloud::setValidRange_m(double min_dist_m, double max_dist_m)
{
	mMinDistance_m = std::max(min_dist_m, 0.0);
	mMaxDistance_m = std::min(max_dist_m, 1000.0);
}

void cLidar2PointCloud::setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

	auto c1 = mSensorToENU.column(0);
	c1[0] = cos(yaw) * cos(pitch);
	c1[1] = sin(yaw) * cos(pitch);
	c1[2] = -sin(pitch);

	auto c2 = mSensorToENU.column(1);
	c2[0] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
	c2[1] = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
	c2[2] = cos(pitch) * sin(roll);

	auto c3 = mSensorToENU.column(2);
	c3[0] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
	c3[1] = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
	c3[2] = cos(pitch) * cos(roll);
}
