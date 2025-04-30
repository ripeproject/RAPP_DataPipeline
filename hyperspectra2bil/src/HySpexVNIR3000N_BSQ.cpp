
#include "HySpexVNIR3000N_BSQ.hpp"

#include <iostream>


cHySpexVNIR3000N_BSQ::cHySpexVNIR3000N_BSQ() : cHySpexVNIR3000N_File()
{}

cHySpexVNIR3000N_BSQ::~cHySpexVNIR3000N_BSQ()
{}

void cHySpexVNIR3000N_BSQ::openDataFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".vnir.bsq";
    filename += ext;

    mOutputFile.open(filename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += filename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexVNIR3000N_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mNumFrames;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();
    //    mImage = image.image().data();
}

void cHySpexVNIR3000N_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mNumFrames;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}

