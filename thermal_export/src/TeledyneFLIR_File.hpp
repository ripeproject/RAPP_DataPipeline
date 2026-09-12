
#pragma once

#include <cbdf\TeledyneFlirParser.hpp>

#include <cbdf/SpidercamParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cTeledyneFlir_File : public cTeledyneFlirParser
{
public:
	cTeledyneFlir_File();
	virtual ~cTeledyneFlir_File();

    void setOutputPath(std::filesystem::path out);

	// Spidercam Parser Data
	void onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps);

	// Experiment Info Parser Data
	void onStartRecordingTimestamp(uint64_t timestamp_ns);
	void onEndRecordingTimestamp(uint64_t timestamp_ns);

private:
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

protected:
	virtual std::filesystem::path createHeaderFilename(char plotID);

protected:
	virtual std::filesystem::path createDataFilename(char plotID);
	virtual void writeHeader(std::filesystem::path filename);

private:
	void openDataFile();

protected:
    std::filesystem::path mOutputPath;
	std::ofstream mOutputFile;

	std::filesystem::path mDataFilename;
	std::filesystem::path mHeaderFilename;

	std::size_t mSpatialSize = 0;
	std::size_t mSpectralSize = 0;

	std::size_t mMaxSpatialSize = 0;
	std::size_t mMaxSpectralSize = 0;

	uint16_t mMaxPixelValue = 0;

	enum eNumBits { ONE = 1, FOUR = 4, EIGHT = 8, SIXTEEN = 16 };
	eNumBits  mNumBits = ONE;

	double mFieldOfView_rad = 0.0;
	uint32_t mFramePeriod_us = 0;
	uint32_t mIntegrationTime_us = 0;


	char mPlotID = 'A';

	std::size_t mActiveRow = 0;


    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
