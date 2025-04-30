
#include "HySpexSWIR384_BIP.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexSWIR384_BIP::cHySpexSWIR384_BIP() : cHySpexSWIR384_File()
{}

cHySpexSWIR384_BIP::~cHySpexSWIR384_BIP()
{}

void cHySpexSWIR384_BIP::openDataFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".swir.bip";
    filename += ext;

    mOutputFile.open(filename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += filename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexSWIR384_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mNumFrames;

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
    ++mNumFrames;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}



