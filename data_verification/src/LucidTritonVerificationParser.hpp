/**
 * @file
 * @brief Parser for reading Lucid Vision Labs Triton image data from a block based data file
 */
#pragma once

#include <cbdf/LucidTritonParser.hpp>

class cLucidTritonVerificationParser : public cLucidTritonParser
{
public:
	bool sensorPresent = false;

	int mNumImages = 0;

    void onModelName(uint8_t deviceID, std::string modelName) override;
    void onVendor(uint8_t deviceID, std::string vendor) override;
    void onFamilyName(uint8_t deviceID, std::string family) override;
    void onDeviceVersion(uint8_t deviceID, std::string version) override;
    void onSerialNumber(uint8_t device_id, std::string serialNumber) override;
    void onFirmwareVersion(uint8_t deviceID, std::string version) override;
    void onMacAddress(uint8_t deviceID, std::string mac_address) override;
    void onIpAddress(uint8_t deviceID, std::string ip_address) override;

    void onFrameRate_Hz(uint8_t device_id, double frameRate_Hz) override;

    void onExposureInfo(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eExposureTimeSelector selector,
        nLucidVisionLabsConnect::nTriton::eExposureAuto mode, double time_us) override;

    void onExposureTimeSelector(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eExposureTimeSelector selector) override;
    void onExposureTime_us(uint8_t deviceID, double time_us) override;
    void onExposureAutoMode(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eExposureAuto mode) override;

    void onGainInfo(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eGainAuto mode, double gain_dB) override;
    void onGainAutoMode(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eGainAuto mode) override;
    void onGain_dB(uint8_t deviceID, double gain_dB) override;

    void onBalanceWhite(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::eBalanceWhiteAuto mode) override;

    void onGammaInfo(uint8_t deviceID, bool enabled, double gamma) override;
    void onGammaEnabled(uint8_t deviceID, bool enabled) override;
    void onGamma(uint8_t deviceID, double gamma) override;

    void onPixelFormat(uint8_t deviceID, nLucidVisionLabsConnect::nTriton::ePixelFormat format) override;

    void onImage(uint8_t device_id, nLucidVisionLabsConnect::cRgbImage image) override;

protected:
};


