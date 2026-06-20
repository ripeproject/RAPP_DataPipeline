/**
 * @file
 * @brief Parser for reading HySpex SWIR 384 image data from a block based data file
 */
#pragma once

#include <cbdf/HySpexSWIR_384_Parser.hpp>

class cHySpexSWIR_384_VerificationParser : public cHySpexSWIR_384_Parser
{
public:
	bool sensorPresent = false;

	int mNumImages = 0;


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

	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image) override;

	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

	void onSensorTemperature_K(uint8_t device_id, float temp_C) override;

protected:
};


