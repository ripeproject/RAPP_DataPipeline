
#include "TeledyneFLIR_Exporter.hpp"

#include "IronbowColorTable.hpp"
#include "RainbowColorTable.hpp"
#include "Rainbow_HC_ColorTable.hpp"
#include "WhiteHotColorTable.hpp"
#include "BlackHotColorTable.hpp"
#include "ArcticColorTable.hpp"
#include "ColorTable.hpp"

#include "Constants.hpp"
#include "MathUtils.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cTeledyneFLIR_Exporter::cTeledyneFLIR_Exporter() : cTeledyneFlirParser()
{
}

cTeledyneFLIR_Exporter::~cTeledyneFLIR_Exporter()
{
    if (mImageIsDirty)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext;

        if (mPlotID != 'A')
        {
            ext = ".";
            ext += mPlotID;
        }

        ext += ".png";

        filename += ext;

        writeImage(filename);

        mImage.release();
    }

    mMetaData.close();
}

void cTeledyneFLIR_Exporter::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;

    std::filesystem::path metafile = out;

    metafile.replace_extension(".metaData.txt");

    mMetaData.open(metafile, std::ios::out | std::ios::trunc);
}

void cTeledyneFLIR_Exporter::setScaleColorToImage(bool enable)
{
    mScaleToImage = enable;
}

void cTeledyneFLIR_Exporter::setScaleColorToCamera(bool enable)
{
    mScaleToImage = !enable;
}

void cTeledyneFLIR_Exporter::setColorTable(eColorTable color_table)
{
    switch (color_table)
    {
    case eColorTable::IRONBOW:
        mColorTable = std::make_unique<cIronbowColorTable>();
        break;
    case eColorTable::RAINBOW:
        mColorTable = std::make_unique<cRainbowColorTable>();
        break;
    case eColorTable::RAINBOW_HC: 
        mColorTable = std::make_unique<cRainbow_HC_ColorTable>();
        break;
    case eColorTable::WHITE_HOT:
        mColorTable = std::make_unique<cWhiteHotColorTable>();
        break;
    case eColorTable::BLACK_HOT:
        mColorTable = std::make_unique<cBlackHotColorTable>();
        break;
    case eColorTable::ARCTIC:
        mColorTable = std::make_unique<cArcticColorTable>();
        break;
    }

}

void cTeledyneFLIR_Exporter::onModelName(uint8_t deviceID, std::string modelName)
{
    mModelName = modelName;

    if (mMetaData.is_open())
    {
        mMetaData << "Mode Name: " << mModelName << "\n";
    }
}

void cTeledyneFLIR_Exporter::onFilter(uint8_t deviceID, std::string filter)
{
    mFilter = filter;

    if (mMetaData.is_open())
    {
        mMetaData << "Filter: " << mFilter << "\n";
    }
}

void cTeledyneFLIR_Exporter::onLens(uint8_t deviceID, std::string lens)
{
    mLens = lens;

    if (mMetaData.is_open())
    {
        mMetaData << "Lens: " << mLens << "\n";
    }
}

void cTeledyneFLIR_Exporter::onSerialNumber(uint8_t device_id, std::string serialNumber)
{
    mSerialNumber = serialNumber;

    if (mMetaData.is_open())
    {
        mMetaData << "Serial Number: " << mSerialNumber << "\n";
    }
}

void cTeledyneFLIR_Exporter::onProgramVersion(uint8_t deviceID, std::string programVersion)
{
    mProgramVersion = programVersion;

    if (mMetaData.is_open())
    {
        mMetaData << "Program Version: " << mProgramVersion << "\n";
    }
}

void cTeledyneFLIR_Exporter::onArticleNumber(uint8_t deviceID, std::string articleNumber)
{
    mArticleNumber = articleNumber;

    if (mMetaData.is_open())
    {
        mMetaData << "Article Number: " << mArticleNumber << "\n";
    }
}

void cTeledyneFLIR_Exporter::onCalibrationTitle(uint8_t deviceID, std::string calibrationTitle)
{
    mCalibrationTitle = calibrationTitle;

    if (mMetaData.is_open())
    {
        mMetaData << "Calibration Title: " << mCalibrationTitle << "\n";
    }
}

void cTeledyneFLIR_Exporter::onLensSerialNumber(uint8_t deviceID, std::string lensSerialNumber)
{
    mLensSerialNumber = lensSerialNumber;

    if (mMetaData.is_open())
    {
        mMetaData << "Lens Serial Number: " << mLensSerialNumber << "\n";
    }
}

void cTeledyneFLIR_Exporter::onArcFileVersion(uint8_t deviceID, std::string arcFileVersion)
{
    mArcFileVersion = arcFileVersion;

    if (mMetaData.is_open())
    {
        mMetaData << "ARC File Version: " << mArcFileVersion << "\n";
    }
}

void cTeledyneFLIR_Exporter::onArcDateTime(uint8_t deviceID, std::string arcDateTime)
{
    mArcDateTime = arcDateTime;

    if (mMetaData.is_open())
    {
        mMetaData << "ARC Date Time: " << mArcDateTime << "\n";
    }
}

