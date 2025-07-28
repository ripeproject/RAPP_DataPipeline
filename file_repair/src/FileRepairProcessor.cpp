
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

cFileRepairProcessor::eRETURN_TYPE cFileRepairProcessor::process_file()
{
    mDataFileRecovery = std::make_unique<cDataFileRecovery>(mID, mTemporaryDirectory);
    
    if (mDataFileRecovery->open(mInputFile))
    {
        new_file_progress(mID, mInputFile.string());

        mTemporaryFile = mDataFileRecovery->tempFileName();

        auto result = run();

        if (result == eResult::PARTIAL_REPAIR)
            return eRETURN_TYPE::FAILED;

        return eRETURN_TYPE::REPAIRED;
    }

    return eRETURN_TYPE::COULD_NOT_OPEN_FILE;
}

cFileRepairProcessor::eResult cFileRepairProcessor::run()
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

        return eResult::PARTIAL_REPAIR;
    }

    complete_file_progress(mID, "Complete", "Fully Repaired");

    moveToFullyRepaired();

    return eResult::REPAIRED;
}

void cFileRepairProcessor::moveToPartialRepaired()
{
    using namespace ceres_file_repair;

    ::create_directory(mPartialRepairedDirectory);

    std::filesystem::path out_file = mPartialRepairedDirectory / mInputFile.filename();

    std::filesystem::rename(mTemporaryFile, out_file);

    ++g_num_partial_files;
}

void cFileRepairProcessor::moveToFullyRepaired()
{
    using namespace ceres_file_repair;

    ::create_directory(mFullyRepairedDirectory);

    std::filesystem::path out_file = mFullyRepairedDirectory / mInputFile.filename();

    std::filesystem::rename(mTemporaryFile, out_file);

    ++g_num_repaired_files;
}

