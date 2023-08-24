
#include "HySpexSWIR384_2_RGB.hpp"


#include <iostream>


cHySpexSWIR384_2_Rgb::cHySpexSWIR384_2_Rgb() : cHySpexSWIR_384_Parser()
{}

cHySpexSWIR384_2_Rgb::~cHySpexSWIR384_2_Rgb()
{}

void cHySpexSWIR384_2_Rgb::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cHySpexSWIR384_2_Rgb::setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm)
{
    mRed_nm = red_nm;
    mGreen_nm = green_nm;
    mBlue_nm = blue_nm;
}

void cHySpexSWIR384_2_Rgb::onID(std::string id) {}
void cHySpexSWIR384_2_Rgb::onSerialNumber(std::string serialNumber) {}
void cHySpexSWIR384_2_Rgb::onWavelengthRange_nm(uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexSWIR384_2_Rgb::onSpatialSize(uint64_t spatialSize) {}
void cHySpexSWIR384_2_Rgb::onSpectralSize(uint64_t spectralSize) {}
void cHySpexSWIR384_2_Rgb::onMaxSpatialSize(uint64_t maxSpatialSize) {}
void cHySpexSWIR384_2_Rgb::onMaxSpectralSize(uint64_t maxSpectralSize) {}
void cHySpexSWIR384_2_Rgb::onMaxPixelValue(uint16_t maxPixelValue) {}

void cHySpexSWIR384_2_Rgb::onResponsivityMatrix(HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexSWIR384_2_Rgb::onQuantumEfficiencyData(HySpexConnect::cSpectralData<float> qe) {}
void cHySpexSWIR384_2_Rgb::onLensName(std::string name) {}
void cHySpexSWIR384_2_Rgb::onLensWorkingDistance_cm(double workingDistance_cm) {}
void cHySpexSWIR384_2_Rgb::onLensFieldOfView_rad(double fieldOfView_rad) {}
void cHySpexSWIR384_2_Rgb::onLensFieldOfView_deg(double fieldOfView_deg) {}

void cHySpexSWIR384_2_Rgb::onAverageFrames(uint16_t averageFrames) {}
void cHySpexSWIR384_2_Rgb::onFramePeriod_us(uint32_t framePeriod_us) {}
void cHySpexSWIR384_2_Rgb::onIntegrationTime_us(uint32_t integrationTime_us) {}

void cHySpexSWIR384_2_Rgb::onBadPixels(HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexSWIR384_2_Rgb::onBadPixelCorrection(HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexSWIR384_2_Rgb::onBadPixelMatrix(HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexSWIR384_2_Rgb::onAmbientTemperature_C(float temp_C) {}

void cHySpexSWIR384_2_Rgb::onSpectralCalibration(HySpexConnect::cSpectralData<float> wavelengths_nm) {}

void cHySpexSWIR384_2_Rgb::onBackgroundMatrixAge_ms(int64_t age_ms) {}
void cHySpexSWIR384_2_Rgb::onNumOfBackgrounds(uint32_t numOfBackgrounds) {}
void cHySpexSWIR384_2_Rgb::onBackgroundMatrix(HySpexConnect::cSpatialMajorData<float> background) {}

void cHySpexSWIR384_2_Rgb::onImage(HySpexConnect::cImageData<uint16_t> image) {}
void cHySpexSWIR384_2_Rgb::onImage(HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{}

void cHySpexSWIR384_2_Rgb::onSensorTemperature_K(float temp_K) {}

void cHySpexSWIR384_2_Rgb::onPosition(spidercam::sPosition_1_t pos)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = pos.X_mm / 1000.0;
    xyz.y = pos.Y_mm / 1000.0;
    xyz.z = pos.Z_mm / 1000.0;
    xyz.s = pos.speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cHySpexSWIR384_2_Rgb::writeRgbImage(std::filesystem::path filename)
{
}

