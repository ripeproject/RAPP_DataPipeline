
#pragma once

#include <cbdf/HySpexSWIR_384_Parser.hpp>
#include <cbdf/SpidercamParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexSWIR384_File : public cHySpexSWIR_384_Parser, public cSpidercamParser
{
public:
    cHySpexSWIR384_File();
	~cHySpexSWIR384_File();

    void setOutputPath(std::filesystem::path out);

private:
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

	void onSensorTemperature_K(uint8_t device_id, float temp_K) override;

    void onPosition(spidercam::sPosition_1_t pos) override;

protected:
	virtual void openDataFile() = 0;
	virtual void writeHeader() = 0;

protected:
    std::filesystem::path mOutputPath;
	std::ofstream mOutputFile;

	std::size_t mSpatialSize;
	std::size_t mSpectralSize;

	std::size_t mMaxSpatialSize = 0;
	std::size_t mMaxSpectralSize = 0;

	uint16_t mMaxPixelValue = 0;

	enum eNumBits {ONE = 1, FOUR = 4, EIGHT = 8, SIXTEEN = 16};
	eNumBits  mNumBits = ONE;

	double mFieldOfView_rad = 0.0;
	uint32_t mFramePeriod_us = 0;
	uint32_t mIntegrationTime_us = 0;

	std::size_t mNumFrames = 0;

	HySpexConnect::cSpectralData<float> mSpectralCalibration;

    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
