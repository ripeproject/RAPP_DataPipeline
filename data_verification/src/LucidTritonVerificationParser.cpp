

#include "LucidTritonVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>


void cLucidTritonVerificationParser::onModelName(uint8_t deviceID, std::string modelName)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onVendor(uint8_t deviceID, std::string vendor)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onFamilyName(uint8_t deviceID, std::string family)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onDeviceVersion(uint8_t deviceID, std::string version)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onSerialNumber(uint8_t device_id, std::string serialNumber)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onFirmwareVersion(uint8_t deviceID, std::string version)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onMacAddress(uint8_t deviceID, std::string mac_address)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onIpAddress(uint8_t deviceID, std::string ip_address)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onFrameRate_Hz(uint8_t device_id, double frameRate_Hz)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onExposureInfo(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eExposureTimeSelector selector,
	nLucidVisionLabsConnect::nTriton::eExposureAuto mode, double time_us)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onExposureTimeSelector(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eExposureTimeSelector selector)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onExposureTime_us(uint8_t deviceID, double time_us)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onExposureAutoMode(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eExposureAuto mode)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onGainInfo(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eGainAuto mode, double gain_dB)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onGainAutoMode(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eGainAuto mode)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onGain_dB(uint8_t deviceID, double gain_dB)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onBalanceWhite(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eBalanceWhiteAuto mode)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onGammaInfo(uint8_t deviceID, bool enabled, double gamma)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onGammaEnabled(uint8_t deviceID, bool enabled)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onGamma(uint8_t deviceID, double gamma)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onPixelFormat(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::ePixelFormat format)
{
	sensorPresent = true;
}

void cLucidTritonVerificationParser::onImage(uint8_t device_id, nLucidVisionLabsConnect::cRgbImage image)
{
	++mNumImages;
}



