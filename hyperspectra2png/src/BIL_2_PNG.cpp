
#include "BIL_2_PNG.hpp"

#include "Constants.hpp"
#include "MathUtils.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cBIL_2_Png::cBIL_2_Png()
{
}

cBIL_2_Png::~cBIL_2_Png()
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

    mMetaData.close();
}

void cBIL_2_Png::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;

    std::filesystem::path metafile = out;

    metafile.replace_extension(".bil.metaData.txt");

    mMetaData.open(metafile, std::ios::out | std::ios::trunc);
}

void cBIL_2_Png::setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm)
{
    mRed_nm = red_nm;
    mGreen_nm = green_nm;
    mBlue_nm = blue_nm;
}

void cBIL_2_Png::onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps)
{
    mResyncTimestamp = true;

    float4 xyz;
    xyz.x = x_mm / 1000.0;
    xyz.y = y_mm / 1000.0;
    xyz.z = z_mm / 1000.0;
    xyz.s = speed_mmps / 1000.0;

    mPositions.push_back(xyz);
}

void cBIL_2_Png::onStartRecordingTimestamp(uint64_t timestamp_ns)
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

void cBIL_2_Png::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
}


void cBIL_2_Png::writeRgbImage(std::filesystem::path filename)
{
    if (mActiveRow == 0)
        return;

    mImage.resize(mActiveRow);
    mMaxRows = mActiveRow;

    cv::String name = filename.string();
    cv::imwrite(name, mImage);
}

