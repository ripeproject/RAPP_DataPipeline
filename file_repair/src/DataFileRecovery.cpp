
#include "DataFileRecovery.hpp"
#include "ParserExceptions.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <stdexcept>

#include <iostream>

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);


//-----------------------------------------------------------------------------
cDataFileRecovery::cDataFileRecovery(int id, std::filesystem::path temporary_dir)
    : mID(id)
{
    mTemporaryDirectory = temporary_dir;
}

cDataFileRecovery::cDataFileRecovery(int id, std::filesystem::path file_to_recover,
                                    std::filesystem::path temporary_dir)
    : mID(id)
{
    mTemporaryDirectory = temporary_dir;
    mCurrentFile = file_to_recover;

    mTemporaryFile = mTemporaryDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mTemporaryFile))
    {
        throw std::logic_error("File already exists");
    }

    mFileWriter.open(mTemporaryFile.string());

    if (!cBlockDataFileRecovery::open(file_to_recover.string()))
    {
        throw std::logic_error(mCurrentFile.string());
    }

    mOriginalFileSize = cBlockDataFileRecovery::file_size();
}

cDataFileRecovery::~cDataFileRecovery()
{
    mFileWriter.close();
}

//-----------------------------------------------------------------------------
std::filesystem::path cDataFileRecovery::tempFileName()
{
    return mTemporaryFile;
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::open(std::filesystem::path file_to_recover)
{
    mCurrentFile = file_to_recover;
    mTemporaryFile = mTemporaryDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mTemporaryFile))
    {
        return false;
    }

    mFileWriter.open(mTemporaryFile.string());
    cBlockDataFileRecovery::open(file_to_recover.string());

    mOriginalFileSize = cBlockDataFileRecovery::file_size();

    return mFileWriter.isOpen() && cBlockDataFileRecovery::isOpen();
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::is_open() const
{
    return mFileWriter.isOpen() && cBlockDataFileRecovery::isOpen();
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::run()
{
    update_prefix_progress(mID, "Recovering", 0);

    bool result = pass1();

    mFileWriter.close();

    update_prefix_progress(mID, "Verifing", 0);

    if (!pass2())
    {
        result = false;
    }

    if (result)
    {
        auto diff = mRecoveredFileSize - mOriginalFileSize;
        result = (mRecoveredFileSize >= mOriginalFileSize);
//        removeFailedFile();
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

            auto file_pos = static_cast<double>(cBlockDataFileRecovery::filePosition());
            file_pos = 100.0 * (file_pos / mOriginalFileSize);
            update_progress(mID, static_cast<int>(file_pos));
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

    fileReader.open(mTemporaryFile.string());

    if (!fileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    mRecoveredFileSize = fileReader.file_size();
    auto file_pos = fileReader.filePosition();
    auto block_size = fileReader.block_size();

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

            block_size = fileReader.block_size();

            file_pos = fileReader.filePosition();
            auto file_pct = static_cast<double>(file_pos);
            file_pct = 100.0 * (file_pct / mRecoveredFileSize);
            update_progress(mID, static_cast<int>(file_pct));
        }
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::formatting_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Formatting Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::io_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": IO Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Runtime Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!fileReader.eof())
        {
            std::string msg = mTemporaryFile.filename().string();
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
        auto dst = std::filesystem::file_size(mTemporaryFile);
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

//    std::filesystem::copy_file();
    return std::filesystem::remove(mCurrentFile);
}


