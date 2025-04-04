
#include "HySpexVNIR3000N_File.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexVNIR3000N_File::cHySpexVNIR3000N_File() : cHySpexVNIR_3000N_Parser(), cSpidercamParser()
{
}

cHySpexVNIR3000N_File::~cHySpexVNIR3000N_File()
{
}

void cHySpexVNIR3000N_File::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
    openDataFile();
}

void cHySpexVNIR3000N_File::onID(std::string id) {}
void cHySpexVNIR3000N_File::onSerialNumber(std::string serialNumber) {}
void cHySpexVNIR3000N_File::onWavelengthRange_nm(uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexVNIR3000N_File::onSpatialSize(uint64_t spatialSize)
{
    mSpatialSize = spatialSize;
}

void cHySpexVNIR3000N_File::onSpectralSize(uint64_t spectralSize)
{
    mSpectralSize = spectralSize;
}

void cHySpexVNIR3000N_File::onMaxSpatialSize(uint64_t maxSpatialSize) { mMaxSpatialSize = maxSpatialSize; }
void cHySpexVNIR3000N_File::onMaxSpectralSize(uint64_t maxSpectralSize) { mMaxSpectralSize = maxSpectralSize; }
void cHySpexVNIR3000N_File::onMaxPixelValue(uint16_t maxPixelValue)
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

void cHySpexVNIR3000N_File::onResponsivityMatrix(HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexVNIR3000N_File::onQuantumEfficiencyData(HySpexConnect::cSpectralData<float> qe) {}
void cHySpexVNIR3000N_File::onLensName(std::string name) {}
void cHySpexVNIR3000N_File::onLensWorkingDistance_cm(double workingDistance_cm) {}
void cHySpexVNIR3000N_File::onLensFieldOfView_rad(double fieldOfView_rad) {}
void cHySpexVNIR3000N_File::onLensFieldOfView_deg(double fieldOfView_deg) {}

void cHySpexVNIR3000N_File::onAverageFrames(uint16_t averageFrames) {}
void cHySpexVNIR3000N_File::onFramePeriod_us(uint32_t framePeriod_us) {}
void cHySpexVNIR3000N_File::onIntegrationTime_us(uint32_t integrationTime_us) {}

void cHySpexVNIR3000N_File::onBadPixels(HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexVNIR3000N_File::onBadPixelCorrection(HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexVNIR3000N_File::onBadPixelMatrix(HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexVNIR3000N_File::onAmbientTemperature_C(float temp_C) {}

void cHySpexVNIR3000N_File::onSpectralCalibration(HySpexConnect::cSpectralData<float> wavelengths_nm)
{
}

void cHySpexVNIR3000N_File::onBackgroundMatrixAge_ms(int64_t age_ms) {}
void cHySpexVNIR3000N_File::onNumOfBackgrounds(uint32_t numOfBackgrounds) {}
void cHySpexVNIR3000N_File::onBackgroundMatrix(HySpexConnect::cSpatialMajorData<float> background) {}

/*
void cHySpexVNIR3000N_File::onImage(HySpexConnect::cImageData<uint16_t> image)
{
    if (mActiveRow >= mMaxRows)
    {
        mImage.resize(mActiveRow + mSpatialSize);
        mMaxRows = mActiveRow + mSpatialSize;
    }

    ++mActiveRow;
}

void cHySpexVNIR3000N_File::onImage(HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    for (int i = 0; image.spatialSize(); ++i)
    {
//        mImage[i] = { 128,128,128 };
    }
}
*/

void cHySpexVNIR3000N_File::onSensorTemperature_C(float temp_C) {}

void cHySpexVNIR3000N_File::onPosition(spidercam::sPosition_1_t pos)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = pos.X_mm / 1000.0;
    xyz.y = pos.Y_mm / 1000.0;
    xyz.z = pos.Z_mm / 1000.0;
    xyz.s = pos.speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}


