
#include "HySpexSWIR384_BIP_ArcMap.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>


cHySpexSWIR384_BIP_ArcMap::cHySpexSWIR384_BIP_ArcMap() : cHySpexSWIR384_BIP()
{}

cHySpexSWIR384_BIP_ArcMap::~cHySpexSWIR384_BIP_ArcMap()
{
    writeHeader();
}

void cHySpexSWIR384_BIP_ArcMap::writeHeader()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".swir.hdr";

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


