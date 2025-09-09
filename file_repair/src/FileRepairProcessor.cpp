
#include "FileRepairProcessor.hpp"

#include "DataFileRecovery.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>
#include <atomic>


namespace ceres_file_repair
{
    std::atomic<uint32_t> g_num_partial_files = 0;
    std::atomic<uint32_t> g_num_repaired_files = 0;

    std::mutex g_failed_dir_mutex;
}

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);

namespace
{
    void create_directory(std::filesystem::path failed_dir)
    {
        using namespace ceres_file_repair;

        std::lock_guard<std::mutex> guard(g_failed_dir_mutex);

        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}


cFileRepairProcessor::cFileRepairProcessor(int id, std::filesystem::path temp_dir,
                                std::filesystem::path partial_repaired_dir, 
                                std::filesystem::path fully_repaired_dir)
:
    mID(id), mTemporaryDirectory(temp_dir), mPartialRepairedDirectory(partial_repaired_dir), mFullyRepairedDirectory(fully_repaired_dir)
{
    mDataFileRecovery = std::make_unique<cDataFileRecovery>(mID, mTemporaryDirectory);
}

cFileRepairProcessor::~cFileRepairProcessor()
{
}

bool cFileRepairProcessor::setFileToRepair(std::filesystem::directory_entry file_to_repair)
{
    if (file_to_repair.exists())
    {
        mInputFile = file_to_repair.path();
        return true;
    }

    return false;
}

void cFileRepairProcessor::process_file()
{
    if (mDataFileRecovery->open(mInputFile))
    {
        new_file_progress(mID, mInputFile.string());

        auto result = run();

        if (result == eRETURN_TYPE::REPAIRED)
            complete_file_progress(mID, "Complete", "Fully Repaired");
        else
            complete_file_progress(mID, "Complete", "Partially Recovered");
    }
}

bool cFileRepairProcessor::open(std::filesystem::path file_to_repair)
{
    if (std::filesystem::exists(file_to_repair))
    {
        mInputFile = file_to_repair;
        auto result = mDataFileRecovery->open(mInputFile);
        return result;
    }

    return false;
}

cFileRepairProcessor::eRETURN_TYPE cFileRepairProcessor::run()
{
    if (!mDataFileRecovery->is_open())
        return eRETURN_TYPE::COULD_NOT_OPEN_FILE;

    mTemporaryFile = mDataFileRecovery->tempFileName();

    // Try to recover the data file
    if (!mDataFileRecovery->run())
    {
        if (!moveToPartialRepaired())
            return eRETURN_TYPE::ABORT;

        return eRETURN_TYPE::FAILED;
    }

    if (!moveToFullyRepaired())
        return eRETURN_TYPE::ABORT;

    return eRETURN_TYPE::REPAIRED;
}

std::filesystem::path cFileRepairProcessor::repairedFileName() const
{
    return mOutputFile;
}

bool cFileRepairProcessor::moveToPartialRepaired()
{
    using namespace ceres_file_repair;

    ::create_directory(mPartialRepairedDirectory);

    mOutputFile = mPartialRepairedDirectory / mInputFile.filename();

    try
    {
        std::filesystem::rename(mTemporaryFile, mOutputFile);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::string msg = "Move File To Partial Repaired: ";
        msg += e.what();
        console_message(msg);
        return false;
    }
    catch (const std::bad_alloc& e)
    {
        std::string msg = "Move File To Partial Repaired: ";
        msg += e.what();
        console_message(msg);
        return false;
    }

    ++g_num_partial_files;

    return true;
}

bool cFileRepairProcessor::moveToFullyRepaired()
{
    using namespace ceres_file_repair;

    ::create_directory(mFullyRepairedDirectory);

    mOutputFile = mFullyRepairedDirectory / mInputFile.filename();

    try
    {
        std::filesystem::rename(mTemporaryFile, mOutputFile);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::string msg = "Move File To Repaired: ";
        msg += e.what();
        console_message(msg);
        return false;
    }
    catch (const std::bad_alloc& e)
    {
        std::string msg = "Move File To Repaired: ";
        msg += e.what();
        console_message(msg);
        return false;
    }

    ++g_num_repaired_files;

    return true;
}

