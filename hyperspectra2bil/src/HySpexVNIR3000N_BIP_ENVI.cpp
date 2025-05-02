
#include "HySpexVNIR3000N_BIP_ENVI.hpp"

#include <iostream>


cHySpexVNIR3000N_BIP_ENVI::cHySpexVNIR3000N_BIP_ENVI() : cHySpexVNIR3000N_BIP()
{
}

cHySpexVNIR3000N_BIP_ENVI::~cHySpexVNIR3000N_BIP_ENVI()
{
    if (mOutputFile.is_open())
    {
        mHeaderFilename = createHeaderFilename(mPlotID);
        writeHeader(mHeaderFilename);
    }
}

void cHySpexVNIR3000N_BIP_ENVI::writeHeader(std::filesystem::path filename)
{
    std::ofstream header(filename, std::ios_base::binary);

    if (!header.is_open())
        return;

    header << "ENVI" << std::endl;
    header << "interleave = bip" << std::endl;

    header << "header offset = 0" << std::endl;
    header << "file type = ENVI Standard" << std::endl;
    header << "data type = 12" << std::endl;
    header << "byte order = 0" << std::endl;
    header << "bands = " << mSpectralSize << std::endl;
    header << "lines = " << mActiveRow << std::endl;
    header << "samples = " << mSpatialSize << std::endl;
    header << "wavelength units = nm" << std::endl;

    auto n = mSpectralCalibration.size();
    if (n > 0)
    {
        --n;
        header << "wavelength = {" << std::endl;

        for (std::size_t i = 0; i < n; ++i)
            header << mSpectralCalibration[i] << ", ";

        header << *mSpectralCalibration.rbegin() << "}" << std::endl;
    }
}


