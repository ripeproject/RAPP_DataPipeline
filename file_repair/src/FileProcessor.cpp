
#include "FileProcessor.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <atomic>


extern std::atomic<uint32_t> g_num_failed_files;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);

std::mutex g_failed_dir_mutex;

namespace
{
    void create_directory(std::filesystem::path failed_dir)
    {
        std::lock_guard<std::mutex> guard(g_failed_dir_mutex);

        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}


cFileProcessor::cFileProcessor(int id, std::filesystem::path temp_dir,
                                std::filesystem::path partial_repaired_dir, 
                                std::filesystem::path fully_repaired_dir)
:
    mID(id), mTemporaryDirectory(temp_dir), mPartialRepairedDirectory(partial_repaired_dir), mFullyRepairedDirectory(fully_repaired_dir)
{
}

cFileProcessor::~cFileProcessor()
{
}

bool cFileProcessor::setFileToRepair(std::filesystem::directory_entry file_to_repair)
{
    if (file_to_repair.exists())
    {
        mInputFile = file_to_repair.path();
        return true;
    }

    return false;
}

void cFileProcessor::process_file()
{
    mDataFileRecovery = std::make_unique<cDataFileRecovery>(mID, mTemporaryDirectory);
    
    if (mDataFileRecovery->open(mInputFile))
    {
        new_file_progress(mID, mInputFile.string());

        mTemporaryFile = mDataFileRecovery->tempFileName();

        run();
    }
}

void cFileProcessor::run()
{
    // Try to recover the data file
    if (!mDataFileRecovery->run())
    {
        complete_file_progress(mID, "Complete", "Partially Recovered");

/*
        std::string msg = "Warning: the file ";
        msg += mInputFile.filename().string();
        msg += " could not be completely recovered!";
        console_message(msg);
*/

        moveToPartialRepaired();

        ++g_num_failed_files;
    }
    else
    {
        complete_file_progress(mID, "Complete", "Fully Repaired");

        moveToFullyRepaired();
    }
}

void cFileProcessor::moveToPartialRepaired()
{
    ::create_directory(mPartialRepairedDirectory);

    std::filesystem::path out_file = mPartialRepairedDirectory / mInputFile.filename();

    std::filesystem::copy_file(mTemporaryFile, out_file);
    std::filesystem::remove(mTemporaryFile);
}

void cFileProcessor::moveToFullyRepaired()
{
    ::create_directory(mFullyRepairedDirectory);

    std::filesystem::path out_file = mFullyRepairedDirectory / mInputFile.filename();

    std::filesystem::copy_file(mTemporaryFile, out_file);
    std::filesystem::remove(mTemporaryFile);
}

