
#pragma once

#include <cbdf/HySpexSWIR_384_Parser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexSWIR384_2_Png : public cHySpexSWIR_384_Parser
{
public:
    cHySpexSWIR384_2_Png();
	~cHySpexSWIR384_2_Png();

    void setOutputPath(std::filesystem::path out);

	void setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm);

	// Spidercam Parser Data
	void onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps);

	// Experiment Info Parser Data
	void onStartRecordingTimestamp(uint64_t timestamp_ns);
	void onEndRecordingTimestamp(uint64_t timestamp_ns);

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

	void onSensorTemperature_K(float temp_K) override;


    void writeRgbImage(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;

	std::size_t mSpatialSize = 0;
	std::size_t mSpectralSize = 0;
	std::size_t mMaxRows = 0;

	float mColorScale = 1.0;

	float mRed_nm = 1220.19f;
	float mGreen_nm = 1655.20f;
	float mBlue_nm = 2144.57f;

	std::size_t mRedIndex = 49;
	std::size_t mGreenIndex = 129;
	std::size_t mBlueIndex = 219;

	char mPlotID = 'A';

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
