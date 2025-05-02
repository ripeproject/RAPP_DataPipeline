
#include "HySpexSWIR384_File.hpp"

#include <iostream>


cHySpexSWIR384_File::cHySpexSWIR384_File() : cHySpexSWIR_384_Parser()
{}

cHySpexSWIR384_File::~cHySpexSWIR384_File()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createHeaderFilename('\0');
        std::filesystem::rename(mHeaderFilename, filename);
    }
}

void cHySpexSWIR384_File::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

std::filesystem::path cHySpexSWIR384_File::createHeaderFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".swir.hdr";
    }
    else
        ext = ".swir.hdr";

    filename += ext;

    return filename;
}

void cHySpexSWIR384_File::openDataFile()
{
    mDataFilename = createDataFilename(mPlotID);

    mOutputFile.open(mDataFilename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += mDataFilename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexSWIR384_File::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cHySpexSWIR384_File::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    openDataFile();

    mActiveRow = 0;
}

void cHySpexSWIR384_File::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mOutputFile.close();

    mHeaderFilename = createHeaderFilename(mPlotID);

    writeHeader(mHeaderFilename);

    ++mPlotID;

    mActiveRow = 0;
}

void cHySpexSWIR384_File::onID(uint8_t device_id, std::string id) {}
void cHySpexSWIR384_File::onSerialNumber(uint8_t device_id, std::string serialNumber) {}
void cHySpexSWIR384_File::onWavelengthRange_nm(uint8_t device_id, uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexSWIR384_File::onSpatialSize(uint8_t device_id, uint64_t spatialSize)
{
    mSpatialSize = spatialSize;
}

void cHySpexSWIR384_File::onSpectralSize(uint8_t device_id, uint64_t spectralSize)
{
    mSpectralSize = spectralSize;
}

void cHySpexSWIR384_File::onMaxSpatialSize(uint8_t device_id, uint64_t maxSpatialSize) { mMaxSpatialSize = maxSpatialSize; }
void cHySpexSWIR384_File::onMaxSpectralSize(uint8_t device_id, uint64_t maxSpectralSize) { mMaxSpectralSize = maxSpectralSize; }
void cHySpexSWIR384_File::onMaxPixelValue(uint8_t device_id, uint16_t maxPixelValue)
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

void cHySpexSWIR384_File::onResponsivityMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexSWIR384_File::onQuantumEfficiencyData(uint8_t device_id, HySpexConnect::cSpectralData<float> qe) {}
void cHySpexSWIR384_File::onLensName(uint8_t device_id, std::string name) {}
void cHySpexSWIR384_File::onLensWorkingDistance_cm(uint8_t device_id, double workingDistance_cm) {}
void cHySpexSWIR384_File::onLensFieldOfView_rad(uint8_t device_id, double fieldOfView_rad) {}
void cHySpexSWIR384_File::onLensFieldOfView_deg(uint8_t device_id, double fieldOfView_deg) {}

void cHySpexSWIR384_File::onAverageFrames(uint8_t device_id, uint16_t averageFrames) {}
void cHySpexSWIR384_File::onFramePeriod_us(uint8_t device_id, uint32_t framePeriod_us) { mFramePeriod_us = framePeriod_us; }
void cHySpexSWIR384_File::onIntegrationTime_us(uint8_t device_id, uint32_t integrationTime_us) { mIntegrationTime_us = integrationTime_us; }

void cHySpexSWIR384_File::onBadPixels(uint8_t device_id, HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexSWIR384_File::onBadPixelCorrection(uint8_t device_id, HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexSWIR384_File::onBadPixelMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexSWIR384_File::onAmbientTemperature_C(uint8_t device_id, float temp_C) {}

void cHySpexSWIR384_File::onSpectralCalibration(uint8_t device_id, HySpexConnect::cSpectralData<float> wavelengths_nm)
{
    mSpectralCalibration = wavelengths_nm;
}

void cHySpexSWIR384_File::onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms) {}
void cHySpexSWIR384_File::onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds) {}
void cHySpexSWIR384_File::onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background) {}

void cHySpexSWIR384_File::onSensorTemperature_K(uint8_t device_id, float temp_K) {}


