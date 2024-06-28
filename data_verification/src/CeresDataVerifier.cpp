
#include "CeresDataVerifier.hpp"
#include "ParserExceptions.hpp"
#include "OusterVerificationParser.hpp"
#include "AxisCommunicationsVerificationParser.hpp"
#include "ExperimentInfoFromJson.hpp"

#include <cbdf/ExperimentInfoLoader.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>

#include <string>
#include <atomic>


extern std::atomic<uint32_t> g_num_failed_files;
extern std::atomic<uint32_t> g_num_invalid_files;
extern std::atomic<uint32_t> g_num_missing_data;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string suffix);

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
cCeresDataVerifier::cCeresDataVerifier(int id, std::filesystem::path invalid_dir, std::filesystem::path exp_file)
:
    mID(id)
{
    mInvalidDirectory = invalid_dir;
    mExperimentFile = exp_file;

    mExperimentInfo = std::make_shared<cExperimentInfo>();
}

cCeresDataVerifier::cCeresDataVerifier(int id, std::filesystem::directory_entry file_to_check,
    std::filesystem::path invalid_dir, std::filesystem::path exp_file)
:
    mID(id)
{
    mFileToCheck = file_to_check;
    mInvalidDirectory = invalid_dir;
    mExperimentFile = exp_file;

    mFileReader.open(file_to_check.path().string());

    if (!mFileReader.isOpen())
    {
        throw std::logic_error(mFileToCheck.string());
    }

    mExperimentInfo = std::make_shared<cExperimentInfo>();
}

cCeresDataVerifier::~cCeresDataVerifier()
{
    mFileReader.close();
}

bool cCeresDataVerifier::setFileToCheck(std::filesystem::directory_entry file_to_check)
{
    mFileToCheck = file_to_check;
    return true;
}

//-----------------------------------------------------------------------------
bool cCeresDataVerifier::open(std::filesystem::path file_to_check)
{
    if (mFileReader.isOpen())
        mFileReader.close();
        
    mFileReader.open(file_to_check.string());
    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cCeresDataVerifier::process_file()
{
    if (open(mFileToCheck))
    {
        mFileSize = mFileReader.file_size();

        new_file_progress(mID, mFileToCheck.string());

        run();
    }
}

//-----------------------------------------------------------------------------
void cCeresDataVerifier::run()
{
    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    auto info = std::make_unique<cExperimentInfoLoader>(mExperimentInfo);
    auto ouster = std::make_unique<cOusterVerificationParser>();
    auto axis = std::make_unique<cAxisCommunicationsVerificationParser>();

    mFileReader.attach(info.get());
    mFileReader.attach(ouster.get());
    mFileReader.attach(axis.get());

    mFileSize = mFileReader.file_size();

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                ++g_num_failed_files;

                complete_file_progress(mID, "Failed!");

                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Invalid Data, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();

        moveFileToInvalid();

        complete_file_progress(mID, "Data Invalid");

        return;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();
        ++g_num_failed_files;

        complete_file_progress(mID, "Failed!");

        return;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mFileToCheck.string();
            msg += ":  std::exception, ";
            msg += e.what();
            console_message(msg);

            mFileReader.close();
            ++g_num_failed_files;

            complete_file_progress(mID, "Failed!");

            return;
        }
    }

    // Check the experiment information to see is the most basic information is there...
    if (!mExperimentInfo->experimentDoc().empty())
    {
        cExperimentInfoFromJson info;
        info.parse(mExperimentInfo->experimentDoc());

        if (info != *mExperimentInfo)
        {
            std::string msg = mFileToCheck.string();
            msg += ": Missing experiment information!";
            console_message(msg);

            moveFileToInvalid();

            complete_file_progress(mID, "Data Invalid");

            return;
        }

        complete_file_progress(mID, "Passed");
        return;
    }
    else
    {
        if (mExperimentFile.empty())
        {
            if (mExperimentInfo->title().empty())
            {
                std::string msg = mFileToCheck.string();
                msg += ": Missing experiment title!";
                console_message(msg);

                moveFileToInvalid();

                complete_file_progress(mID, "Data Invalid");

                return;
            }
        }
        else
        {
            std::ifstream in;
            in.open(mExperimentFile.string());

            if (!in.is_open())
            {
                in.close();

                // We could not open the experiment file so we can only check the title
                if (mExperimentInfo->title().empty())
                {
                    std::string msg = mFileToCheck.string();
                    msg += ": Missing experiment title!";
                    console_message(msg);

                    moveFileToInvalid();

                    complete_file_progress(mID, "Data Invalid");

                    return;
                }
            }
            else
            {
                nlohmann::json jsonDoc = nlohmann::json::parse(in, nullptr, false, true);

                in.close();

                cExperimentInfoFromJson info;
                info.parse(jsonDoc);

                if (info != *mExperimentInfo)
                {
                    std::string msg = mFileToCheck.string();
                    msg += ": Missing experiment information!";
                    console_message(msg);

                    moveFileToInvalid();

                    complete_file_progress(mID, "Data Invalid");

                    return;
                }
            }
        }
    }

    complete_file_progress(mID, "Passed");
}

//-----------------------------------------------------------------------------
void cCeresDataVerifier::moveFileToInvalid()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mInvalidDirectory);

    std::filesystem::path dest = mInvalidDirectory / mFileToCheck.filename();
    std::filesystem::rename(mFileToCheck, dest);

    ++g_num_invalid_files;
}

//-----------------------------------------------------------------------------


