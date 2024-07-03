
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
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);


std::mutex g_dir_mutex;

namespace
{
    void create_directory(std::filesystem::path failed_dir)
    {
        std::lock_guard<std::mutex> guard(g_dir_mutex);

        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}


cFileProcessor::cFileProcessor(int id, std::filesystem::path temp_dir, 
                                        std::filesystem::path failed_dir,
                                        std::filesystem::path repaired_dir,
                                        std::filesystem::path exp_file)
:
    mID(id), mTemporaryDirectory(temp_dir), mFailedDirectory(failed_dir), 
    mRepairedDirectory(repaired_dir), mExperimentFile(exp_file)
{
}

cFileProcessor::~cFileProcessor()
{
}

bool cFileProcessor::setFileToRepair(std::filesystem::directory_entry file_to_repair)
{
    if (file_to_repair.exists())
    {
        mFileToRepair = file_to_repair.path();
        return true;
    }

    return false;
}

void cFileProcessor::process_file()
{
    mDataRepair = std::make_unique<cDataRepair>(mID, mTemporaryDirectory, mExperimentFile);

    if (mDataRepair->open(mFileToRepair))
    {
        mTemporaryFile = mDataRepair->tempFileName();

        new_file_progress(mID, mFileToRepair.string());

        run();
    }
}

void cFileProcessor::run()
{
    // Read header data to check for missing data...
    auto result = mDataRepair->pass1();

    if (result == cDataRepair::eResult::INVALID_FILE)
    {
        mDataRepair->deleteTemporaryFile();

        ::create_directory(mFailedDirectory);

        std::filesystem::path dest = mFailedDirectory / mFileToRepair.filename();
        std::filesystem::rename(mFileToRepair, dest);

        ++g_num_failed_files;

        complete_file_progress(mID, "Complete", "Failed");

        return;
    }
    else if (result == cDataRepair::eResult::INVALID_DATA)
    {
        mDataRepair->deleteTemporaryFile();

        complete_file_progress(mID, "Complete", "Invalid Data");

        return;
    }
    else if (result == cDataRepair::eResult::MISSING_DATA)
    {

    }

    // Try to repaired data in file...
    result = mDataRepair->pass2();

    if (result == cDataRepair::eResult::INVALID_FILE)
    {
        mDataRepair->deleteTemporaryFile();

        ::create_directory(mFailedDirectory);

        std::filesystem::path dest = mFailedDirectory / mFileToRepair.filename();
        std::filesystem::rename(mFileToRepair, dest);

        ++g_num_failed_files;

        complete_file_progress(mID, "Complete", "Failed");

        return;
    }
    else if (result == cDataRepair::eResult::INVALID_DATA)
    {
        mDataRepair->deleteTemporaryFile();

        complete_file_progress(mID, "Complete", "Invalid Data");

        return;
    }

    // Validate the repaired file...
    result = mDataRepair->pass3();

    if (result == cDataRepair::eResult::INVALID_FILE)
    {
        // This should not happen at this point!
        mDataRepair->deleteTemporaryFile();

        ::create_directory(mFailedDirectory);

        std::filesystem::path dest = mFailedDirectory / mFileToRepair.filename();
        std::filesystem::rename(mFileToRepair, dest);

        ++g_num_failed_files;

        complete_file_progress(mID, "Complete", "Failed");

        return;
    }
    else if (result == cDataRepair::eResult::INVALID_DATA)
    {
        mDataRepair->deleteTemporaryFile();

        complete_file_progress(mID, "Complete", "Invalid Data");

        return;
    }

    ::create_directory(mRepairedDirectory);

    std::filesystem::path dest = mRepairedDirectory / mTemporaryFile.filename();
    std::filesystem::rename(mTemporaryFile, dest);

    complete_file_progress(mID, "Complete", "Fixed");
}

