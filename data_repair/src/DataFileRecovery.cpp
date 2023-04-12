
#include "DataFileRecovery.hpp"
#include "ParserExceptions.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <stdexcept>


extern void console_message(const std::string& msg);

namespace
{
    void create_directory(std::filesystem::path recovered_dir)
    {
        if (!std::filesystem::exists(recovered_dir))
        {
            std::filesystem::create_directory(recovered_dir);
        }
    }
}


//-----------------------------------------------------------------------------
cDataFileRecovery::cDataFileRecovery(std::filesystem::path repaired_dir)
{
    mRepairedDirectory = repaired_dir;
}

cDataFileRecovery::cDataFileRecovery(std::filesystem::directory_entry file_to_repair,
    std::filesystem::path repaired_dir)
{
    mRepairedDirectory = repaired_dir;
    mCurrentFile = file_to_repair;

    mRepairedFile = mRepairedDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mRepairedFile))
    {
        throw std::logic_error("File already exists");
    }

    mFileWriter.open(mRepairedFile.string());

    if (!cBlockDataFileRepair::open(file_to_repair.path().string()))
    {
        throw std::logic_error(mCurrentFile.string());
    }
}

cDataFileRecovery::~cDataFileRecovery()
{
    mFileWriter.close();
}


//-----------------------------------------------------------------------------
bool cDataFileRecovery::open(std::filesystem::directory_entry file_to_repair)
{
    mCurrentFile = file_to_repair;
    mRepairedFile = mRepairedDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mRepairedFile))
    {
        return false;
    }

    mFileWriter.open(mRepairedFile.string());
    cBlockDataFileRepair::open(file_to_repair.path().string());

    return mFileWriter.isOpen() && cBlockDataFileRepair::isOpen();
}

//-----------------------------------------------------------------------------
void cDataFileRecovery::process_file(std::filesystem::directory_entry file_to_repair)
{
    if (open(file_to_repair))
    {
        std::string msg = "Processing ";
        msg += file_to_repair.path().string();
        msg += "...";
        console_message(msg);

        run();
    }
}

//-----------------------------------------------------------------------------
void cDataFileRecovery::run()
{
    if (!cBlockDataFileRepair::isOpen())
    {
        throw std::logic_error("No file is open for repair.");
    }

    try
    {
        while (!eof())
        {
            if (fail())
            {
//                emit fileResults(mId, false, "I/O Error: failbit is set.");
                cBlockDataFileRepair::close();
                mFileWriter.close();
                return;
            }

            cBlockDataFileRepair::processBlock();
        }
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Failed due to ";
        msg += e.what();
        console_message(msg);

//        emit fileResults(mId, false, e.what());
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": CRC Error, ";
        msg += e.what();
        console_message(msg);

        moveFileToRepaired(false);
        //        emit fileResults(mId, false, e.what());
        return;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Unexpected EOF, ";
        msg += e.what();
        console_message(msg);

        moveFileToRepaired(false);

//        emit fileResults(mId, true, msg);
        return;
    }
    catch (const std::exception& e)
    {
        if (eof())
        {
            moveFileToRepaired();
            /*
            if (!moveFileToRepaired())
                emit fileResults(mId, false, "File size mismatch!");
            else
                emit fileResults(mId, true, QString());
            */
        }
        else
        {
            std::string msg = mCurrentFile.string();
            msg += ": Failed due to ";
            msg += e.what();
            console_message(msg);

            //            emit fileResults(mId, false, e.what());
        }
        return;
    }

    moveFileToRepaired();
/*
    if (!moveFileToRepaired())
        emit fileResults(mId, false, "File size mismatch!");
    else
        emit fileResults(mId, true, QString());
*/
}

void cDataFileRecovery::processBlock(const cBlockID& id)
{
    mFileWriter.writeBlock(id);
}

void cDataFileRecovery::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
    mFileWriter.writeBlock(id, buf, len);
}

//-----------------------------------------------------------------------------
bool cDataFileRecovery::moveFileToRepaired(bool size_check)
{
    if (cBlockDataFileReader::isOpen())
        cBlockDataFileReader::close();

    if (mFileWriter.isOpen())
        mFileWriter.close();

    if (size_check)
    {
        auto src = std::filesystem::file_size(mCurrentFile);
        auto dst = std::filesystem::file_size(mRepairedFile);
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

    auto failedDirectory = mCurrentFile.parent_path();

    if (!std::filesystem::exists(failedDirectory))
        return false;

    // Move the failed data file to a "recovered" directory
    // to signal that the file was fully repaired!
    {
        auto recoveredDirectory = failedDirectory / "recovered";

        ::create_directory(recoveredDirectory);

        std::filesystem::path dest = recoveredDirectory / mCurrentFile.filename();
        std::filesystem::rename(mCurrentFile, dest);
    }

    // Move the fully repaired data file back into the main data
    // directory.
    {
        auto dataDirectory = mRepairedDirectory.parent_path();

        if (!std::filesystem::exists(dataDirectory))
            return false;

        std::filesystem::path dest = dataDirectory / mRepairedFile.filename();

        if (std::filesystem::exists(dest))
            return false;

        std::filesystem::rename(mRepairedFile, dest);
    }

    return true;
}


