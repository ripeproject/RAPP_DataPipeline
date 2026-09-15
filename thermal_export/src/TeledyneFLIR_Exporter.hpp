
#pragma once

#include <cbdf\TeledyneFlirParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>
#include <memory>

// Forward Declarations
class cColorTable;


enum class eColorTable { IRONBOW, RAINBOW, RAINBOW_HC, WHITE_HOT, BLACK_HOT, ARCTIC };

class cTeledyneFLIR_Exporter : public cTeledyneFlirParser
{
public:
	cTeledyneFLIR_Exporter();
	~cTeledyneFLIR_Exporter();

    void setOutputPath(std::filesystem::path out);

	void setScaleColorToImage(bool enable = true);
	void setScaleColorToCamera(bool enable = true);

	void setColorTable(eColorTable color_table);

	// Spidercam Parser Data
	void onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps);

	// Experiment Info Parser Data
	void onStartRecordingTimestamp(uint64_t timestamp_ns);
	void onEndRecordingTimestamp(uint64_t timestamp_ns);

private:
	// Teledyne FLIR Parser Methods
	void onModelName(uint8_t deviceID, std::string modelName) override;
	void onFilter(uint8_t deviceID, std::string filter) override;
	void onLens(uint8_t deviceID, std::string lens) override;
	void onSerialNumber(uint8_t device_id, std::string serialNumber) override;
	void onProgramVersion(uint8_t deviceID, std::string programVersion) override;
	void onArticleNumber(uint8_t deviceID, std::string articleNumber) override;
	void onCalibrationTitle(uint8_t deviceID, std::string calibrationTitle) override;
	void onLensSerialNumber(uint8_t deviceID, std::string lensSerialNumber) override;
	void onArcFileVersion(uint8_t deviceID, std::string arcFileVersion) override;
	void onArcDateTime(uint8_t deviceID, std::string arcDateTime) override;
	void onArcSignature(uint8_t deviceID, std::string arcSignature) override;
	void onCountryCode(uint8_t deviceID, std::string countryCode) override;

	void onFrameRate_Hz(uint8_t device_id, double frameRate_Hz) override;

	void onFrameRateRange_Hz(uint8_t device_id, double minFrameRate_Hz, double maxFrameRate_Hz) override;
	void onThermalRange_K(uint8_t device_id, float minThermalValue_K, float maxThermalValue_K) override;

	void onHorizonalFoV_deg(uint8_t device_id, float horizonalFoV_deg) override;
	void onFocalLength(uint8_t device_id, float focalLength) override;

	void onImage(uint8_t device_id, nTeledyneAtlasConnect::cThermalImage image) override;


    void writeImage(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;
	std::fstream mMetaData;

	std::string mModelName;
	std::string mFilter;
	std::string mLens;
	std::string mSerialNumber;
	std::string mProgramVersion;
	std::string mArticleNumber;
	std::string mCalibrationTitle;
	std::string mLensSerialNumber;
	std::string mArcFileVersion;
	std::string mArcDateTime;
	std::string mArcSignature;
	std::string mCountryCode;

	double mFrameRate_Hz = 0;
	double mMinFrameRate_Hz = 0;
	double mMaxFrameRate_Hz = 0;
	
	float mMinThermalValue_K = 0;
	float mMaxThermalValue_K = 0;

	float mHorizonalFoV_deg = 0;
	float mFocalLength = 0;

	bool mScaleToImage = true;
	std::unique_ptr<cColorTable> mColorTable;

	char mPlotID = 'A';

	cv::Mat mImage;

	uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
