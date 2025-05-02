
#include "HySpexVNIR3000N_BIL.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexVNIR3000N_BIL::cHySpexVNIR3000N_BIL() : cHySpexVNIR3000N_File()
{
}

cHySpexVNIR3000N_BIL::~cHySpexVNIR3000N_BIL()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createDataFilename('\0');
        std::filesystem::rename(mDataFilename, filename);
    }
}

std::filesystem::path cHySpexVNIR3000N_BIL::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".vnir.bil";
    }
    else
        ext = ".vnir.bil";

    filename += ext;

    return filename;
}

void cHySpexVNIR3000N_BIL::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();

    for (int band = 0; band < m; ++band)
    {
        auto channels = data.band(band);
        for (auto pixel : channels)
            mOutputFile.write(reinterpret_cast<char*>(&pixel), sizeof pixel);
    }
}

void cHySpexVNIR3000N_BIL::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}

