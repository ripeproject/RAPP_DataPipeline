
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloud.hpp"

#include <cbdf/SpidercamParser.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cPointCloud2Ply : public cPointCloudParser, public cSpidercamParser
{
public:
    static bool mIndividualPlyFiles;

public:
	cPointCloud2Ply();
	~cPointCloud2Ply();

    void setOutputPath(std::filesystem::path out);

private:
    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) override;
    void onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) override;
    void onImuData(pointcloud::imu_data_t data) override;
    void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(cPointCloud pointCloud) override;

    void onPosition(spidercam::sPosition_1_t pos) override;

    void writePosition(std::filesystem::path filename);
    void writePointcloud(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;

    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };
    struct color3 { uint8_t r, g, b; };

    color3 mColor;

    std::vector<float4>   mPositions;
    std::vector<color3>   mColors;

    std::vector<float3>   mVertices;
    std::vector<uint32_t> mRanges;
    std::vector<uint3>    mReturns;
    std::vector<uint16_t> mFrameIDs;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
