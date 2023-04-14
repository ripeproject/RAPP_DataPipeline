
#include "DataFileRecovery.hpp"
#include "ParserExceptions.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <stdexcept>

#include <iostream>

extern void console_message(const std::string& msg);


//-----------------------------------------------------------------------------
cDataFileRecovery::cDataFileRecovery(std::filesystem::path recovery_dir)
{
    mRecoveryDirectory = recovery_dir;
}

cDataFileRecovery::cDataFileRecovery(std::filesystem::path file_to_recover,
                                    std::filesystem::path recovery_dir)
{
    mRecoveryDirectory = recovery_dir;
    mCurrentFile = file_to_recover;

    mRecoveryFile = mRecoveryDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mRecoveryFile))
    {
        throw std::logic_error("File already exists");
    }

    mFileWriter.open(mRecoveryFile.string());

    if (!cBlockDataFileRecovery::open(file_to_recover.string()))
    {
        throw std::logic_error(mCurrentFile.string());
    }
}

cDataFileRecovery::~cDataFileRecovery()
{
    mFileWriter.close();
}

//-----------------------------------------------------------------------------
std::filesystem::path cDataFileRecovery::recoveredFileName()
{
    return mRecoveryFile;
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::open(std::filesystem::path file_to_recover)
{
    mCurrentFile = file_to_recover;
    mRecoveryFile = mRecoveryDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mRecoveryFile))
    {
        return false;
    }

    mFileWriter.open(mRecoveryFile.string());
    cBlockDataFileRecovery::open(file_to_recover.string());

    return mFileWriter.isOpen() && cBlockDataFileRecovery::isOpen();
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::run()
{
    std::string msg = "Recovering ";
    msg += mCurrentFile.string();
    msg += "...";
    console_message(msg);

    bool result = pass1();

    msg = "Verifing: ";
    msg += mRecoveryFile.string();
    msg += "...";
    console_message(msg);

    if (!pass2())
    {
        result = false;
    }

    if (result)
    {
        removeFailedFile();
    }

    return result;
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::pass1()
{
    if (!cBlockDataFileRecovery::isOpen())
    {
        throw std::logic_error("No file is open for recover.");
    }

    try
    {
        while (!eof())
        {
            if (fail())
            {
                cBlockDataFileRecovery::close();
                mFileWriter.close();
                return false;
            }

            cBlockDataFileRecovery::processBlock();
        }
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.filename().string();
        msg += ": ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.filename().string();
        msg += ": Failed due to ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = mCurrentFile.filename().string();
        msg += ": Unexpected EOF, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!eof())
        {
            std::string msg = mCurrentFile.filename().string();
            msg += ": Failed due to ";
            msg += e.what();
            console_message(msg);

            return false;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
bool cDataFileRecovery::pass2()
{
    cBlockDataFileReader fileReader;

    fileReader.open(mRecoveryFile.string());

    if (!fileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    try
    {
        while (!fileReader.eof())
        {
            if (fileReader.fail())
            {
                fileReader.close();
                return true;
            }

            fileReader.processBlock();
        }
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mRecoveryFile.filename().string();
        msg += ": ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::formatting_error& e)
    {
        std::string msg = mRecoveryFile.filename().string();
        msg += ": Formatting Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mRecoveryFile.filename().string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::io_error& e)
    {
        std::string msg = mRecoveryFile.filename().string();
        msg += ": IO Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = mRecoveryFile.filename().string();
        msg += ": Runtime Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!fileReader.eof())
        {
            std::string msg = mRecoveryFile.filename().string();
            msg += ": std::exception, ";
            msg += e.what();
            console_message(msg);

            return false;
        }
    }

    fileReader.close();

    return true;
}

//-----------------------------------------------------------------------------
void cDataFileRecovery::processBlock(const cBlockID& id)
{
//    std::cout << "Class ID = " << id.classID() << ", Major = " << static_cast<int>(id.majorVersion())
//        << ", Minor = " << static_cast<int>(id.minorVersion()) << ", Data ID = " << id.dataID() << "\n";

    mFileWriter.writeBlock(id);
}

void cDataFileRecovery::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
//    std::cout << "Class ID = " << id.classID() << ", Major = " << static_cast<int>(id.majorVersion())
//        << ", Minor = " << static_cast<int>(id.minorVersion()) << ", Data ID = " << id.dataID()
//        << ", len = " << len << "\n";

    mFileWriter.writeBlock(id, buf, len);
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::removeFailedFile()
{
    if (cBlockDataFileReader::isOpen())
        cBlockDataFileReader::close();

    if (mFileWriter.isOpen())
        mFileWriter.close();

    {
        auto src = std::filesystem::file_size(mCurrentFile);
        auto dst = std::filesystem::file_size(mRecoveryFile);
        std::size_t diff = 0;
        if (dst > src)
            diff = dst - src;
        else
            diff = src - dst;

        // The 4096 is one hard drive data block.
        if (diff > 4096)
        {
            return false;
        }
    }

    return std::filesystem::remove(mCurrentFile);
}


