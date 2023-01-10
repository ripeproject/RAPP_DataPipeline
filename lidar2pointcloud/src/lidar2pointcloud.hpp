
#pragma once

#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <numbers>


class cLidar2PointCloud : public cOusterParser
{
public:
	static void setValidRange_m(double min_dist_m, double max_dist_m);
	static void setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg);

public:
	cLidar2PointCloud();
	~cLidar2PointCloud();

private:
	static double mMinDistance_m;
	static double mMaxDistance_m;

	static ouster::cRotationMatrix<double> mSensorToENU;
};
