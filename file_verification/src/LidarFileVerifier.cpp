
#include "LidarFileVerifier.hpp"

#include "bdf_v1/BlockDataFile.hpp"
#include "bdf_v1/ParserExceptions.hpp"
#include "bdf_v1/OusterParser.hpp"
#include "bdf_v1/PvtParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <atomic>


extern std::atomic<uint32_t> g_num_failed_files;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);

extern std::mutex g_failed_dir_mutex;

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
        mFileSize = mpFileReader->size();

        new_file_progress(mID, mFileToCheck.string());

        run();
    }
}

//-----------------------------------------------------------------------------
void cLidarFileVerifier::run()
{
    if (!mpFileReader->isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

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
                return;
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

        moveFileToFailed();
        return;
    }
    catch (const std::exception& e)
    {
        if (!mpFileReader->eof())
        {
            moveFileToFailed();
        }
        return;
    }

    mpFileReader->close();

    complete_file_progress(mID, "Complete", "passed");
}

//-----------------------------------------------------------------------------
void cLidarFileVerifier::moveFileToFailed()
{
    if (mpFileReader->isOpen())
        mpFileReader->close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mFileToCheck.filename();
    std::filesystem::rename(mFileToCheck, dest);

    complete_file_progress(mID, "Complete", "failed");

    ++g_num_failed_files;
}

//-----------------------------------------------------------------------------


