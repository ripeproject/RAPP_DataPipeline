
#include "HySpexSWIR384_BIP.hpp"

#include <iostream>


cHySpexSWIR384_BIP::cHySpexSWIR384_BIP() : cHySpexSWIR384_File()
{}

cHySpexSWIR384_BIP::~cHySpexSWIR384_BIP()
{
    mOutputFile.close();

    if (mPlotID == 'B')
    {
        auto filename = createDataFilename('\0');
        std::filesystem::rename(mDataFilename, filename);
    }
}

std::filesystem::path cHySpexSWIR384_BIP::createDataFilename(char plotID)
{
    std::filesystem::path filename = mOutputPath;

    std::string ext;

    if (plotID > '@')
    {
        ext = ".";
        ext += mPlotID;
        ext += ".swir.bip";
    }
    else
        ext = ".swir.bip";

    filename += ext;

    return filename;
}

void cHySpexSWIR384_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
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

void cHySpexSWIR384_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mActiveRow;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}



