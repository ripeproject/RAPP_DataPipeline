
#include "HySpexVNIR3000N_2_PNG.hpp"

#include "MathUtils.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexVNIR3000N_2_Png::cHySpexVNIR3000N_2_Png() : cHySpexVNIR_3000N_Parser()
{
}

cHySpexVNIR3000N_2_Png::~cHySpexVNIR3000N_2_Png()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (mPlotID != 'A')
    {
        ext = ".";
        ext += mPlotID;
    }

    ext += ".vnir.png";

    filename += ext;

    writeRgbImage(filename);

    mImage.release();
}

void cHySpexVNIR3000N_2_Png::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cHySpexVNIR3000N_2_Png::setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm)
{
    mRed_nm = red_nm;
    mGreen_nm = green_nm;
    mBlue_nm = blue_nm;
}

void cHySpexVNIR3000N_2_Png::onID(uint8_t device_id, std::string id) {}
void cHySpexVNIR3000N_2_Png::onSerialNumber(uint8_t device_id, std::string serialNumber) {}
void cHySpexVNIR3000N_2_Png::onWavelengthRange_nm(uint8_t device_id, uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexVNIR3000N_2_Png::onSpatialSize(uint8_t device_id, uint64_t spatialSize)
{
    mSpatialSize = spatialSize;
    mMaxRows = spatialSize;
    mImage.create(spatialSize, spatialSize, CV_8UC3);
}

void cHySpexVNIR3000N_2_Png::onSpectralSize(uint8_t device_id, uint64_t spectralSize)
{
    mSpectralSize = spectralSize;
}

void cHySpexVNIR3000N_2_Png::onMaxSpatialSize(uint8_t device_id, uint64_t maxSpatialSize) {}
void cHySpexVNIR3000N_2_Png::onMaxSpectralSize(uint8_t device_id, uint64_t maxSpectralSize) {}
void cHySpexVNIR3000N_2_Png::onMaxPixelValue(uint8_t device_id, uint16_t maxPixelValue)
{
    mColorScale = 255.0 / maxPixelValue;
    mColorScale *= 4.0;
}

void cHySpexVNIR3000N_2_Png::onResponsivityMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexVNIR3000N_2_Png::onQuantumEfficiencyData(uint8_t device_id, HySpexConnect::cSpectralData<float> qe) {}
void cHySpexVNIR3000N_2_Png::onLensName(uint8_t device_id, std::string name) {}
void cHySpexVNIR3000N_2_Png::onLensWorkingDistance_cm(uint8_t device_id, double workingDistance_cm) {}
void cHySpexVNIR3000N_2_Png::onLensFieldOfView_rad(uint8_t device_id, double fieldOfView_rad) {}
void cHySpexVNIR3000N_2_Png::onLensFieldOfView_deg(uint8_t device_id, double fieldOfView_deg) {}

void cHySpexVNIR3000N_2_Png::onAverageFrames(uint8_t device_id, uint16_t averageFrames) {}
void cHySpexVNIR3000N_2_Png::onFramePeriod_us(uint8_t device_id, uint32_t framePeriod_us) {}
void cHySpexVNIR3000N_2_Png::onIntegrationTime_us(uint8_t device_id, uint32_t integrationTime_us) {}

void cHySpexVNIR3000N_2_Png::onBadPixels(uint8_t device_id, HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexVNIR3000N_2_Png::onBadPixelCorrection(uint8_t device_id, HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexVNIR3000N_2_Png::onBadPixelMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexVNIR3000N_2_Png::onAmbientTemperature_C(uint8_t device_id, float temp_C) {}

void cHySpexVNIR3000N_2_Png::onSpectralCalibration(uint8_t device_id, HySpexConnect::cSpectralData<float> wavelengths_nm)
{
    mRedIndex = 279;
    mGreenIndex = 169;
    mBlueIndex = 69;

    float red_diff = 10000.0f;
    float green_diff = 10000.0f;
    float blue_diff = 10000.0f;

    for (std::size_t i = 0; i < wavelengths_nm.size(); ++i)
    {
        float wavelength = wavelengths_nm[i];

        if (std::abs(wavelength - mRed_nm) < red_diff)
        {
            red_diff = std::abs(wavelength - mRed_nm);
            mRedIndex = i;
        }

        if (std::abs(wavelength - mGreen_nm) < green_diff)
        {
            green_diff = std::abs(wavelength - mGreen_nm);
            mGreenIndex = i;
        }

        if (std::abs(wavelength - mBlue_nm) < blue_diff)
        {
            blue_diff = std::abs(wavelength - mBlue_nm);
            mBlueIndex = i;
        }
    }
}

void cHySpexVNIR3000N_2_Png::onBackgroundMatrixAge_ms(uint8_t device_id, int64_t age_ms) {}
void cHySpexVNIR3000N_2_Png::onNumOfBackgrounds(uint8_t device_id, uint32_t numOfBackgrounds) {}
void cHySpexVNIR3000N_2_Png::onBackgroundMatrix(uint8_t device_id, HySpexConnect::cSpatialMajorData<float> background) {}

void cHySpexVNIR3000N_2_Png::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    if (mActiveRow >= mMaxRows)
    {
        mImage.resize(mActiveRow + mSpatialSize);
        mMaxRows = mActiveRow + mSpatialSize;
    }

    auto n = image.spatialSize();
    auto data = image.image();
    auto red = data.band(mRedIndex);
    auto green = data.band(mGreenIndex);
    auto blue = data.band(mBlueIndex);

    for (int i = 0; i < n; ++i)
    {
        uint8_t r = nMathUtils::bound<uint8_t>(red[i] * mColorScale);
        uint8_t g = nMathUtils::bound<uint8_t>(green[i] * mColorScale);
        uint8_t b = nMathUtils::bound<uint8_t>(blue[i] * mColorScale);
        cv::Point p(i, mActiveRow);
        mImage.at<cv::Vec3b>(p) = { b,g,r };
    }

    ++mActiveRow;
}

void cHySpexVNIR3000N_2_Png::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    for (int i = 0; image.spatialSize(); ++i)
    {
//        mImage[i] = { 128,128,128 };
    }
}

void cHySpexVNIR3000N_2_Png::onSensorTemperature_C(uint8_t device_id, float temp_C) {}

void cHySpexVNIR3000N_2_Png::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cHySpexVNIR3000N_2_Png::onStartRecordingTimestamp(uint64_t timestamp_ns) 
{
    if (mActiveRow == 0)
        return;

    std::filesystem::path filename = mOutputPath;

    std::string ext;

    ext = ".";
    ext += mPlotID;
    ext += ".vnir.png";

    filename += ext;

    writeRgbImage(filename);

    ++mPlotID;

    mActiveRow = 0;
}

void cHySpexVNIR3000N_2_Png::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
}


void cHySpexVNIR3000N_2_Png::writeRgbImage(std::filesystem::path filename)
{
    if (mActiveRow == 0)
        return;

    mImage.resize(mActiveRow);
    mMaxRows = mActiveRow;

    cv::String name = filename.string();
    cv::imwrite(name, mImage);
}

