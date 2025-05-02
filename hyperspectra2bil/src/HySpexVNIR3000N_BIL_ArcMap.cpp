
#include "HySpexVNIR3000N_BIL_ArcMap.hpp"

#include <iostream>


cHySpexVNIR3000N_BIL_ArcMap::cHySpexVNIR3000N_BIL_ArcMap() : cHySpexVNIR3000N_BIL()
{
}

cHySpexVNIR3000N_BIL_ArcMap::~cHySpexVNIR3000N_BIL_ArcMap()
{
    if (mOutputFile.is_open())
    {
        mHeaderFilename = createHeaderFilename(mPlotID);
        writeHeader(mHeaderFilename);
    }
}

void cHySpexVNIR3000N_BIL_ArcMap::writeHeader(std::filesystem::path filename)
{
    std::ofstream header(filename, std::ios_base::binary);

    if (!header.is_open())
        return;

    int bandrowbytes = (mSpatialSize * static_cast<int>(mNumBits)) / 8;
    int totalrowbytes = mSpectralSize * bandrowbytes;

    header << "BYTEORDER      I" << std::endl;
    header << "LAYOUT         BIL" << std::endl;
    header << "NROWS          " << mActiveRow << std::endl;
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


