
#include "DataRepair.hpp"
#include "ParserExceptions.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <stdexcept>


extern void console_message(const std::string& msg);


//-----------------------------------------------------------------------------
cDataRepair::cDataRepair(std::filesystem::path repaired_dir)
{
    mRepairedDirectory = repaired_dir;

    mOusterRepairParser = std::make_unique<cOusterRepairParser>();
}

cDataRepair::cDataRepair(std::filesystem::path file_to_repair,
                        std::filesystem::path repaired_dir)
    : cDataRepair(repaired_dir)
{
    mCurrentFile = file_to_repair;

    mRepairedFile = mRepairedDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mRepairedFile))
    {
        throw std::logic_error("File already exists");
    }

    mFileReader.open(mCurrentFile.string());
    mFileWriter.open(mRepairedFile.string());

    if (!mFileReader.isOpen() || !mFileWriter.isOpen())
    {
        throw std::logic_error(mCurrentFile.string());
    }
}

cDataRepair::~cDataRepair()
{
    mFileReader.close();
    mFileWriter.close();
}


//-----------------------------------------------------------------------------
bool cDataRepair::open(std::filesystem::path file_to_repair)
{
    mCurrentFile = file_to_repair;
    mRepairedFile = mRepairedDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mRepairedFile))
    {
        return false;
    }

    mFileWriter.open(mRepairedFile.string());
    mFileReader.open(mCurrentFile.string());

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
void cDataRepair::run()
{
    mFileReader.registerCallback([this](const cBlockID& id) { this->processBlock(id); });
    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len) { this->processBlock(id, buf, len); });

    mFileReader.attach(mOusterRepairParser.get());
    mOusterRepairParser->attach(&mFileWriter);

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                mFileWriter.close();
                return;
            }

            mFileReader.processBlock();
        }
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Failed due to ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": CRC Error, ";
        msg += e.what();
        console_message(msg);

//        moveFileToRepaired(false);
        return;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Unexpected EOF, ";
        msg += e.what();
        console_message(msg);

//        moveFileToRepaired(false);
        return;
    }
    catch (const std::exception& e)
    {
        if (mFileReader.eof())
        {
//            moveFileToRepaired();
        }
        else
        {
            std::string msg = mCurrentFile.string();
            msg += ": Failed due to ";
            msg += e.what();
            console_message(msg);
        }
        return;
    }

//    moveFileToRepaired();

    mFileReader.close();
    mFileWriter.close();
}

void cDataRepair::processBlock(const cBlockID& id)
{
    mFileWriter.writeBlock(id);
}

void cDataRepair::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
    mFileWriter.writeBlock(id, buf, len);
}

//-----------------------------------------------------------------------------
bool cDataRepair::moveFile(bool size_check)
{
/*
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
*/
    return true;
}


