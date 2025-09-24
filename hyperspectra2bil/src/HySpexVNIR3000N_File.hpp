
#pragma once

#include <cbdf/HySpexVNIR_3000N_Parser.hpp>

#include <cbdf/SpidercamParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexVNIR3000N_File : public cHySpexVNIR_3000N_Parser
{
public:
	cHySpexVNIR3000N_File();
	virtual ~cHySpexVNIR3000N_File();

    void setOutputPath(std::filesystem::path out);

	// Spidercam Parser Data
	void onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps);

	// Experiment Info Parser Data
	void onStartRecordingTimestamp(uint64_t timestamp_ns);
	void onEndRecordingTimestamp(uint64_t timestamp_ns);

private:
	void onBeginReference(uint8_t device_id) override;
	void onEndOfReference(uint8_t device_id) override;

	void onID(uint8_t device_id, std::string id) override;
	void onSerialNumber(uint8_t device_id, std::string serialNumber) override;
	void onWavelengthRange_nm(uint8_t device_id, uint16_t minWavelength_nm, uint16_t maxWavelength_nm) override;
	void onSpatialSize(uint8_t device_id, uint64_t spatialSize) override;
	void onSpectralSize(uint8_t device_id, uint64_t spectralSize) override;
	void onMaxSpatialSize(uint8_t device_id, uint64_t maxSpatialSize) override;
	void onMaxSpectralSize(uint8_t device_id, uint64_t maxSpectralSize) override;
	void onMaxPixelValue(uint8_t device_id, uint16_t maxPixelValue) override;

	void onResponsivityMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> re) override;
	void onQuantumEfficiencyData(uint8_t device_id, HySpexConnect::cSpectralData<float> qe) override;
	void onLensName(uint8_t device_id, std::string name) override;
	void onLensWorkingDistance_cm(uint8_t device_id, double workingDistance_cm) override;
	void onLensFieldOfView_rad(uint8_t device_id, double fieldOfView_rad) override;
	void onLensFieldOfView_deg(uint8_t device_id, double fieldOfView_deg) override;

	void onAverageFrames(uint8_t device_id, uint16_t averageFrames) override;
	void onFramePeriod_us(uint8_t device_id, uint32_t framePeriod_us) override;
	void onIntegrationTime_us(uint8_t device_id, uint32_t integrationTime_us) override;

	void onBadPixels(uint8_t device_id, HySpexConnect::cBadPixelData bad_pixels) override;
	void onBadPixelCorrection(uint8_t device_id, HySpexConnect::cBadPixelCorrectionData corrections) override;
	void onBadPixelMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<uint8_t> matrix) override;

	void onAmbientTemperature_C(uint8_t device_id, float temp_C) override;

	void onSpectralCalibration(uint8_t device_id, HySpexConnect::cSpectralData<float> wavelengths_nm) override;

	void onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms) override;
	void onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds) override;
	void onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background) override;

	void onSensorTemperature_C(uint8_t device_id, float temp_C) override;

protected:
	virtual std::filesystem::path createHeaderFilename(char plotID);

protected:
	virtual std::filesystem::path createDataFilename(char plotID) = 0;
	virtual void writeHeader(std::filesystem::path filename) = 0;

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

	HySpexConnect::cSpectralData<float> mSpectralCalibration;

	char mPlotID = 'A';

	std::size_t mActiveRow = 0;


    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
