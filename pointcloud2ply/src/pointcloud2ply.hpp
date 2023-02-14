
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloudTypes.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <numbers>
#include <optional>
#include <fstream>


struct sPoint_t
{
    double x;
    double y;
    double z;

    sPoint_t() : x(0), y(0), z(0) {}
    sPoint_t(double i, double j, double k) : x(i), y(j), z(k) {}
};


class cPointCloud2Ply : public cPointCloudParser
{
public:
	cPointCloud2Ply();
	~cPointCloud2Ply();

private:
    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onImuData(pointcloud::imu_data_t data) override;


private:

private:
//    ouster::matrix_col_major<pointcloud::sCloudPoint_t> mCloud;

    std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;

	uint64_t mStartTimestamp_ns = 0;
};
