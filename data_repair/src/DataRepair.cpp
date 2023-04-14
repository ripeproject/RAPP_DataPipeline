
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
    std::string msg = "Repairing data in ";
    msg += mCurrentFile.string();
    msg += "...";
    console_message(msg);

    mFileReader.registerCallback([this](const cBlockID& id) { this->processBlock(id); });
    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len) { this->processBlock(id, buf, len); });

    mFileReader.attach(mOusterRepairParser.get());
    mOusterRepairParser->attach(&mFileWriter);

    bool result = pass1();

    mFileReader.close();
    mFileWriter.close();

    if (result)
    {
        removeRecoveryFile();
    }

    msg = "Verifing: ";
    msg += mRepairedFile.string();
    msg += "...";
    console_message(msg);

    if (pass2())
    {
        moveRepairedFile();
    }
}

//-----------------------------------------------------------------------------
bool cDataRepair::pass1()
{
    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                mFileWriter.close();
                return false;
            }

            mFileReader.processBlock();
        }
    }
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Invalid data, ";
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
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": CRC Error, ";
        msg += e.what();
        console_message(msg);
        return false;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Unexpected EOF, ";
        msg += e.what();
        console_message(msg);
        return false;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mCurrentFile.string();
            msg += ": Failed due to ";
            msg += e.what();
            console_message(msg);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
bool cDataRepair::pass2()
{
    cBlockDataFileReader fileReader;

    fileReader.open(mRepairedFile.string());

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
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mRepairedFile.filename().string();
        msg += ": Invalid Data, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mRepairedFile.filename().string();
        msg += ": ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::formatting_error& e)
    {
        std::string msg = mRepairedFile.filename().string();
        msg += ": Formatting Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mRepairedFile.filename().string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const bdf::io_error& e)
    {
        std::string msg = mRepairedFile.filename().string();
        msg += ": IO Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = mRepairedFile.filename().string();
        msg += ": Runtime Error, ";
        msg += e.what();
        console_message(msg);

        return false;
    }
    catch (const std::exception& e)
    {
        if (!fileReader.eof())
        {
            std::string msg = mRepairedFile.filename().string();
            msg += ": std::exception, ";
            msg += e.what();
            console_message(msg);

            return false;
        }
    }

    fileReader.close();

    return true;
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
bool cDataRepair::removeRecoveryFile()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    if (mFileWriter.isOpen())
        mFileWriter.close();

    return std::filesystem::remove(mCurrentFile);
}

//-----------------------------------------------------------------------------
bool cDataRepair::moveRepairedFile()
{
    // Move the fully repaired data file back into the main data
    // directory.
    auto dataDirectory = mRepairedDirectory.parent_path();

    if (!std::filesystem::exists(dataDirectory))
        return false;

    std::filesystem::path dest = dataDirectory / mRepairedFile.filename();

    if (std::filesystem::exists(dest))
        return false;

    std::filesystem::rename(mRepairedFile, dest);

    return true;
}


