
#include "LidarDataVerifier.hpp"
#include "bdf_v1/ParserExceptions.hpp"
#include "bdf_v1/OusterParser.hpp"
#include "bdf_v1/PvtParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <atomic>


extern std::atomic<uint32_t> g_num_failed_files;
extern std::atomic<uint32_t> g_num_invalid_files;

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
    mInvalidDirectory = invalid_dir;
}

cLidarDataVerifier::cLidarDataVerifier(int id, std::filesystem::directory_entry file_to_check,
    std::filesystem::path invalid_dir)
    :
    mID(id)
{
    mInvalidDirectory = invalid_dir;
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
bool cLidarDataVerifier::setFileToCheck(std::filesystem::directory_entry file_to_check)
{
    mFileToCheck = file_to_check;
    return true;
}

//-----------------------------------------------------------------------------
bool cLidarDataVerifier::open(std::filesystem::path file_to_check)
{
    if (mFileReader.isOpen())
        mFileReader.close();
        
    mFileReader.open(file_to_check.string());

    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::process_file()
{
    if (open(mFileToCheck))
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
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const v1::bdf::invalid_data& e)
    {
        mFileReader.close();
        std::string msg = e.what();

        moveFileToInvalid();

        complete_file_progress(mID, "Invalid Data");

        return;
    }
    catch (const std::exception& e)
    {
        ++g_num_failed_files;

        mFileReader.close();

        complete_file_progress(mID, "Failed!");

        return;
    }

    mFileReader.close();

    complete_file_progress(mID, "Passed");
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::moveFileToInvalid()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mInvalidDirectory);

    std::filesystem::path dest = mInvalidDirectory / mFileToCheck.filename();
    std::filesystem::rename(mFileToCheck, dest);

    ++g_num_invalid_files;
}

//-----------------------------------------------------------------------------


