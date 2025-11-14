
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
{}

cFileProcessor_BIL::~cFileProcessor_BIL()
{
}

bool cFileProcessor_BIL::open(std::filesystem::path out)
{
    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    return false;
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
}


