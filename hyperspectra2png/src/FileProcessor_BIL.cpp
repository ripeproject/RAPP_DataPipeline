
#include "FileProcessor_BIL.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_file_progress(const int id, const int progress_pct);


cFileProcessor_BIL::cFileProcessor_BIL(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    cFileProcessor(id, in, out)
{
    mOutputFile.replace_extension("png");
}

cFileProcessor_BIL::~cFileProcessor_BIL()
{
}

bool cFileProcessor_BIL::open(std::filesystem::path out)
{
    std::filesystem::path headerFile = mInputFile;
    headerFile.replace_extension("hdr");

    if (!mConverter.loadHeader(headerFile.string()))
    {
        return false;
    }

    auto range = mConverter.getWavelengthRange();

    auto vnirFirst = mVnirBlue_nm;
    auto vnirLast = mVnirBlue_nm;

    vnirFirst = std::min(vnirFirst, mVnirGreen_nm);
    vnirFirst = std::min(vnirFirst, mVnirRed_nm);

    vnirLast = std::max(vnirFirst, mVnirGreen_nm);
    vnirLast = std::max(vnirFirst, mVnirRed_nm);

    if ((range.first <= vnirFirst) && (vnirLast <= range.second))
    {
        mConverter.setRgbWavelengths_nm(mVnirRed_nm, mVnirGreen_nm, mVnirBlue_nm);
    }
    else
    {
        mConverter.setRgbWavelengths_nm(mSwirRed_nm, mSwirGreen_nm, mSwirBlue_nm);
    }

    std::filesystem::path testFile  = out.replace_extension("png");

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    return true;
}

void cFileProcessor_BIL::process_file()
{
    if (open(mOutputFile))
    {
        new_file_progress(mID, mInputFile.string());

        run();
    }
}

void cFileProcessor_BIL::run()
{
    mConverter.writeRgbImage(mInputFile, mOutputFile);
}


