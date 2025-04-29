
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

void cHySpexVNIR3000N_File::onID(uint8_t device_id, std::string id) {}
void cHySpexVNIR3000N_File::onSerialNumber(uint8_t device_id, std::string serialNumber) {}
void cHySpexVNIR3000N_File::onWavelengthRange_nm(uint8_t device_id, uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexVNIR3000N_File::onSpatialSize(uint8_t device_id, uint64_t spatialSize)
{
    mSpatialSize = spatialSize;
}

void cHySpexVNIR3000N_File::onSpectralSize(uint8_t device_id, uint64_t spectralSize)
{
    mSpectralSize = spectralSize;
}

void cHySpexVNIR3000N_File::onMaxSpatialSize(uint8_t device_id, uint64_t maxSpatialSize) { mMaxSpatialSize = maxSpatialSize; }
void cHySpexVNIR3000N_File::onMaxSpectralSize(uint8_t device_id, uint64_t maxSpectralSize) { mMaxSpectralSize = maxSpectralSize; }
void cHySpexVNIR3000N_File::onMaxPixelValue(uint8_t device_id, uint16_t maxPixelValue)
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

void cHySpexVNIR3000N_File::onResponsivityMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexVNIR3000N_File::onQuantumEfficiencyData(uint8_t device_id, HySpexConnect::cSpectralData<float> qe) {}
void cHySpexVNIR3000N_File::onLensName(uint8_t device_id, std::string name) {}
void cHySpexVNIR3000N_File::onLensWorkingDistance_cm(uint8_t device_id, double workingDistance_cm) {}
void cHySpexVNIR3000N_File::onLensFieldOfView_rad(uint8_t device_id, double fieldOfView_rad) {}
void cHySpexVNIR3000N_File::onLensFieldOfView_deg(uint8_t device_id, double fieldOfView_deg) {}

void cHySpexVNIR3000N_File::onAverageFrames(uint8_t device_id, uint16_t averageFrames) {}
void cHySpexVNIR3000N_File::onFramePeriod_us(uint8_t device_id, uint32_t framePeriod_us) {}
void cHySpexVNIR3000N_File::onIntegrationTime_us(uint8_t device_id, uint32_t integrationTime_us) {}

void cHySpexVNIR3000N_File::onBadPixels(uint8_t device_id, HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexVNIR3000N_File::onBadPixelCorrection(uint8_t device_id, HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexVNIR3000N_File::onBadPixelMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexVNIR3000N_File::onAmbientTemperature_C(uint8_t device_id, float temp_C) {}

void cHySpexVNIR3000N_File::onSpectralCalibration(uint8_t device_id, HySpexConnect::cSpectralData<float> wavelengths_nm)
{
}

void cHySpexVNIR3000N_File::onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms) {}
void cHySpexVNIR3000N_File::onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds) {}
void cHySpexVNIR3000N_File::onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background) {}

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

void cHySpexVNIR3000N_File::onSensorTemperature_C(uint8_t device_id, float temp_C) {}

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


