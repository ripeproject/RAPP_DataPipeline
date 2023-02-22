
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloud.hpp"

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
    void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(cPointCloud pointCloud) override;

private:

private:
    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct uint3 { uint16_t s, r, a; };

    uint16_t mFrameID = 0;
    std::vector<float3>   mVertices;
    std::vector<uint32_t> mRanges;
    std::vector<uint3>    mReturns;
    std::vector<uint16_t> mFrameIDs;


    std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;

	uint64_t mStartTimestamp_ns = 0;
};
