
#include "HySpexSWIR384_File.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexSWIR384_File::cHySpexSWIR384_File() : cHySpexSWIR_384_Parser()
{}

cHySpexSWIR384_File::~cHySpexSWIR384_File()
{
    mOutputFile.close();
}

void cHySpexSWIR384_File::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
    openDataFile();
}

void cHySpexSWIR384_File::onID(std::string id) {}
void cHySpexSWIR384_File::onSerialNumber(std::string serialNumber) {}
void cHySpexSWIR384_File::onWavelengthRange_nm(uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexSWIR384_File::onSpatialSize(uint64_t spatialSize)
{
    mSpatialSize = spatialSize;
}

void cHySpexSWIR384_File::onSpectralSize(uint64_t spectralSize)
{
    mSpectralSize = spectralSize;
}

void cHySpexSWIR384_File::onMaxSpatialSize(uint64_t maxSpatialSize) { mMaxSpatialSize = maxSpatialSize; }
void cHySpexSWIR384_File::onMaxSpectralSize(uint64_t maxSpectralSize) { mMaxSpectralSize = maxSpectralSize; }
void cHySpexSWIR384_File::onMaxPixelValue(uint16_t maxPixelValue)
{
    mMaxPixelValue = maxPixelValue;

    if (mMaxPixelValue < 2)
    {
        mNumBits = ONE;
    }
    else if (mMaxPixelValue < 16)
    {
        mNumBits = FOUR;
    }
    else if (mMaxPixelValue < 256)
    {
        mNumBits = EIGHT;
    }
    else
    {
        mNumBits = SIXTEEN;
    }
}

void cHySpexSWIR384_File::onResponsivityMatrix(HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexSWIR384_File::onQuantumEfficiencyData(HySpexConnect::cSpectralData<float> qe) {}
void cHySpexSWIR384_File::onLensName(std::string name) {}
void cHySpexSWIR384_File::onLensWorkingDistance_cm(double workingDistance_cm) {}
void cHySpexSWIR384_File::onLensFieldOfView_rad(double fieldOfView_rad) {}
void cHySpexSWIR384_File::onLensFieldOfView_deg(double fieldOfView_deg) {}

void cHySpexSWIR384_File::onAverageFrames(uint16_t averageFrames) {}
void cHySpexSWIR384_File::onFramePeriod_us(uint32_t framePeriod_us) { mFramePeriod_us = framePeriod_us; }
void cHySpexSWIR384_File::onIntegrationTime_us(uint32_t integrationTime_us) { mIntegrationTime_us = integrationTime_us; }

void cHySpexSWIR384_File::onBadPixels(HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexSWIR384_File::onBadPixelCorrection(HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexSWIR384_File::onBadPixelMatrix(HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexSWIR384_File::onAmbientTemperature_C(float temp_C) {}

void cHySpexSWIR384_File::onSpectralCalibration(HySpexConnect::cSpectralData<float> wavelengths_nm)
{
    mSpectralCalibration = wavelengths_nm;
}

void cHySpexSWIR384_File::onBackgroundMatrixAge_ms(int64_t age_ms) {}
void cHySpexSWIR384_File::onNumOfBackgrounds(uint32_t numOfBackgrounds) {}
void cHySpexSWIR384_File::onBackgroundMatrix(HySpexConnect::cSpatialMajorData<float> background) {}

void cHySpexSWIR384_File::onSensorTemperature_K(float temp_K) {}

void cHySpexSWIR384_File::onPosition(spidercam::sPosition_1_t pos)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = pos.X_mm / 1000.0;
    xyz.y = pos.Y_mm / 1000.0;
    xyz.z = pos.Z_mm / 1000.0;
    xyz.s = pos.speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}


