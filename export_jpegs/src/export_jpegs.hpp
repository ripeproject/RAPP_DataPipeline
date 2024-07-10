
#pragma once

#include <cbdf/AxisCommunicationsParser.hpp>
#include <cbdf/SsnxParser.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cExportJpegs : public cAxisCommunicationsParser, public cSsnxParser
{
public:
    cExportJpegs();
	~cExportJpegs();

    void setOutputPath(std::filesystem::path out);
    void setRootFileName(const std::string& filename);

// Image handlers
private:
    void onActiveCameraId(int id) override;
    void onFramesPerSecond(int frames_per_sec) override;
    void onImageSize(int width, int height) override;
    void onBitmap(const cBitmapBuffer& buffer) override;
    void onJPEG(const cJpegBuffer& buffer) override;
    void onMpegFrame(const cMpegFrameBuffer& buffer) override;

    void writeJpeg(std::filesystem::path filename);

// GPS handlers
private:
    void onPVT_Cartesian(ssnx::gps::PVT_Cartesian_1_t pos) override;
    void onPVT_Cartesian(ssnx::gps::PVT_Cartesian_2_t pos) override;
    void onPVT_Geodetic(ssnx::gps::PVT_Geodetic_1_t pos) override;
    void onPVT_Geodetic(ssnx::gps::PVT_Geodetic_2_t pos) override;
    void onPosCovGeodetic(ssnx::gps::PosCovGeodetic_1_t cov) override;
    void onVelCovGeodetic(ssnx::gps::VelCovGeodetic_1_t cov) override;
    void onDOP(ssnx::gps::DOP_1_t dop) override;
    void onPVT_Residuals(ssnx::gps::PVT_Residuals_1_t residuals) override;
    void onRAIMStatistics(ssnx::gps::RAIMStatistics_1_t raim) override;
    void onPOS_Local(ssnx::gps::POS_Local_1_t pos) override;
    void onPOS_Projected(ssnx::gps::POS_Projected_1_t pos) override;
    void onReceiverTime(ssnx::gps::ReceiverTime_1_t time) override;
    void onRtcmDatum(ssnx::gps::RtcmDatum_1_t datum) override;

private:
    std::filesystem::path mOutputPath;
    std::string mRootFileName;

    uint32_t    mFrameCount = 0;

    int mWidth = 0;
    int mHeight = 0;

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
