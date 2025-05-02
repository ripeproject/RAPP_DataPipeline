
#include "HySpexSWIR384_BSQ.hpp"

#include <iostream>


cHySpexSWIR384_BSQ::cHySpexSWIR384_BSQ() : cHySpexSWIR384_File()
{}

cHySpexSWIR384_BSQ::~cHySpexSWIR384_BSQ()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createDataFilename('\0');
        std::filesystem::rename(mDataFilename, filename);
    }
}

std::filesystem::path cHySpexSWIR384_BSQ::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".swir.bsq";
    }
    else
        ext = ".swir.bsq";

    filename += ext;

    return filename;
}

void cHySpexSWIR384_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mActiveRow;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();
    //    mImage = image.image().data();
}

void cHySpexSWIR384_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}


