

#include "HySpexSWIR_384_VerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>


void cHySpexSWIR_384_VerificationParser::onBeginReference(uint8_t device_id)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onEndOfReference(uint8_t device_id)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onID(uint8_t device_id, std::string id)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onSerialNumber(uint8_t device_id, std::string serialNumber)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onWavelengthRange_nm(uint8_t device_id, uint16_t minWavelength_nm, uint16_t maxWavelength_nm)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onSpatialSize(uint8_t device_id, uint64_t spatialSize)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onSpectralSize(uint8_t device_id, uint64_t spectralSize)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onMaxSpatialSize(uint8_t device_id, uint64_t maxSpatialSize)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onMaxSpectralSize(uint8_t device_id, uint64_t maxSpectralSize)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onMaxPixelValue(uint8_t device_id, uint16_t maxPixelValue)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onResponsivityMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> re)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onQuantumEfficiencyData(uint8_t device_id, HySpexConnect::cSpectralData<float> qe)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onLensName(uint8_t device_id, std::string name)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onLensWorkingDistance_cm(uint8_t device_id, double workingDistance_cm)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onLensFieldOfView_rad(uint8_t device_id, double fieldOfView_rad)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onLensFieldOfView_deg(uint8_t device_id, double fieldOfView_deg)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onAverageFrames(uint8_t device_id, uint16_t averageFrames)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onFramePeriod_us(uint8_t device_id, uint32_t framePeriod_us)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onIntegrationTime_us(uint8_t device_id, uint32_t integrationTime_us)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onBadPixels(uint8_t device_id, HySpexConnect::cBadPixelData bad_pixels)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onBadPixelCorrection(uint8_t device_id, HySpexConnect::cBadPixelCorrectionData corrections)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onBadPixelMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<uint8_t> matrix)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onAmbientTemperature_C(uint8_t device_id, float temp_C)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onSpectralCalibration(uint8_t device_id, HySpexConnect::cSpectralData<float> wavelengths_nm)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background)
{
	sensorPresent = true;
}

void cHySpexSWIR_384_VerificationParser::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
	sensorPresent = true;
	++mNumImages;
}

void cHySpexSWIR_384_VerificationParser::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
	sensorPresent = true;
	++mNumImages;
}

void cHySpexSWIR_384_VerificationParser::onSensorTemperature_K(uint8_t device_id, float temp_C)
{
	sensorPresent = true;
}

