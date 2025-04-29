
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

	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

	void onSensorTemperature_K(uint8_t device_id, float temp_K) override;


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
