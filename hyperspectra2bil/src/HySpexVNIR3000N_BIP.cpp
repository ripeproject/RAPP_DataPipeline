
#include "HySpexVNIR3000N_BIP.hpp"

#include <iostream>


cHySpexVNIR3000N_BIP::cHySpexVNIR3000N_BIP() : cHySpexVNIR3000N_File()
{}

cHySpexVNIR3000N_BIP::~cHySpexVNIR3000N_BIP()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createDataFilename('\0');
        std::filesystem::rename(mDataFilename, filename);
    }
}

std::filesystem::path cHySpexVNIR3000N_BIP::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".vnir.bip";
    }
    else
        ext = ".vnir.bip";

    filename += ext;

    return filename;
}

void cHySpexVNIR3000N_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();

    for (int channel = 0; channel < n; ++channel)
    {
        auto bands = data.channel(channel);
        for (auto pixel : bands)
            mOutputFile.write(reinterpret_cast<char*>(&pixel), sizeof pixel);
    }
}

void cHySpexVNIR3000N_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    if (!mOutputFile.is_open())
        return;

    ++mActiveRow;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}

