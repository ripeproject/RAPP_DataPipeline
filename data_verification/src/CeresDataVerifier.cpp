
#include "CeresDataVerifier.hpp"
#include "ParserExceptions.hpp"
#include "OusterVerificationParser.hpp"
#include "AxisCommunicationsVerificationParser.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>


extern void console_message(const std::string& msg);

namespace
{
    void create_directory(std::filesystem::path failed_dir)
    {
        if (!std::filesystem::exists(failed_dir))
        {
            std::filesystem::create_directory(failed_dir);
        }
    }
}

//-----------------------------------------------------------------------------
cCeresDataVerifier::cCeresDataVerifier(std::filesystem::path failed_dir)
{
    mFailedDirectory = failed_dir;
}

cCeresDataVerifier::cCeresDataVerifier(std::filesystem::directory_entry file_to_check,
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

cCeresDataVerifier::~cCeresDataVerifier()
{
    mFileReader.close();
}

//-----------------------------------------------------------------------------
bool cCeresDataVerifier::open(std::filesystem::directory_entry file_to_check)
{
    if (mFileReader.isOpen())
        mFileReader.close();
        
    mCurrentFile = file_to_check;
    mFileReader.open(file_to_check.path().string());

    return mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cCeresDataVerifier::process_file(std::filesystem::directory_entry file_to_check)
{
    if (open(file_to_check))
    {
        run();
    }
}

//-----------------------------------------------------------------------------
void cCeresDataVerifier::run()
{
    if (!pass1())
    {
        mFileReader.close();
        moveFileToFailed();
        return;
    }

    mFileReader.open(mCurrentFile.string());

    if (!pass2())
    {
        mFileReader.close();
        moveFileToFailed();
        return;
    }

    mFileReader.close();
}

//-----------------------------------------------------------------------------
bool cCeresDataVerifier::pass1()
{
    std::string msg = "Pass1: ";
    msg += mCurrentFile.string();
    msg += "...";
    console_message(msg);

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
        }
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.string();
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
        std::string msg = mCurrentFile.string();
        msg += ": Formatting Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::io_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": IO Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Runtime Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mCurrentFile.string();
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
bool cCeresDataVerifier::pass2()
{
    std::string msg = "Pass2: ";
    msg += mCurrentFile.string();
    msg += "...";
    console_message(msg);

    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    auto ouster = std::make_unique<cOusterVerificationParser>();
    auto axis = std::make_unique<cAxisCommunicationsVerificationParser>();

    mFileReader.attach(ouster.get());
    mFileReader.attach(axis.get());

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
        }
    }
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Invalid Data, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mCurrentFile.string();
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
void cCeresDataVerifier::moveFileToFailed()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    ::create_directory(mFailedDirectory);

    std::filesystem::path dest = mFailedDirectory / mCurrentFile.filename();

    std::string msg = "Moving ";
    msg += mCurrentFile.string();
    msg += " to ";
    msg += dest.string();
    console_message(msg);

    std::filesystem::rename(mCurrentFile, dest);
}

//-----------------------------------------------------------------------------


