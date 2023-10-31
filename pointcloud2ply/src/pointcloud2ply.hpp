
#pragma once

#include "PointCloudParser.hpp"
#include "PointCloud.hpp"

#include <cbdf/SpidercamParser.hpp>

#include <filesystem>
#include <string>
#include <fstream>

#define USE_FLOATS

class cPointCloud2Ply : public cPointCloudParser, public cSpidercamParser
{
public:
    static bool mIndividualPlyFiles;
    static bool mSaveDollyPositions;

public:
	cPointCloud2Ply();
	~cPointCloud2Ply();

    void setOutputPath(std::filesystem::path out);

private:
    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) override;
    void onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) override;
    void onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) override;

    void onDimensions(double x_min_m, double x_max_m,
        double y_min_m, double y_max_m, double z_min_m, double z_max_m) override;

    void onImuData(pointcloud::imu_data_t data) override;

    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud) override;

    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud) override;

    void onPointCloudData(cPointCloud pointCloud) override;
    void onPointCloudData(cPointCloud_FrameId pointCloud) override;
    void onPointCloudData(cPointCloud_SensorInfo pointCloud) override;

    void onPosition(spidercam::sPosition_1_t pos) override;

    void writePosition(std::filesystem::path filename);
    void writePointcloud(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;

    uint32_t    mFrameCount = 0;

#ifdef USE_FLOATS
    typedef float range_t;
    struct returns3 { float s, r, a; };
    struct beam_loc { float theta, phi; };
#else
    typedef uint32_t range_t;
    struct returns3 { uint16_t s, r, a; };
    struct beam_loc { double theta, phi; };
#endif


    struct position3 { float x, y, z; };
    struct position4 { float x, y, z, s; };
    struct color3 { uint8_t r, g, b; };
    struct pixel_loc { uint16_t chn, pixel; };

    color3 mColor;

    std::vector<position4>  mPositions;
    std::vector<color3>     mColors;

    std::vector<position3>  mVertices;

    std::vector<range_t>    mRanges;

    std::vector<returns3>   mReturns;
    std::vector<uint16_t>   mFrameIDs;

    std::vector<pixel_loc>  mPixelLocations;
    std::vector<beam_loc>   mBeamLocations;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
