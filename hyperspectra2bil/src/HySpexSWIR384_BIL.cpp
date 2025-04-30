
#include "HySpexSWIR384_BIL.hpp"

#include <iostream>
#include <exception>


cHySpexSWIR384_BIL::cHySpexSWIR384_BIL() : cHySpexSWIR384_File()
{}

cHySpexSWIR384_BIL::~cHySpexSWIR384_BIL()
{
}

void cHySpexSWIR384_BIL::openDataFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".swir.bil";
    filename += ext;

    mOutputFile.open(filename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += filename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexSWIR384_BIL::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mNumFrames;

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

void cHySpexSWIR384_BIL::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mNumFrames;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}




