

#include "HySpexVNIR_3000N_VerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

void cHySpexVNIR_3000N_VerificationParser::onBeginReference(uint8_t device_id)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onEndOfReference(uint8_t device_id)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onID(uint8_t device_id, std::string id)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onSerialNumber(uint8_t device_id, std::string serialNumber)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onWavelengthRange_nm(uint8_t device_id, uint16_t minWavelength_nm, uint16_t maxWavelength_nm)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onSpatialSize(uint8_t device_id, uint64_t spatialSize)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onSpectralSize(uint8_t device_id, uint64_t spectralSize)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onMaxSpatialSize(uint8_t device_id, uint64_t maxSpatialSize)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onMaxSpectralSize(uint8_t device_id, uint64_t maxSpectralSize)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onMaxPixelValue(uint8_t device_id, uint16_t maxPixelValue)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onResponsivityMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> re)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onQuantumEfficiencyData(uint8_t device_id, HySpexConnect::cSpectralData<float> qe)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onLensName(uint8_t device_id, std::string name)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onLensWorkingDistance_cm(uint8_t device_id, double workingDistance_cm)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onLensFieldOfView_rad(uint8_t device_id, double fieldOfView_rad)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onLensFieldOfView_deg(uint8_t device_id, double fieldOfView_deg)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onAverageFrames(uint8_t device_id, uint16_t averageFrames)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onFramePeriod_us(uint8_t device_id, uint32_t framePeriod_us)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onIntegrationTime_us(uint8_t device_id, uint32_t integrationTime_us)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onBadPixels(uint8_t device_id, HySpexConnect::cBadPixelData bad_pixels)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onBadPixelCorrection(uint8_t device_id, HySpexConnect::cBadPixelCorrectionData corrections)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onBadPixelMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<uint8_t> matrix)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onAmbientTemperature_C(uint8_t device_id, float temp_C)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onSpectralCalibration(uint8_t device_id, HySpexConnect::cSpectralData<float> wavelengths_nm)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background)
{
	sensorPresent = true;
}

void cHySpexVNIR_3000N_VerificationParser::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
	sensorPresent = true;
	++mNumImages;
}

void cHySpexVNIR_3000N_VerificationParser::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
	sensorPresent = true;
	++mNumImages;
}

void cHySpexVNIR_3000N_VerificationParser::onSensorTemperature_C(uint8_t device_id, float temp_C)
{
	sensorPresent = true;
}



