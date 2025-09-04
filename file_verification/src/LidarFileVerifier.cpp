
#include "LidarFileVerifier.hpp"

#include "bdf_v1/BlockDataFile.hpp"
#include "bdf_v1/ParserExceptions.hpp"
#include "bdf_v1/OusterParser.hpp"
#include "bdf_v1/PvtParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <atomic>


namespace ceres_file_verifier
{
    extern std::atomic<uint32_t> g_num_failed_files;

    extern std::mutex g_failed_dir_mutex;
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
        using namespace ceres_file_verifier;

        std::lock_guard<std::mutex> guard(g_failed_dir_mutex);

        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}

//-----------------------------------------------------------------------------
cLidarFileVerifier::cLidarFileVerifier(int id, std::filesystem::path failed_dir)
    :
    mID(id)
{
    mpFileReader = std::make_unique<v1::cBlockDataFileReader>();
    mFailedDirectory = failed_dir;
}

cLidarFileVerifier::cLidarFileVerifier(int id, std::filesystem::directory_entry file_to_check,
    std::filesystem::path failed_dir)
    :
    mID(id)
{
    mFailedDirectory = failed_dir;
    mFileToCheck = file_to_check;
    mpFileReader->open(file_to_check.path().string());

    if (!mpFileReader->isOpen())
    {
        throw std::logic_error(mFileToCheck.string());
    }
}

cLidarFileVerifier::~cLidarFileVerifier()
{
    mpFileReader->close();
}

//-----------------------------------------------------------------------------
bool cLidarFileVerifier::setFileToCheck(std::filesystem::directory_entry file_to_check)
{
    mFileToCheck = file_to_check;
    return true;
}

//-----------------------------------------------------------------------------
bool cLidarFileVerifier::open(std::filesystem::path file_to_check)
{
    if (mpFileReader->isOpen())
        mpFileReader->close();
        
    mpFileReader->open(file_to_check.string());

    return mpFileReader->isOpen();
}

//-----------------------------------------------------------------------------
void cLidarFileVerifier::process_file()
{
    if (open(mFileToCheck))
    {
        new_file_progress(mID, mFileToCheck.string());

        auto result = run();

        if (result == eRETURN_TYPE::PASSED)
            complete_file_progress(mID, "Complete", "Passed");
        else
            complete_file_progress(mID, "Complete", "Failed");
    }
}

//-----------------------------------------------------------------------------
cLidarFileVerifier::eRETURN_TYPE cLidarFileVerifier::run()
{
    if (!mpFileReader->isOpen())
    {
        return eRETURN_TYPE::COULD_NOT_OPEN_FILE;
    }

    mFileSize = mpFileReader->size();

    update_prefix_progress(mID, "Checking", 0);

    auto ouster = std::make_unique<v1::cOusterParser>();
    auto pvt = std::make_unique<v1::cPvtParser>();

    mpFileReader->attach(ouster.get());
    mpFileReader->attach(pvt.get());

    try
    {
        while (!mpFileReader->eof())
        {
            if (mpFileReader->fail())
            {
                mpFileReader->close();
                return eRETURN_TYPE::INVALID_FILE;
            }

            mpFileReader->processBlock();

            auto file_pos = static_cast<double>(mpFileReader->filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const v1::bdf::invalid_data& e)
    {
        mpFileReader->close();
        std::string msg = e.what();

        if (!moveFileToFailed())
            return eRETURN_TYPE::ABORT;

        return eRETURN_TYPE::INVALID_FILE;
    }
    catch (const std::exception& e)
    {
        if (!mpFileReader->eof())
        {
            if (!moveFileToFailed())
                return eRETURN_TYPE::ABORT;
        }
        return eRETURN_TYPE::INVALID_FILE;
    }

    mpFileReader->close();

    return eRETURN_TYPE::PASSED;
}

//-----------------------------------------------------------------------------
bool cLidarFileVerifier::moveFileToFailed()
{
    using namespace ceres_file_verifier;

    if (mpFileReader->isOpen())
        mpFileReader->close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mFileToCheck.filename();

    try
    {
        std::filesystem::rename(mFileToCheck, dest);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::string msg = "Move File To Failed: ";
        msg += e.what();
        console_message(msg);
        return false;
    }
    catch (const std::bad_alloc& e)
    {
        std::string msg = "Move File To Failed: ";
        msg += e.what();
        console_message(msg);
        return false;
    }

    ++g_num_failed_files;

    return true;
}

//-----------------------------------------------------------------------------


