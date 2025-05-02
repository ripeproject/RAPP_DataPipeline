
#include "HySpexSWIR384_BIL.hpp"

#include <iostream>
#include <exception>


cHySpexSWIR384_BIL::cHySpexSWIR384_BIL() : cHySpexSWIR384_File()
{}

cHySpexSWIR384_BIL::~cHySpexSWIR384_BIL()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createDataFilename('\0');
        std::filesystem::rename(mDataFilename, filename);
    }
}

std::filesystem::path cHySpexSWIR384_BIL::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".swir.bil";
    }
    else
        ext = ".swir.bil";

    filename += ext;

    return filename;
}

void cHySpexSWIR384_BIL::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mActiveRow;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();

    if (!mOutputFile.is_open())
        return;

    for (int band = 0; band < m; ++band)
    {
        auto channels = data.band(band);
        for (auto pixel : channels)
        {
            mOutputFile.write(reinterpret_cast<char*>(&pixel), sizeof pixel);
        }
    }
}

void cHySpexSWIR384_BIL::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mActiveRow;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}




