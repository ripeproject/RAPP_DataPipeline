
#include "TeledyneFLIR_File.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cTeledyneFlir_File::cTeledyneFlir_File() : cTeledyneFlirParser()
{
}

cTeledyneFlir_File::~cTeledyneFlir_File()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createHeaderFilename('\0');
        std::filesystem::rename(mHeaderFilename, filename);
    }
}

void cTeledyneFlir_File::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

std::filesystem::path cTeledyneFlir_File::createHeaderFilename(char plotID)
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

std::filesystem::path cTeledyneFlir_File::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".csv";
    }
    else
        ext = ".csv";

    filename += ext;

    return filename;
}

void cTeledyneFlir_File::writeHeader(std::filesystem::path filename)
{}

void cTeledyneFlir_File::openDataFile()
{
    mDataFilename = createDataFilename(mPlotID);

/*
    mOutputFile.open(mDataFilename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += mDataFilename.string();
        throw std::runtime_error(msg);
    }
*/
}

void cTeledyneFlir_File::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cTeledyneFlir_File::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    openDataFile();

    mActiveRow = 0;
}

void cTeledyneFlir_File::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mOutputFile.close();

    mHeaderFilename = createHeaderFilename(mPlotID);

    writeHeader(mHeaderFilename);

    ++mPlotID;

    mActiveRow = 0;
}

void cTeledyneFlir_File::onModelName(uint8_t deviceID, std::string modelName) {}
void cTeledyneFlir_File::onFilter(uint8_t deviceID, std::string filter) {}
void cTeledyneFlir_File::onLens(uint8_t deviceID, std::string lens) {}
void cTeledyneFlir_File::onSerialNumber(uint8_t device_id, std::string serialNumber) {}
void cTeledyneFlir_File::onProgramVersion(uint8_t deviceID, std::string programVersion) {}
void cTeledyneFlir_File::onArticleNumber(uint8_t deviceID, std::string articleNumber) {}
void cTeledyneFlir_File::onCalibrationTitle(uint8_t deviceID, std::string calibrationTitle) {}
void cTeledyneFlir_File::onLensSerialNumber(uint8_t deviceID, std::string lensSerialNumber) {}
void cTeledyneFlir_File::onArcFileVersion(uint8_t deviceID, std::string arcFileVersion) {}
void cTeledyneFlir_File::onArcDateTime(uint8_t deviceID, std::string arcDateTime) {}
void cTeledyneFlir_File::onArcSignature(uint8_t deviceID, std::string arcSignature) {}
void cTeledyneFlir_File::onCountryCode(uint8_t deviceID, std::string countryCode) {}

void cTeledyneFlir_File::onFrameRate_Hz(uint8_t device_id, double frameRate_Hz) {}

void cTeledyneFlir_File::onFrameRateRange_Hz(uint8_t device_id, double minFrameRate_Hz, double maxFrameRate_Hz) {}
void cTeledyneFlir_File::onThermalRange_K(uint8_t device_id, float minThermalValue_K, float maxThermalValue_K) {}

void cTeledyneFlir_File::onHorizonalFoV_deg(uint8_t device_id, float horizonalFoV_deg) {}
void cTeledyneFlir_File::onFocalLength(uint8_t device_id, float focalLength) {}

void cTeledyneFlir_File::onImage(uint8_t device_id, nTeledyneAtlasConnect::cThermalImage image)
{
    auto min_K = image.minTemperature();
    auto max_K = image.maxTemperature();
}


