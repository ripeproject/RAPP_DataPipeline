
#include "LidarDataVerifier.hpp"
#include "bdf_v1/ParserExceptions.hpp"
#include "bdf_v1/OusterParser.hpp"
#include "bdf_v1/PvtParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>


extern void console_message(const std::string& msg);
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
cLidarDataVerifier::cLidarDataVerifier(std::filesystem::path failed_dir)
{
    mFailedDirectory = failed_dir;
}

cLidarDataVerifier::cLidarDataVerifier(std::filesystem::directory_entry file_to_check,
    std::filesystem::path failed_dir)
{
    mFailedDirectory = failed_dir;
    mCurrentFile = file_to_check;
    mFileReader.open(file_to_check.path().string());

    if (!mFileReader.isOpen())
    {
        throw std::logic_error(mCurrentFile.string());
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
        
    mCurrentFile = file_to_check;
    mFileReader.open(file_to_check.path().string());

    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::process_file(std::filesystem::directory_entry file_to_check)
{
    if (open(file_to_check))
    {
        std::string msg = "Processing ";
        msg += file_to_check.path().string();
        msg += "...";
        console_message(msg);

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
}

//-----------------------------------------------------------------------------
void cLidarDataVerifier::moveFileToFailed()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mCurrentFile.filename();
    std::filesystem::rename(mCurrentFile, dest);
}

//-----------------------------------------------------------------------------