void cTeledyneFLIR_Exporter::onArcSignature(uint8_t deviceID, std::string arcSignature)
{
    mArcSignature = arcSignature;

    if (mMetaData.is_open())
    {
        mMetaData << "ARC Signature: " << mArcSignature << "\n";
    }
}

void cTeledyneFLIR_Exporter::onCountryCode(uint8_t deviceID, std::string countryCode)
{
    mCountryCode = countryCode;

    if (mMetaData.is_open())
    {
        mMetaData << "Country Code: " << mSerialNumber << "\n";
    }
}

void cTeledyneFLIR_Exporter::onFrameRate_Hz(uint8_t device_id, double frameRate_Hz)
{
    mFrameRate_Hz = frameRate_Hz;

    if (mMetaData.is_open())
    {
        mMetaData << "Frame Rate (Hz): " << mFrameRate_Hz << "\n";
    }
}

void cTeledyneFLIR_Exporter::onFrameRateRange_Hz(uint8_t device_id, double minFrameRate_Hz, double maxFrameRate_Hz)
{
    mMinFrameRate_Hz = minFrameRate_Hz;
    mMaxFrameRate_Hz = maxFrameRate_Hz;

    if (mMetaData.is_open())
    {
        mMetaData << "Frame Rate Range (Hz): " << mMinFrameRate_Hz << " to " << mMaxFrameRate_Hz << "\n";
    }
}

void cTeledyneFLIR_Exporter::onThermalRange_K(uint8_t device_id, float minThermalValue_K, float maxThermalValue_K)
{
    mMinThermalValue_K = minThermalValue_K;
    mMaxThermalValue_K = maxThermalValue_K;

    if (mMetaData.is_open())
    {
        mMetaData << "Camera Thermal Range (K): " << mMinThermalValue_K << " to " << mMaxThermalValue_K << "\n";
    }

    if (!mScaleToImage && mColorTable)
        mColorTable->setRange(mMinThermalValue_K, mMaxThermalValue_K);
}

void cTeledyneFLIR_Exporter::onHorizonalFoV_deg(uint8_t device_id, float horizonalFoV_deg)
{
    mHorizonalFoV_deg = horizonalFoV_deg;

    if (mMetaData.is_open())
    {
        mMetaData << "Horizonal FoV (deg): " << mHorizonalFoV_deg << "\n";
    }
}

void cTeledyneFLIR_Exporter::onFocalLength(uint8_t device_id, float focalLength)
{
    mFocalLength = focalLength;

    if (mMetaData.is_open())
    {
        mMetaData << "Focal Length: " << mFocalLength << "\n";
    }
}

void cTeledyneFLIR_Exporter::onImage(uint8_t device_id, nTeledyneAtlasConnect::cThermalImage image)
{
    if (image.empty())
        return;

    auto range = image.findThermalLimits();

    if (mScaleToImage && mColorTable)
        mColorTable->setRange(range.minTemp_K, range.maxTemp_K);

    if (mMetaData.is_open())
    {
        if (!mImageSizeWritten)
        {
            mMetaData << "Image Size: " << image.width() << " x " << image.height() << "\n";
            mImageSizeWritten = true;
        }

        mMetaData << "Plot " << mPlotID << " Thermal Range (K): " << range.minTemp_K << " to " << range.maxTemp_K << "\n";
    }

    auto numOfRows = image.height();
    auto numOfCols = image.width();

    std::fstream thermalData;

    std::filesystem::path thermalfile = mOutputPath;

    std::string ext;

    ext = ".";
    ext += mPlotID;
    ext += ".csv";

    thermalfile.replace_extension(ext);

    thermalData.open(thermalfile, std::ios::out | std::ios::trunc);

    if (thermalData.is_open())
    {
        for (int j = 0; j < numOfRows; ++j)
        {
            auto row = image.row(j);

            for (int i = 0; i < (numOfCols-1); ++i)
            {
                thermalData << row[i] << ", ";
            }

            thermalData << row[numOfCols - 1] << "\n";
        }
    }

    thermalData.close();

    if (mColorTable)
    {
        mImage.create(numOfRows, numOfCols, CV_8UC3);

        for (int j = 0; j < numOfRows; ++j)
        {
            auto row = image.row(j);

            for (int i = 0; i < numOfCols; ++i)
            {
                cv::Point p(i, j);

                auto color = mColorTable->getColorRGB(row[i]);
                mImage.at<cv::Vec3b>(p) = { color.blue, color.green, color.red };
            }
        }

        mImageIsDirty = true;

        std::filesystem::path filename = mOutputPath;

        std::string ext;

        ext = ".";
        ext += mPlotID;
        ext += ".png";

        filename += ext;

        writeImage(filename);

        ++mPlotID;
    }

/*
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
*/
}

void cTeledyneFLIR_Exporter::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cTeledyneFLIR_Exporter::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
}

void cTeledyneFLIR_Exporter::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
}

void cTeledyneFLIR_Exporter::writeImage(std::filesystem::path filename)
{
    cv::String name = filename.string();
    cv::imwrite(name, mImage);

    mImageIsDirty = false;
}

