
#include "HySpexVNIR3000N_BIP.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexVNIR3000N_BIP::cHySpexVNIR3000N_BIP() : cHySpexVNIR3000N_File()
{
}

cHySpexVNIR3000N_BIP::~cHySpexVNIR3000N_BIP()
{
}

void cHySpexVNIR3000N_BIP::openDataFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".bip";
    filename += ext;

    mOutputFile.open(filename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += filename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexVNIR3000N_BIP::writeHeader()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".hdr";

    filename += ext;

    std::ofstream header(filename, std::ios_base::binary);

    if (!header.is_open())
        return;

    int bandrowbytes = (mSpatialSize * static_cast<int>(mNumBits)) / 8;
    int totalrowbytes = (mSpatialSize * mSpectralSize * static_cast<int>(mNumBits)) / 8;

    header << "BYTEORDER      I" << std::endl;
    header << "LAYOUT         BIP" << std::endl;
    header << "NROWS          " << mNumFrames << std::endl;
    header << "NCOLS          " << mSpatialSize << std::endl;
    header << "NBANDS         " << mSpectralSize << std::endl;
    header << "NBITS          " << static_cast<int>(mNumBits) << std::endl;
    header << "BANDROWBYTES   " << bandrowbytes << std::endl;
    header << "TOTALROWBYTES  " << totalrowbytes << std::endl;
    header << "ULXMAP         0" << std::endl;
    header << "ULYMAP         0" << std::endl;
    header << "XDIM           1" << std::endl;
    header << "YDIM           1" << std::endl;
    header << "NODATA         0" << std::endl;
}

void cHySpexVNIR3000N_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image)
{
    ++mNumFrames;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();

    for (int channel = 0; channel < n; ++channel)
    {
        auto bands = data.channel(channel);
        for (auto pixel : bands)
            mOutputFile << pixel;
    }
}

void cHySpexVNIR3000N_BIP::onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mNumFrames;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}

