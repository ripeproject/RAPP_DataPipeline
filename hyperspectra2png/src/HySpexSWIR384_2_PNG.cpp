
#include "HySpexSWIR384_2_PNG.hpp"

#include "MathUtils.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexSWIR384_2_Png::cHySpexSWIR384_2_Png() : cHySpexSWIR_384_Parser()
{}

cHySpexSWIR384_2_Png::~cHySpexSWIR384_2_Png()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (mPlotID != 'A')
    {
        ext = ".";
        ext += mPlotID;
    }

    ext += ".swir.png";

    filename += ext;

    writeRgbImage(filename);

    mImage.release();
}

void cHySpexSWIR384_2_Png::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cHySpexSWIR384_2_Png::setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm)
{
    mRed_nm = red_nm;
    mGreen_nm = green_nm;
    mBlue_nm = blue_nm;
}

void cHySpexSWIR384_2_Png::onID(std::string id) {}
void cHySpexSWIR384_2_Png::onSerialNumber(std::string serialNumber) {}
void cHySpexSWIR384_2_Png::onWavelengthRange_nm(uint16_t minWavelength_nm, uint16_t maxWavelength_nm) {}
void cHySpexSWIR384_2_Png::onSpatialSize(uint64_t spatialSize)
{
    mSpatialSize = spatialSize;
    mMaxRows = spatialSize;
    mImage.create(spatialSize, spatialSize, CV_8UC3);
}

void cHySpexSWIR384_2_Png::onSpectralSize(uint64_t spectralSize)
{
    mSpectralSize = spectralSize;
}

void cHySpexSWIR384_2_Png::onMaxSpatialSize(uint64_t maxSpatialSize) {}
void cHySpexSWIR384_2_Png::onMaxSpectralSize(uint64_t maxSpectralSize) {}
void cHySpexSWIR384_2_Png::onMaxPixelValue(uint16_t maxPixelValue)
{
    mColorScale = 255.0 / maxPixelValue;
    mColorScale *= 4.0;
}

void cHySpexSWIR384_2_Png::onResponsivityMatrix(HySpexConnect::cSpatialMajorData<float> re) {}
void cHySpexSWIR384_2_Png::onQuantumEfficiencyData(HySpexConnect::cSpectralData<float> qe) {}
void cHySpexSWIR384_2_Png::onLensName(std::string name) {}
void cHySpexSWIR384_2_Png::onLensWorkingDistance_cm(double workingDistance_cm) {}
void cHySpexSWIR384_2_Png::onLensFieldOfView_rad(double fieldOfView_rad) {}
void cHySpexSWIR384_2_Png::onLensFieldOfView_deg(double fieldOfView_deg) {}

void cHySpexSWIR384_2_Png::onAverageFrames(uint16_t averageFrames) {}
void cHySpexSWIR384_2_Png::onFramePeriod_us(uint32_t framePeriod_us) {}
void cHySpexSWIR384_2_Png::onIntegrationTime_us(uint32_t integrationTime_us) {}

void cHySpexSWIR384_2_Png::onBadPixels(HySpexConnect::cBadPixelData bad_pixels) {}
void cHySpexSWIR384_2_Png::onBadPixelCorrection(HySpexConnect::cBadPixelCorrectionData corrections) {}
void cHySpexSWIR384_2_Png::onBadPixelMatrix(HySpexConnect::cSpatialMajorData<uint8_t> matrix) {}

void cHySpexSWIR384_2_Png::onAmbientTemperature_C(float temp_C) {}

void cHySpexSWIR384_2_Png::onSpectralCalibration(HySpexConnect::cSpectralData<float> wavelengths_nm)
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

void cHySpexSWIR384_2_Png::onBackgroundMatrixAge_ms(int64_t age_ms) {}
void cHySpexSWIR384_2_Png::onNumOfBackgrounds(uint32_t numOfBackgrounds) {}
void cHySpexSWIR384_2_Png::onBackgroundMatrix(HySpexConnect::cSpatialMajorData<float> background) {}

void cHySpexSWIR384_2_Png::onImage(HySpexConnect::cImageData<uint16_t> image)
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

void cHySpexSWIR384_2_Png::onImage(HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    for (int i = 0; image.spatialSize(); ++i)
    {
//        mImage[i] = { 128,128,128 };
    }
}

void cHySpexSWIR384_2_Png::onSensorTemperature_K(float temp_K) {}

void cHySpexSWIR384_2_Png::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cHySpexSWIR384_2_Png::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    if (mActiveRow == 0)
        return;
    
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    ext = ".";
    ext += mPlotID;
    ext += ".swir.png";

    filename += ext;

    writeRgbImage(filename);

    ++mPlotID;

    mActiveRow = 0;
}

void cHySpexSWIR384_2_Png::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
}

void cHySpexSWIR384_2_Png::writeRgbImage(std::filesystem::path filename)
{
    if (mActiveRow == 0)
        return;

    mImage.resize(mActiveRow);
    mMaxRows = mActiveRow;

    cv::String name = filename.string();
    cv::imwrite(name, mImage);
}

