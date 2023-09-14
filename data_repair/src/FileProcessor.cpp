
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
extern void update_file_progress(const int id, std::string filename, const int progress_pct);
extern void update_file_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string filename, std::string suffix);


cFileProcessor::cFileProcessor(int id, std::filesystem::path recovered_dir, std::filesystem::path repaired_dir)
    : mID(id), mRecoveredDirectory(recovered_dir), mRepairedDirectory(repaired_dir)
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
    mDataFileRecovery = std::make_unique<cDataFileRecovery>(mID, mRecoveredDirectory);
    
    if (mDataFileRecovery->open(mInputFile))
    {
        new_file_progress(mID, mInputFile.string());

        mRecoveredFile = mDataFileRecovery->recoveredFileName();

        mDataRepair = std::make_unique<cDataRepair>(mID, mRepairedDirectory);

        run();
    }
}

void cFileProcessor::run()
{
    // Try to recover the data file
    if (!mDataFileRecovery->run())
    {
        complete_file_progress(mID, mInputFile.string(), "incomplete");

        std::string msg = "Warning: the file ";
        msg += mInputFile.filename().string();
        msg += " could not be completely recovered!";
        console_message(msg);

        ++g_num_failed_files;
    }

    if (mDataRepair->open(mRecoveredFile))
    {
        mDataRepair->run();
        complete_file_progress(mID, mInputFile.string(), "recovered");
    }
}

