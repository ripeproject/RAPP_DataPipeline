
#pragma once

#include "PlotConfigFile.hpp"

#include <cbdf/ExperimentParser.hpp>
#include <cbdf/AxisCommunicationsParser.hpp>
#include <cbdf/SsnxParser.hpp>
#include <cbdf/SpidercamParser.hpp>

#include <filesystem>
#include <string>
#include <fstream>


struct sPosTime_t
{
    bool    positionValid = false;
    double	lat_deg = 0;
    double	lon_deg = 0;
    double	elevation_m = 0;

    bool    dateTimeValid = false;
    int     utcYear = 0;
    int     utcMonth = 0;
    int     utcDay = 0;
    int     utcHour = 0;
    int     utcMinute = 0;
    int     utcSecond = 0;
};


class cExportJpegs : public cExperimentParser, public cAxisCommunicationsParser, public cSsnxParser, public cSpidercamParser
{
public:
    cExportJpegs();
	~cExportJpegs();

    bool abort() const;

    void setOutputPath(std::filesystem::path out);
    void setRootFileName(const std::string& filename);
    void setTimeStamp(const std::string& time_stamp);

    void setPlotFile(std::shared_ptr<cPlotConfigFile>& plot_file);

// Experiment Info handlers
private:
    void onBeginHeader() override;
    void onEndOfHeader() override;

    void onBeginFooter() override;
    void onEndOfFooter() override;

    void onExperimentTitle(const std::string& title) override;
    void onMeasurementTitle(const std::string& title) override;
    void onPrincipalInvestigator(const std::string& investigator) override;

    void onBeginResearcherList() override;
    void onEndOfResearcherList() override;
    void onResearcher(const std::string& researcher) override;

    void onSpecies(const std::string& species) override;
    void onCultivar(const std::string& cultivar) override;
    void onPermitInfo(const std::string& permit) override;
    void onExperimentDoc(const std::string& doc) override;

    void onBeginTreatmentList() override;
    void onEndOfTreatmentList() override;
    void onTreatment(const std::string& treatment) override;

    void onConstructName(const std::string& name) override;

    void onBeginEventNumberList() override;
    void onEndOfEventNumberList() override;
    void onEventNumber(const std::string& event) override;

    void onFieldDesign(const std::string& design) override;
    void onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;
    void onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;

    void onBeginCommentList() override;
    void onEndOfCommentList() override;
    void onComment(const std::string& comment) override;

    void onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) override;
    void onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) override;

    void onDayOfYear(std::uint16_t day_of_year) override;

    void onBeginSensorList() override;
    void onEndOfSensorList() override;
    void onSensorBlockInfo(uint16_t class_id, const std::string& name) override;
    void onSensorBlockInfo(uint16_t class_id, const std::string& name, const std::string& instance,
        const std::string& manufacturer, const std::string& model, const std::string& serial_number, uint8_t device_id) override;

    void onStartTime(sExperimentTime_t time) override;
    void onEndTime(sExperimentTime_t time) override;

    void onStartRecordingTimestamp(uint64_t timestamp_ns) override;
    void onEndRecordingTimestamp(uint64_t timestamp_ns) override;
    void onHeartbeatTimestamp(uint64_t timestamp_ns) override;

// Image handlers
private:
    void onActiveCameraId(uint8_t device_id, int id) override;
    void onFramesPerSecond(uint8_t device_id, int frames_per_sec) override;
    void onImageSize(uint8_t device_id, int width, int height) override;
    void onBitmap(uint8_t device_id, const cBitmapBuffer& buffer) override;
    void onJPEG(uint8_t device_id, const cJpegBuffer& buffer) override;
    void onMpegFrame(uint8_t device_id, const cMpegFrameBuffer& buffer) override;

// Spidercam handlers
    void onPosition(spidercam::sPosition_1_t position) override;

// GPS handlers
private:
    void onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_1_t pos) override;
    void onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_2_t pos) override;
    void onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_1_t pos) override;
    void onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_2_t pos) override;
    void onPosCovGeodetic(uint8_t device_id, ssnx::gps::PosCovGeodetic_1_t cov) override;
    void onVelCovGeodetic(uint8_t device_id, ssnx::gps::VelCovGeodetic_1_t cov) override;
    void onDOP(uint8_t device_id, ssnx::gps::DOP_1_t dop) override;
    void onPVT_Residuals(uint8_t device_id, ssnx::gps::PVT_Residuals_1_t residuals) override;
    void onRAIMStatistics(uint8_t device_id, ssnx::gps::RAIMStatistics_1_t raim) override;
    void onPOS_Local(uint8_t device_id, ssnx::gps::POS_Local_1_t pos) override;
    void onPOS_Projected(uint8_t device_id, ssnx::gps::POS_Projected_1_t pos) override;
    void onReceiverTime(uint8_t device_id, ssnx::gps::ReceiverTime_1_t time) override;
    void onRtcmDatum(uint8_t device_id, ssnx::gps::RtcmDatum_1_t datum) override;

    void onReferencePoint(uint8_t device_id, double avgLat_rad, double avgLng_rad, double avgHeight_m,
        double stdLat_rad, double stdLng_rad, double stdHeight_m, bool heightComputed) override;

private:
    bool mAbort = false;

    std::filesystem::path mOutputPath;
    std::string mRootFileName;
    std::string mTimeStamp;

    bool mUsePlotPrefix = false;

    std::shared_ptr<cPlotConfigFile> mPlotConfigData;
    std::vector<cPlotConfigPlotInfo> mPlots;

    std::string mExperimentTitle;
    std::string mMeasurementTitle;

    uint32_t    mFrameCount = 0;
    sPosTime_t  mCenterPos;

    int mWidth = 0;
    int mHeight = 0;

    double	mX_mm = 0;
    double	mY_mm = 0;
    double	mZ_mm = 0;
    double	mDistance = 0;

    bool    mPositionValid = false;
    double	mLat_deg = 0;
    double	mLon_deg = 0;
    double	mHeight_m = 0;
    float   mUndulation_m = 0;
    double	mElevation_m = 0;

    bool    mDateTimeValid = false;
    int     mUtcYear = 0;
    int     mUtcMonth = 0;
    int     mUtcDay = 0;
    int     mUtcHour = 0;
    int     mUtcMinute = 0;
    int     mUtcSecond = 0;
    int     mDeltaLS = 0;
};
