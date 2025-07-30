
#include "DataRepairProcessor.hpp"

#include "DataRepair.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <atomic>

namespace ceres_data_repair
{
    std::atomic<uint32_t> g_num_partial_data_files;
    std::atomic<uint32_t> g_num_repaired_data_files;

    std::mutex g_dir_mutex;
}

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);


namespace
{
    void create_directory(std::filesystem::path failed_dir)
    {
        using namespace ceres_data_repair;

        std::lock_guard<std::mutex> guard(g_dir_mutex);

        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}


cDataRepairProcessor::cDataRepairProcessor(int id, std::filesystem::path temp_dir,
                                        std::filesystem::path failed_dir,
                                        std::filesystem::path repaired_dir,
                                        std::filesystem::path exp_file)
:
    mID(id), mTemporaryDirectory(temp_dir), mFailedDirectory(failed_dir), 
    mRepairedDirectory(repaired_dir), mExperimentFile(exp_file)
{
    mDataRepair = std::make_unique<cDataRepair>(mID, mTemporaryDirectory, mExperimentFile);
}

cDataRepairProcessor::~cDataRepairProcessor()
{
}

bool cDataRepairProcessor::setFileToRepair(std::filesystem::directory_entry file_to_repair)
{
    if (file_to_repair.exists())
    {
        mFileToRepair = file_to_repair.path();
        return true;
    }

    return false;
}

void cDataRepairProcessor::process_file()
{
    if (mDataRepair->open(mFileToRepair))
    {
        mTemporaryFile = mDataRepair->tempFileName();

        new_file_progress(mID, mFileToRepair.string());

        auto result = run();

        switch (result)
        {
        case eRETURN_TYPE::PASSED:
        {
            complete_file_progress(mID, "Complete", "Fixed");

            return;
        }
        case eRETURN_TYPE::INVALID_DATA:
        {
            complete_file_progress(mID, "Complete", "Invalid Data");
            break;
        }

        case eRETURN_TYPE::INVALID_FILE:
        {
            complete_file_progress(mID, "Complete", "Failed");
            break;
        }
        }
    }
}

bool cDataRepairProcessor::open(std::filesystem::path file_to_repair)
{
    if (std::filesystem::exists(file_to_repair))
    {
        mFileToRepair = file_to_repair;
        auto result = mDataRepair->open(mFileToRepair);
        mTemporaryFile = mDataRepair->tempFileName();

        return result;
    }

    return false;
}

cDataRepairProcessor::eRETURN_TYPE cDataRepairProcessor::run()
{
    using namespace ceres_data_repair;

    // Read header data to check for missing data...
    auto result = mDataRepair->pass1();

    if (result == cDataRepair::eResult::INVALID_FILE)
    {
        mDataRepair->deleteTemporaryFile();

        ::create_directory(mFailedDirectory);

        std::filesystem::path dest = mFailedDirectory / mFileToRepair.filename();
        std::filesystem::rename(mFileToRepair, dest);

        ++g_num_partial_data_files;

        return eRETURN_TYPE::INVALID_FILE;
    }
    else if (result == cDataRepair::eResult::INVALID_DATA)
    {
        mDataRepair->deleteTemporaryFile();

        return eRETURN_TYPE::INVALID_DATA;
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

        ++g_num_partial_data_files;

        return eRETURN_TYPE::INVALID_FILE;
    }
    else if (result == cDataRepair::eResult::INVALID_DATA)
    {
        mDataRepair->deleteTemporaryFile();

        return eRETURN_TYPE::INVALID_DATA;
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

        ++g_num_partial_data_files;

        return eRETURN_TYPE::INVALID_FILE;
    }
    else if (result == cDataRepair::eResult::INVALID_DATA)
    {
        mDataRepair->deleteTemporaryFile();

        return eRETURN_TYPE::INVALID_DATA;
    }

    ::create_directory(mRepairedDirectory);

    std::filesystem::path dest = mRepairedDirectory / mTemporaryFile.filename();
    std::filesystem::rename(mTemporaryFile, dest);

    ++g_num_repaired_data_files;

    return eRETURN_TYPE::PASSED;
}

