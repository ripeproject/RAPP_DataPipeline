
#include "LidarDataVerifier.hpp"
#include "bdf_v1/BlockDataFile.hpp"
#include "bdf_v1/ParserExceptions.hpp"
#include "bdf_v1/OusterParser.hpp"
#include "bdf_v1/PvtParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <atomic>


extern std::atomic<uint32_t> g_num_failed_data_files;
extern std::atomic<uint32_t> g_num_invalid_data_files;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string suffix);

extern std::mutex g_invalid_dir_mutex;

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
cLidarDataVerifier::cLidarDataVerifier(int id, std::filesystem::path invalid_dir)
    :
    mID(id)
{
    mpFileReader = std::make_unique<v1::cBlockDataFileReader>();
    mInvalidDirectory = invalid_dir;
}

cLidarDataVerifier::cLidarDataVerifier(int id, std::filesystem::directory_entry file_to_check,
    std::filesystem::path invalid_dir)
    :
    mID(id)
{
    mInvalidDirectory = invalid_dir;
    mFileToCheck = file_to_check;
    mpFileReader->open(file_to_check.path().string());

    if (!mpFileReader->isOpen())
    {
        throw std::logic_error(mFileToCheck.string());
    }
}

cLidarDataVerifier::~cLidarDataVerifier()
{
    mpFileReader->close();
}

//-----------------------------------------------------------------------------
bool cLidarDataVerifier::setFileToCheck(std::filesystem::directory_entry file_to_check)
{
    mFileToCheck = file_to_check;
    return true;
}

//-----------------------------------------------------------------------------
bool cLidarDataVerifier::open(std::filesystem::path file_to_check)
{
    if (mpFileReader->isOpen())
        mpFileReader->close();
        
    mpFileReader->open(file_to_check.string());

    return mpFileReader->isOpen();
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::process_file()
{
    if (open(mFileToCheck))
    {
        mFileSize = mpFileReader->size();

        new_file_progress(mID, mFileToCheck.string());

        run();
    }
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::run()
{
    if (!mpFileReader->isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

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

        moveFileToInvalid();

        complete_file_progress(mID, "Invalid Data");

        return;
    }
    catch (const std::exception& e)
    {
        ++g_num_failed_data_files;

        mpFileReader->close();

        complete_file_progress(mID, "Failed!");

        return;
    }

    mpFileReader->close();

    complete_file_progress(mID, "Passed");
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::moveFileToInvalid()
{
    if (mpFileReader->isOpen())
        mpFileReader->close();

    ::create_directory(mInvalidDirectory);

    std::filesystem::path dest = mInvalidDirectory / mFileToCheck.filename();
    std::filesystem::rename(mFileToCheck, dest);

    ++g_num_invalid_data_files;
}

//-----------------------------------------------------------------------------


