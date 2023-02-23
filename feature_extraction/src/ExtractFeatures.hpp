
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloud.hpp"
#include "LineDetection3D.h"

#include <filesystem>
#include <string>
#include <fstream>



class cExtractFeatures : public cPointCloudParser
{
public:
    static bool mIndividualPlyFiles;

public:
    cExtractFeatures();
	~cExtractFeatures();

    void setOutputPath(std::filesystem::path out);

private:
    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onImuData(pointcloud::imu_data_t data) override;
    void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(cPointCloud pointCloud) override;

    void writePlyFile(std::filesystem::path filename);
    void writePlaneFile(std::filesystem::path filename, const std::vector<PLANE>& planes, double scale);
    void writeLineFile(std::filesystem::path filename, const std::vector<std::vector<cv::Point3d>>& lines, double scale);

private:
    std::filesystem::path mOutputPath;

    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct uint3  { uint16_t s, r, a; };
    struct color3 { uint8_t r, g, b; };

    std::vector<float3>   mVertices;
    std::vector<uint32_t> mRanges;
    std::vector<uint3>    mReturns;
    std::vector<uint16_t> mFrameIDs;

	uint64_t mStartTimestamp_ns = 0;
};
