
#include "HySpexVNIR3000N_BIL.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexVNIR3000N_BIL::cHySpexVNIR3000N_BIL() : cHySpexVNIR3000N_File()
{
}

cHySpexVNIR3000N_BIL::~cHySpexVNIR3000N_BIL()
{
}

void cHySpexVNIR3000N_BIL::openDataFile()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".bil";
    filename += ext;

    mOutputFile.open(filename, std::ios_base::binary);

    if (!mOutputFile.is_open())
    {
        std::string msg = "Could not open: ";
        msg += filename.string();
        throw std::runtime_error(msg);
    }
}

void cHySpexVNIR3000N_BIL::writeHeader()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".hdr";

    filename += ext;

    std::ofstream header(filename, std::ios_base::binary);

    if (!header.is_open())
        return;

    int bandrowbytes = (mSpatialSize * static_cast<int>(mNumBits)) / 8;
    int totalrowbytes = mSpectralSize * bandrowbytes;

    header << "BYTEORDER      I" << std::endl;
    header << "LAYOUT         BIL" << std::endl;
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

    /*
        BYTEORDER      I
        LAYOUT         BIL
        NROWS          621
        NCOLS          1405
        NBANDS         1
        NBITS          32
        BANDROWBYTES   5620
        TOTALROWBYTES  5620
        PIXELTYPE      SIGNEDINT
        ULXMAP         -125
        ULYMAP         49.9166666666687
        XDIM           0.04166666666667
        YDIM           0.04166666666667
        NODATA - 9999
    */
}

void cHySpexVNIR3000N_BIL::onImage(HySpexConnect::cImageData<uint16_t> image)
{
    ++mNumFrames;

    auto n = image.spatialSize();
    auto m = image.spectralSize();
    auto data = image.image();

    for (int band = 0; band < m; ++band)
    {
        auto channels = data.band(band);
        for (auto pixel : channels)
            mOutputFile << pixel;
    }
}

void cHySpexVNIR3000N_BIL::onImage(HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip)
{
    ++mNumFrames;

    for (int i = 0; image.spatialSize(); ++i)
    {
        //        mImage[i] = { 128,128,128 };
    }
}

