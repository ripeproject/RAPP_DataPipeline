
#include "HySpexVNIR3000N_BSQ.hpp"

#include <iostream>


cHySpexVNIR3000N_BSQ::cHySpexVNIR3000N_BSQ() : cHySpexVNIR3000N_File()
{}

cHySpexVNIR3000N_BSQ::~cHySpexVNIR3000N_BSQ()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createDataFilename('\0');
        std::filesystem::rename(mDataFilename, filename);
    }
}

std::filesystem::path cHySpexVNIR3000N_BSQ::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".vnir.bsq";
    }
    else
        ext = ".vnir.bsq";

    filename += ext;

    return filename;
}

void cHySpexVNIR3000N_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();

    //    mImage = image.image().data();
}

void cHySpexVNIR3000N_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}

