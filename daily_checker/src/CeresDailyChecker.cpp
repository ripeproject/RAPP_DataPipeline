
#include "CeresDailyChecker.hpp"

#include <cbdf/ExperimentInfoLoader.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>

#include <string>
#include <atomic>


extern std::atomic<uint32_t> g_num_failed_files;
extern std::atomic<uint32_t> g_num_invalid_files;
extern std::atomic<uint32_t> g_num_missing_data;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string suffix);
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);

std::mutex g_invalid_dir_mutex;


namespace
{
    void create_directory(std::filesystem::path invalid_dir)
    {
        std::lock_guard<std::mutex> guard(g_invalid_dir_mutex);

        if (!std::filesystem::exists(invalid_dir))
        {
            std::filesystem::create_directory(invalid_dir);
        }
    }
}

//-----------------------------------------------------------------------------
cCeresDailyChecker::cCeresDailyChecker(int id, std::filesystem::path source_directory, std::filesystem::path exp_file)
:
    mID(id)
{
    mSourceDir = source_directory;
    mExperimentFile = exp_file;
}

cCeresDailyChecker::cCeresDailyChecker(int id, std::filesystem::directory_entry file_to_check,
    std::filesystem::path source_directory, std::filesystem::path exp_file)
:
    cCeresDailyChecker(id, source_directory, exp_file)
{
    mFileToCheck = file_to_check;

    mFileReader.open(file_to_check.path().string());

    if (!mFileReader.isOpen())
    {
    }

}

cCeresDailyChecker::~cCeresDailyChecker()
{
    mFileReader.close();
}

bool cCeresDailyChecker::setFileToCheck(std::filesystem::directory_entry file_to_check)
{
    mFileToCheck = file_to_check;
    return mFileToCheck.exists();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void cCeresDailyChecker::process_file()
{
    auto filename = mFileToCheck.path().filename();

    bool needs_repair = false;

    new_file_progress(mID, mFileToCheck.path().string());

    update_prefix_progress(mID, "Verifying File", 0);

    {
        std::filesystem::path failed_files = mSourceDir / "failed_files";
        std::unique_ptr<cCeresFileVerifier> pFileVerifier = std::make_unique<cCeresFileVerifier>(mID, mFileToCheck, failed_files);

        auto result = pFileVerifier->run();

        switch (result)
        {
        case cCeresFileVerifier::eRETURN_TYPE::COULD_NOT_OPEN_FILE:
            return;
        case cCeresFileVerifier::eRETURN_TYPE::PASSED:
            needs_repair = false;
            break;
        case cCeresFileVerifier::eRETURN_TYPE::INVALID_DATA:
        case cCeresFileVerifier::eRETURN_TYPE::INVALID_FILE:
            needs_repair = true;
            break;
        }

    }

    if (needs_repair)
    {
        update_prefix_progress(mID, "Repairing File", 0);

        std::filesystem::path failed_file = mSourceDir / "failed_files" / filename;

        std::filesystem::path tmp_dir = mSourceDir / "file_repair_tmp";
        std::filesystem::path partial_repaired_dir = mSourceDir / "partial_repaired_files";
        std::filesystem::path repaired_files = mSourceDir / "fully_repaired_files";

        std::unique_ptr<cFileRepairProcessor> pFileRepair = std::make_unique<cFileRepairProcessor>(mID, tmp_dir, partial_repaired_dir, repaired_files);

        if (pFileRepair->setFileToRepair(std::filesystem::directory_entry(failed_file)))
        {
            auto result = pFileRepair->run();

            std::filesystem::path repaired_file = repaired_files / filename;
            std::filesystem::path out_file = mSourceDir / filename;

            if (std::filesystem::exists(repaired_file) && !std::filesystem::exists(out_file))
            {
                std::filesystem::rename(repaired_file, out_file);
            }
            else
            {
                complete_file_progress(mID, "Error", "Error in copying repaired file!");
                return;
            }
        }
        else
        {
            complete_file_progress(mID, "Error", "Could not open failed file");
            return;
        }
    }

    needs_repair = false;

    update_prefix_progress(mID, "Verifying Data", 0);

    {
        std::filesystem::path invalid_data_files = mSourceDir / "invalid_data";
        std::unique_ptr<cCeresDataVerifier> pDataVerifier = std::make_unique<cCeresDataVerifier>(mID, mFileToCheck, invalid_data_files, mExperimentFile);

        auto result = pDataVerifier->run();

        switch (result)
        {
        case cCeresDataVerifier::eRETURN_TYPE::COULD_NOT_OPEN_FILE:
            complete_file_progress(mID, "Error", "Could not open failed file");
            return;
        case cCeresDataVerifier::eRETURN_TYPE::PASSED:
            needs_repair = false;
            break;
        case cCeresDataVerifier::eRETURN_TYPE::INVALID_DATA:
            needs_repair = true;
            break;
        case cCeresDataVerifier::eRETURN_TYPE::INVALID_FILE:
        {
            complete_file_progress(mID, "Error", "File is invalid.");
            return;

        }
        }
    }

    if (needs_repair)
    {
        update_prefix_progress(mID, "Repairing Data", 0);

        std::filesystem::path invalid_data_file = mSourceDir / "invalid_data" / filename;

        std::filesystem::path tmp_dir = mSourceDir / "data_repair_tmp";
        std::filesystem::path failed_data_files = mSourceDir / "failed_data_files";
        std::filesystem::path repaired_data_files = mSourceDir / "repaired_data_files";

        std::unique_ptr<cDataRepairProcessor> pDataRepair = std::make_unique<cDataRepairProcessor>(mID, tmp_dir, failed_data_files, repaired_data_files, mExperimentFile);

        if (pDataRepair->setFileToRepair(std::filesystem::directory_entry(invalid_data_file)))
        {
            auto result = pDataRepair->run();

            std::filesystem::path repaired_file = repaired_data_files / filename;
            std::filesystem::path out_file = mSourceDir / filename;

            if (std::filesystem::exists(repaired_file) && !std::filesystem::exists(out_file))
            {
                std::filesystem::rename(repaired_file, out_file);
            }
            else
            {
                complete_file_progress(mID, "Error", "Error in copying repaired file!");
                return;
            }
        }
        else
        {
            complete_file_progress(mID, "Error", "Could not open invalid data file");
            return;
        }

        complete_file_progress(mID, "Complete", "Fixed");
        return;
    }
    else
    {
        complete_file_progress(mID, "Complete", "Passed");
    }
}

