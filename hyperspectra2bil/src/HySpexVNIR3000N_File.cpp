
#include "HySpexVNIR3000N_File.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexVNIR3000N_File::cHySpexVNIR3000N_File() : cHySpexVNIR_3000N_Parser()
{
}

cHySpexVNIR3000N_File::~cHySpexVNIR3000N_File()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createHeaderFilename('\0');
        std::filesystem::rename(mHeaderFilename, filename);
    }
}

void cHySpexVNIR3000N_File::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

std::filesystem::path cHySpexVNIR3000N_File::createHeaderFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".vnir.hdr";
    }
    else
        ext = ".vnir.hdr";

    filename += ext;

    return filename;
}

void cHySpexVNIR3000N_File::openDataFile()
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

void cHySpexVNIR3000N_File::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cHySpexVNIR3000N_File::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    openDataFile();

    mActiveRow = 0;
}

void cHySpexVNIR3000N_File::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mOutputFile.close();

    mHeaderFilename = createHeaderFilename(mPlotID);

    writeHeader(mHeaderFilename);

    ++mPlotID;

    mActiveRow = 0;
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
    mSpectralCalibration = wavelengths_nm;
}

void cHySpexVNIR3000N_File::onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms) {}
void cHySpexVNIR3000N_File::onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds) {}
void cHySpexVNIR3000N_File::onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background) {}

void cHySpexVNIR3000N_File::onSensorTemperature_C(uint8_t device_id, float temp_C) {}


