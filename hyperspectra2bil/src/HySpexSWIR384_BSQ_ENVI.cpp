
#include "HySpexSWIR384_BSQ_ENVI.hpp"

#include <iostream>


cHySpexSWIR384_BSQ_ENVI::cHySpexSWIR384_BSQ_ENVI() : cHySpexSWIR384_BSQ()
{}

cHySpexSWIR384_BSQ_ENVI::~cHySpexSWIR384_BSQ_ENVI()
{
    writeHeader();
}

void cHySpexSWIR384_BSQ_ENVI::writeHeader()
{
    std::filesystem::path filename = mOutputPath;

    std::string ext = ".swir.hdr";

    filename += ext;

    std::ofstream header(filename, std::ios_base::binary);

    if (!header.is_open())
        return;

    header << "ENVI" << std::endl;
    header << "interleave = bsq" << std::endl;

    header << "header offset = 0" << std::endl;
    header << "file type = ENVI Standard" << std::endl;
    header << "data type = 12" << std::endl;
    header << "byte order = 0" << std::endl;
    header << "bands = " << mSpectralSize << std::endl;
    header << "lines = " << mNumFrames << std::endl;
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

