
#include "CeresDataVerifier.hpp"
#include "ParserExceptions.hpp"
#include "SsnxVerificationParser.hpp"
#include "OusterVerificationParser.hpp"
#include "AxisCommunicationsVerificationParser.hpp"
#include "LucidTritonVerificationParser.hpp"
#include "HySpexVNIR_3000N_VerificationParser.hpp"
#include "HySpexSWIR_384_VerificationParser.hpp"
#include "TeledyneAtlasFlirVerificationParser.hpp"

#include "ExperimentInfoFromJson.hpp"

#include <cbdf/ExperimentInfoLoader.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>

#include <string>
#include <atomic>


namespace ceres_data_verifier
{
    std::atomic<uint32_t> g_num_failed_files = 0;
    std::atomic<uint32_t> g_num_invalid_files = 0;
    std::atomic<uint32_t> g_num_missing_data = 0;

    std::mutex g_invalid_dir_mutex;
}

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string suffix);


namespace
{
    void create_directory(std::filesystem::path invalid_dir)
    {
        using namespace ceres_data_verifier;
            
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

        auto result = run();

        switch (result)
        {
        case eRETURN_TYPE::PASSED:
        {
            complete_file_progress(mID, "Passed");
            break;
        }
        case eRETURN_TYPE::INVALID_DATA:
        {
            complete_file_progress(mID, "Data Invalid");
            break;
        }
        case eRETURN_TYPE::INVALID_FILE:
        {
            complete_file_progress(mID, "Failed!");
            break;
        }
        case eRETURN_TYPE::WARNING_MISSING_DATA:
        {
            complete_file_progress(mID, "Warning Missing Data");
            break;
        }
        case eRETURN_TYPE::COULD_NOT_OPEN_FILE:
            break;
        }
    }
}

//-----------------------------------------------------------------------------
cCeresDataVerifier::eRETURN_TYPE cCeresDataVerifier::run()
{
    using namespace ceres_data_verifier;
    using namespace cdv;

    if (!mFileReader.isOpen())
    {
        return eRETURN_TYPE::COULD_NOT_OPEN_FILE;
    }

    auto info = std::make_unique<cExperimentInfoLoader>(mExperimentInfo);
    auto gps = std::make_unique<cSsnxVerificationParser>();
    auto ouster = std::make_unique<cOusterVerificationParser>();
    auto axis = std::make_unique<cAxisCommunicationsVerificationParser>();
    auto triton = std::make_unique<cLucidTritonVerificationParser>();
    auto vnir = std::make_unique<cHySpexVNIR_3000N_VerificationParser>();
    auto swir = std::make_unique<cHySpexSWIR_384_VerificationParser>();
    auto flir = std::make_unique<cTeledyneAtlasFlirVerificationParser>();


    mFileReader.attach(info.get());
    mFileReader.attach(gps.get());
    mFileReader.attach(ouster.get());
    mFileReader.attach(axis.get());
    mFileReader.attach(triton.get());
    mFileReader.attach(vnir.get());
    mFileReader.attach(swir.get());
    mFileReader.attach(flir.get());


    mFileSize = mFileReader.file_size();

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                ++g_num_failed_files;

                return eRETURN_TYPE::INVALID_FILE;
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

        return eRETURN_TYPE::INVALID_DATA;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();
        ++g_num_failed_files;

        return eRETURN_TYPE::INVALID_FILE;
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

            return eRETURN_TYPE::INVALID_FILE;
        }
    }

    if (gps->sensorPresent)
    {
        if (gps->mNumPosition == 0)
        {
            mMessage = "Missing GPS position data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }
    }

    if (ouster->sensorPresent)
    {
        if (ouster->mNumLidarDataFrames == 0)
        {
            mMessage = "Missing LiDAR data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }
    }

    if (axis->sensorPresent)
    {
        if (axis->mNumImages == 0)
        {
            mMessage = "Missing Axis Communications data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }
    }

    if (triton->sensorPresent)
    {
        if (triton->mNumImages == 0)
        {
            mMessage = "Missing Lucid Vision Labs Triton image data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }
    }

    if (vnir->sensorPresent)
    {
        if (vnir->mNumImages == 0)
        {
            mMessage = "Missing HySpex VNIR 3000N image data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }
    }

    if (swir->sensorPresent)
    {
        if (swir->mNumImages == 0)
        {
            mMessage = "Missing HySpex SWIR 384 image data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }
    }

    if (flir->sensorPresent)
    {
        if (flir->mNumImages == 0)
        {
            mMessage = "Missing Teledyne Atlas FLIR image data!";

            std::string msg = mFileToCheck.string();
            msg += ": ";
            msg += mMessage;

            console_message(msg);

            return eRETURN_TYPE::WARNING_MISSING_DATA;
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

            return eRETURN_TYPE::WARNING_MISSING_DATA;
        }

        return eRETURN_TYPE::PASSED;
    }
    else
    {
        if (mExperimentFile.empty())
        {
            if (mExperimentInfo->experimentTitle().empty())
            {
                std::string msg = mFileToCheck.string();
                msg += ": Missing experiment title!";
                console_message(msg);

                moveFileToInvalid();

                return eRETURN_TYPE::INVALID_DATA;
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
                if (mExperimentInfo->experimentTitle().empty())
                {
                    std::string msg = mFileToCheck.string();
                    msg += ": Missing experiment title!";
                    console_message(msg);

                    moveFileToInvalid();

                    return eRETURN_TYPE::INVALID_DATA;
                }
            }
            else
            {
                try
                {
                    cExperimentInfoFromJson info;

                    nlohmann::json jsonDoc = nlohmann::json::parse(in, nullptr, false, true);
                    info.parse(jsonDoc);

                    in.close();

                    if (info != *mExperimentInfo)
                    {
                        std::string msg = mFileToCheck.string();
                        msg += ": Missing experiment information!";
                        console_message(msg);

                        moveFileToInvalid();

                        return eRETURN_TYPE::INVALID_DATA;
                    }
                }
                catch (const std::exception& e)
                {
                    if (mExperimentInfo->experimentTitle().empty())
                    {
                        std::string msg = mFileToCheck.string();
                        msg += ": Missing experiment title!";
                        console_message(msg);

                        moveFileToInvalid();

                        return eRETURN_TYPE::INVALID_DATA;
                    }
                }
            }
        }
    }

    return eRETURN_TYPE::PASSED;
}

//-----------------------------------------------------------------------------
void cCeresDataVerifier::moveFileToInvalid()
{
    using namespace ceres_data_verifier;

    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mInvalidDirectory);

    std::filesystem::path dest = mInvalidDirectory / mFileToCheck.filename();
    std::filesystem::rename(mFileToCheck, dest);

    ++g_num_invalid_files;
}

//-----------------------------------------------------------------------------


