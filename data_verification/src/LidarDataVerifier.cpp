
#include "LidarDataVerifier.hpp"
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
extern void update_file_progress(const int id, std::string filename, const int progress_pct);
extern void update_file_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string filename, std::string suffix);

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
cLidarDataVerifier::cLidarDataVerifier(int id, std::filesystem::path failed_dir)
    :
    mID(id)
{
    mFailedDirectory = failed_dir;
}

cLidarDataVerifier::cLidarDataVerifier(int id, std::filesystem::directory_entry file_to_check,
    std::filesystem::path failed_dir)
    :
    mID(id)
{
    mFailedDirectory = failed_dir;
    mFileToCheck = file_to_check;
    mFileReader.open(file_to_check.path().string());

    if (!mFileReader.isOpen())
    {
        throw std::logic_error(mFileToCheck.string());
    }
}

cLidarDataVerifier::~cLidarDataVerifier()
{
    mFileReader.close();
}

//-----------------------------------------------------------------------------
bool cLidarDataVerifier::open(std::filesystem::directory_entry file_to_check)
{
    if (mFileReader.isOpen())
        mFileReader.close();
        
    mFileToCheck = file_to_check;
    mFileReader.open(file_to_check.path().string());

    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::process_file(std::filesystem::directory_entry file_to_check)
{
    if (open(file_to_check))
    {
        mFileSize = mFileReader.size();

        new_file_progress(mID, mFileToCheck.string());

        run();
    }
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::run()
{
    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    auto ouster = std::make_unique<v1::cOusterParser>();
    auto pvt = std::make_unique<v1::cPvtParser>();

    mFileReader.attach(ouster.get());
    mFileReader.attach(pvt.get());

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_file_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const v1::bdf::invalid_data& e)
    {
        mFileReader.close();
        std::string msg = e.what();

        moveFileToFailed();
        return;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            moveFileToFailed();
        }
        return;
    }

    mFileReader.close();

    complete_file_progress(mID, mFileToCheck.string(), "passed");
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::moveFileToFailed()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mFileToCheck.filename();
    std::filesystem::rename(mFileToCheck, dest);

    complete_file_progress(mID, mFileToCheck.string(), "failed");

    ++g_num_failed_files;
}

//-----------------------------------------------------------------------------


