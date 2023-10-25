
#include "CeresFileVerifier.hpp"
#include "ParserExceptions.hpp"

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

std::mutex g_failed_dir_mutex;


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
cCeresFileVerifier::cCeresFileVerifier(int id, std::filesystem::path failed_dir)
:
    mID(id)
{
    mFailedDirectory = failed_dir;
}

cCeresFileVerifier::cCeresFileVerifier(int id, std::filesystem::directory_entry file_to_check,
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

cCeresFileVerifier::~cCeresFileVerifier()
{
    mFileReader.close();
}

//-----------------------------------------------------------------------------
bool cCeresFileVerifier::setFileToCheck(std::filesystem::directory_entry file_to_check)
{
    mFileToCheck = file_to_check;
    return true;
}

//-----------------------------------------------------------------------------
bool cCeresFileVerifier::open(std::filesystem::path file_to_check)
{
    if (mFileReader.isOpen())
        mFileReader.close();
        
    mFileReader.open(file_to_check.string());
    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cCeresFileVerifier::process_file()
{
    if (open(mFileToCheck))
    {
        mFileSize = mFileReader.file_size();

        new_file_progress(mID, mFileToCheck.string());

        run();
    }
}

//-----------------------------------------------------------------------------
void cCeresFileVerifier::run()
{
    if (!pass1())
    {
        mFileReader.close();
        moveFileToFailed();
        return;
    }

    mFileReader.open(mFileToCheck.string());

    mFileSize = mFileReader.file_size();

    if (!pass2())
    {
        mFileReader.close();
        moveFileToFailed();
        return;
    }

    mFileReader.close();

    complete_file_progress(mID, "Complete", "passed");
}

//-----------------------------------------------------------------------------
bool cCeresFileVerifier::pass1()
{
    update_prefix_progress(mID, "Pass 1", 0);

    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                return true;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": CRC Error, ";
        msg += " class id = ";
        msg += std::to_string(e.class_id);
        msg += ", major version = ";
        msg += std::to_string(e.major_version);
        msg += ", minor version = ";
        msg += std::to_string(e.minor_version);
        msg += ", data id = ";
        msg += std::to_string(e.data_id);
        msg += ", ";

        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::formatting_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Formatting Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::io_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": IO Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Runtime Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mFileToCheck.string();
            msg += ": std::exception, ";
            msg += e.what();
            console_message(msg);

            moveFileToFailed();

            return false;
        }
    }

    mFileReader.close();

    return true;
}

//-----------------------------------------------------------------------------
bool cCeresFileVerifier::pass2()
{
//    std::string msg = "Pass2: ";
//    msg += mFileToCheck.string();
//    msg += "...";
//    console_message(msg);
    update_prefix_progress(mID, "Pass 2", 0);


    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

/*
    auto ouster = std::make_unique<cOusterVerificationParser>();
    auto axis = std::make_unique<cAxisCommunicationsVerificationParser>();

    mFileReader.attach(ouster.get());
    mFileReader.attach(axis.get());
*/

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                return false;
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

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mFileToCheck.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mFileToCheck.string();
            msg += ":  std::exception, ";
            msg += e.what();
            console_message(msg);

            return false;
        }
    }

    mFileReader.close();

    return true;
}

//-----------------------------------------------------------------------------
void cCeresFileVerifier::moveFileToFailed()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mFileToCheck.filename();

    complete_file_progress(mID, "Complete", "failed");

    std::string msg = "Moving ";
    msg += mFileToCheck.string();
    msg += " to ";
    msg += dest.string();
    console_message(msg);

    std::filesystem::rename(mFileToCheck, dest);

    ++g_num_failed_files;
}

//-----------------------------------------------------------------------------


