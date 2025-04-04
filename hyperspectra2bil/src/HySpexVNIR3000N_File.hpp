
#pragma once

#include <cbdf/HySpexVNIR_3000N_Parser.hpp>

#include <cbdf/SpidercamParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexVNIR3000N_File : public cHySpexVNIR_3000N_Parser, public cSpidercamParser
{
public:
	cHySpexVNIR3000N_File();
	~cHySpexVNIR3000N_File();

    void setOutputPath(std::filesystem::path out);

private:
	void onID(std::string id) override;
	void onSerialNumber(std::string serialNumber) override;
	void onWavelengthRange_nm(uint16_t minWavelength_nm, uint16_t maxWavelength_nm) override;
	void onSpatialSize(uint64_t spatialSize) override;
	void onSpectralSize(uint64_t spectralSize) override;
	void onMaxSpatialSize(uint64_t maxSpatialSize) override;
	void onMaxSpectralSize(uint64_t maxSpectralSize) override;
	void onMaxPixelValue(uint16_t maxPixelValue) override;

	void onResponsivityMatrix(HySpexConnect::cSpatialMajorData<float> re) override;
	void onQuantumEfficiencyData(HySpexConnect::cSpectralData<float> qe) override;
	void onLensName(std::string name) override;
	void onLensWorkingDistance_cm(double workingDistance_cm) override;
	void onLensFieldOfView_rad(double fieldOfView_rad) override;
	void onLensFieldOfView_deg(double fieldOfView_deg) override;

	void onAverageFrames(uint16_t averageFrames) override;
	void onFramePeriod_us(uint32_t framePeriod_us) override;
	void onIntegrationTime_us(uint32_t integrationTime_us) override;

	void onBadPixels(HySpexConnect::cBadPixelData bad_pixels) override;
	void onBadPixelCorrection(HySpexConnect::cBadPixelCorrectionData corrections) override;
	void onBadPixelMatrix(HySpexConnect::cSpatialMajorData<uint8_t> matrix) override;

	void onAmbientTemperature_C(float temp_C) override;

	void onSpectralCalibration(HySpexConnect::cSpectralData<float> wavelengths_nm) override;

	void onBackgroundMatrixAge_ms(int64_t age_ms) override;
	void onNumOfBackgrounds(uint32_t numOfBackgrounds) override;
	void onBackgroundMatrix(HySpexConnect::cSpatialMajorData<float> background) override;

	void onSensorTemperature_C(float temp_C) override;

    void onPosition(spidercam::sPosition_1_t pos) override;

protected:
	virtual void openDataFile() = 0;
	virtual void writeHeader() = 0;

protected:
    std::filesystem::path mOutputPath;
	std::ofstream mOutputFile;

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

	std::size_t mNumFrames = 0;

	std::size_t mActiveRow = 0;

    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
