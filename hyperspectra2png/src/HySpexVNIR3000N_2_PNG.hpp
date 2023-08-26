
#pragma once

#include <cbdf/HySpexVNIR_3000N_Parser.hpp>

#include <cbdf/SpidercamParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexVNIR3000N_2_Png : public cHySpexVNIR_3000N_Parser, public cSpidercamParser
{
public:
    cHySpexVNIR3000N_2_Png();
	~cHySpexVNIR3000N_2_Png();

    void setOutputPath(std::filesystem::path out);

	void setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm);

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

	void onImage(HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

	void onSensorTemperature_C(float temp_C) override;

    void onPosition(spidercam::sPosition_1_t pos) override;

    void writeRgbImage(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;

	std::size_t mSpatialSize = 0;
	std::size_t mMaxRows = 0;

	float mColorScale = 1.0;

	float mRed_nm   = 641.07f;
	float mGreen_nm = 547.95f;
	float mBlue_nm  = 463.30f;

	std::size_t mRedIndex   = 279;
	std::size_t mGreenIndex = 169;
	std::size_t mBlueIndex  = 69;

	std::size_t mActiveRow = 0;

	cv::Mat mImage;

    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
