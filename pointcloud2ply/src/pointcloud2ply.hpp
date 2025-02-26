
#pragma once

#include <cbdf/PointCloud.hpp>
#include <cbdf/PointCloudParser.hpp>
#include <cbdf/PlotInfoParser.hpp>
#include <cbdf/SpidercamParser.hpp>

#include <filesystem>
#include <string>
#include <fstream>

#define USE_FLOATS

class cPointCloud2Ply : public cPointCloudParser, public cPlotInfoParser, public cSpidercamParser
{
public:
    static bool mIndividualPlyFiles;
    static bool mSaveDollyPositions;
    static bool mUseBinaryFormat;
    static bool mResetOrigin;

public:
	cPointCloud2Ply();
	~cPointCloud2Ply();

    void setOutputPath(std::filesystem::path out);

    void close();

private:
    void onBeginPointCloudBlock() override;
    void onEndPointCloudBlock() override;

    void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) override;
    void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) override;
    void onDistanceWindow(double min_dist_m, double max_dist_m) override;
    void onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg) override;
    void onAltitudeWindow(double min_altitude_deg, double max_altitude_deg) override;

    void onReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm) override;

    void onVegetationOnly(const bool vegetation_only) override;

    void onDimensions(double x_min_m, double x_max_m,
        double y_min_m, double y_max_m, double z_min_m, double z_max_m) override;

    void onBeginSensorKinematics() override;
    void onEndSensorKinematics() override;
    void onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point) override;

    void onPointCloudData(cPointCloud pointCloud) override;
    void onPointCloudData(cPointCloud_FrameId pointCloud) override;
    void onPointCloudData(cPointCloud_SensorInfo pointCloud) override;

    void onBeginPointCloudList() override;
    void onEndPointCloudList() override;

    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud) override;

    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud) override;
    void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud) override;

private:
    void onBeginPlotInfoList() override;
    void onEndPlotInfoList() override;

    void onBeginPlotInfoBlock() override;
    void onEndPlotInfoBlock() override;

    void onPlotID(int id) override;
    void onSubPlotID(int id) override;
    void onName(const std::string& name) override;
    void onDescription(const std::string& description) override;

    void onSpecies(const std::string& species) override;
    void onCultivar(const std::string& cultivar) override;


    void onBeginTreatmentList() override;
    void onEndTreatmentList() override;
    void onTreatment(const std::string& treatment) override;

    void onConstructName(const std::string& name) override;
    void onEvent(const std::string& event) override;

    void onPotLabel(const std::string& pot_label) override;
    void onSeedGeneration(const std::string& seed_generation) override;
    void onCopyNumber(const std::string& copy_number) override;
    void onLeafType(const std::string& leaf_type) override;

    void onPlotDimensions(double x_min_m, double x_max_m,
        double y_min_m, double y_max_m, double z_min_m, double z_max_m) override;

    void onPlotPointCloudData(cPlotPointCloud pointCloud) override;


private:
    void onPosition(spidercam::sPosition_1_t pos) override;

    void writePosition(std::filesystem::path filename);
    void writePointcloud(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;

    uint32_t    mFrameCount = 0;

    int32_t    mPlotId = 0;
    int32_t    mSubPlotId = 0;

#ifdef USE_FLOATS
    typedef float range_t;
    struct returns3 { float s, r, a; };

    typedef float frameID_t;
    struct pixel_loc { float chn, pixel; };
#else
    typedef uint32_t range_t;
    struct returns3 { uint16_t s, r, a; };

    typedef uint16_t frameID_t;
    struct pixel_loc { uint16_t chn, pixel; };
#endif


    struct position3 { float x, y, z; };
    struct position4 { float x, y, z, s; };
    struct color3 { uint8_t r, g, b; };

    color3 mColor;

    std::vector<position4>  mPositions;
    std::vector<color3>     mColors;

    std::vector<position3>  mVertices;

    std::vector<range_t>    mRanges;

    std::vector<returns3>   mReturns;

    std::vector<frameID_t>  mFrameIDs;

    std::vector<pixel_loc>  mPixelLocations;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
