
#include "HySpexSWIR384_BSQ.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexSWIR384_BSQ::cHySpexSWIR384_BSQ() : cHySpexSWIR384_File()
{}

cHySpexSWIR384_BSQ::~cHySpexSWIR384_BSQ()
{
}

void cHySpexSWIR384_BSQ::openDataFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".bsq";
    filename += ext;

    mOutputFile.open(filename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += filename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexSWIR384_BSQ::writeHeader()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".hdr";

    filename += ext;
}

void cHySpexSWIR384_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mNumFrames;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();
    //    mImage = image.image().data();
}

void cHySpexSWIR384_BSQ::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mNumFrames;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}


