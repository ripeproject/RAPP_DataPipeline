/**
 * @file
 * @brief Parser for reading Lucid Vision Labs Triton image data from a block based data file
 */
#pragma once

#include <cbdf/TeledyneFlirParser.hpp>

class cTeledyneAtlasFlirVerificationParser : public cTeledyneFlirParser
{
public:
	bool sensorPresent = false;

	int mNumImages = 0;

    void onModelName(uint8_t deviceID, std::string modelName) override;
    void onFilter(uint8_t deviceID, std::string filter) override;
    void onLens(uint8_t deviceID, std::string lens) override;
    void onSerialNumber(uint8_t device_id, std::string serialNumber) override;
    void onProgramVersion(uint8_t deviceID, std::string programVersion) override;
    void onArticleNumber(uint8_t deviceID, std::string articleNumber) override;
    void onCalibrationTitle(uint8_t deviceID, std::string calibrationTitle) override;
    void onLensSerialNumber(uint8_t deviceID, std::string lensSerialNumber) override;
    void onArcFileVersion(uint8_t deviceID, std::string arcFileVersion) override;
    void onArcDateTime(uint8_t deviceID, std::string arcDateTime) override;
    void onArcSignature(uint8_t deviceID, std::string arcSignature) override;
    void onCountryCode(uint8_t deviceID, std::string countryCode) override;

    void onFrameRate_Hz(uint8_t device_id, double frameRate_Hz) override;

    void onFrameRateRange_Hz(uint8_t device_id, double minFrameRate_Hz, double maxFrameRate_Hz) override;
    void onThermalRange_K(uint8_t device_id, float minThermalValue_K, float maxThermalValue_K) override;

    void onHorizonalFoV_deg(uint8_t device_id, float horizonalFoV_deg) override;
    void onFocalLength(uint8_t device_id, float focalLength) override;

    void onImage(uint8_t device_id, nTeledyneAtlasConnect::cThermalImage image) override;


protected:
};


