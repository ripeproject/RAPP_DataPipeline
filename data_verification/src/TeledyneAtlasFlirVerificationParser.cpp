

#include "TeledyneAtlasFlirVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>


void cTeledyneAtlasFlirVerificationParser::onModelName(uint8_t deviceID, std::string modelName) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onFilter(uint8_t deviceID, std::string filter) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onLens(uint8_t deviceID, std::string lens) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onSerialNumber(uint8_t device_id, std::string serialNumber) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onProgramVersion(uint8_t deviceID, std::string programVersion) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onArticleNumber(uint8_t deviceID, std::string articleNumber) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onCalibrationTitle(uint8_t deviceID, std::string calibrationTitle) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onLensSerialNumber(uint8_t deviceID, std::string lensSerialNumber) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onArcFileVersion(uint8_t deviceID, std::string arcFileVersion) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onArcDateTime(uint8_t deviceID, std::string arcDateTime) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onArcSignature(uint8_t deviceID, std::string arcSignature) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onCountryCode(uint8_t deviceID, std::string countryCode) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onFrameRate_Hz(uint8_t device_id, double frameRate_Hz) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onFrameRateRange_Hz(uint8_t device_id, double minFrameRate_Hz, double maxFrameRate_Hz) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onThermalRange_K(uint8_t device_id, float minThermalValue_K, float maxThermalValue_K) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onHorizonalFoV_deg(uint8_t device_id, float horizonalFoV_deg) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onFocalLength(uint8_t device_id, float focalLength) 
{
	sensorPresent = true;
}

void cTeledyneAtlasFlirVerificationParser::onImage(uint8_t device_id, nTeledyneAtlasConnect::cThermalImage image) 
{
	++mNumImages;
}



