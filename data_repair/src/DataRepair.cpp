
#include "DataRepair.hpp"
#include "ParserExceptions.hpp"

#include "ExperimentInfoFromJson.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <memory>
#include <string>
#include <stdexcept>


extern std::atomic<uint32_t> g_num_failed_files;
extern std::atomic<uint32_t> g_num_repaired_files;

extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id, std::string prefix, std::string suffix);


//-----------------------------------------------------------------------------
cDataRepair::cDataRepair(int id, std::filesystem::path temporary_dir, std::filesystem::path exp_file)
    : mID(id)
{
    mTemporaryDirectory = temporary_dir;
    mExperimentFile = exp_file;

    mExperimentInfoRepairParser = std::make_unique<cExperimentInfoRepairParser>();
    mOusterRepairParser = std::make_unique<cOusterRepairParser>();
}

cDataRepair::cDataRepair(int id, std::filesystem::path file_to_repair,
                        std::filesystem::path temporary_dir, std::filesystem::path exp_file)
    : cDataRepair(id, temporary_dir, exp_file)
{
    mCurrentFile = file_to_repair;

    mTemporaryFile = mTemporaryDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mTemporaryFile))
    {
        throw std::logic_error("File already exists");
    }

    mFileReader.open(mCurrentFile.string());
    mFileWriter.open(mTemporaryFile.string());

    mFileSize = mFileReader.file_size();

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
std::filesystem::path cDataRepair::tempFileName()
{
    return mTemporaryFile;
}

//-----------------------------------------------------------------------------
bool cDataRepair::open(std::filesystem::path file_to_repair)
{
    mCurrentFile = file_to_repair;
    mTemporaryFile = mTemporaryDirectory / mCurrentFile.filename();

    if (std::filesystem::exists(mTemporaryFile))
    {
        return false;
    }

    mFileWriter.open(mTemporaryFile.string());
    mFileReader.open(mCurrentFile.string());

    mFileSize = mFileReader.file_size();

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

//-----------------------------------------------------------------------------
cDataRepair::eResult cDataRepair::pass1()
{
    cBlockDataFileReader fileReader;
    fileReader.open(mCurrentFile.string());

    if (!fileReader.isOpen())
    {
        return eResult::INVALID_FILE;
    }

    update_prefix_progress(mID, "Checking Info Block", 0);

    fileReader.attach(mExperimentInfoRepairParser.get());

    try
    {
        while (!fileReader.eof())
        {
            if (fileReader.fail())
            {
                fileReader.close();
                return eResult::INVALID_FILE;
            }

            fileReader.processBlock();

            auto file_pos = static_cast<double>(fileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Invalid data, ";
        msg += e.what();
        console_message(msg);

        fileReader.close();

        return eResult::INVALID_DATA;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        fileReader.close();

        return eResult::INVALID_FILE;
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": CRC Error, ";
        msg += e.what();
        console_message(msg);

        fileReader.close();

        return eResult::INVALID_FILE;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Unexpected EOF, ";
        msg += e.what();
        console_message(msg);

        fileReader.close();

        return eResult::INVALID_FILE;
    }
    catch (const std::exception& e)
    {
        if (!fileReader.eof())
        {
            std::string msg = mCurrentFile.string();
            msg += ": Failed due to ";
            msg += e.what();
            console_message(msg);

            fileReader.close();

            return eResult::INVALID_FILE;
        }
    }

    fileReader.close();

    // Check the experiment information to see is the most basic information is there...
    if (!mExperimentFile.empty())
    {
        std::ifstream in;
        in.open(mExperimentFile.string());

        if (!in.is_open())
        {
            in.close();
        }
        else
        {
            try
            {
                cdr::cExperimentInfoFromJson info;

                nlohmann::json jsonDoc = nlohmann::json::parse(in, nullptr, false, true);
                info.parse(jsonDoc);

                in.close();

                mExperimentInfoRepairParser->setReferenceInfo(info);
            }
            catch (const std::exception& e)
            {
                auto msg = e.what();
                console_message(msg);
            }
        }
    }

    return eResult::VALID;
}

//-----------------------------------------------------------------------------
cDataRepair::eResult cDataRepair::pass2()
{
    update_prefix_progress(mID, "Repairing", 0);

    mFileReader.registerCallback([this](const cBlockID& id) { this->processBlock(id); });
    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len) { this->processBlock(id, buf, len); });

    mFileReader.attach(mExperimentInfoRepairParser.get());
    mFileReader.attach(mOusterRepairParser.get());

    mExperimentInfoRepairParser->attach(&mFileWriter);
    mOusterRepairParser->attach(&mFileWriter);

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                mFileWriter.close();
                return eResult::INVALID_FILE;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Invalid data, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();
        mFileWriter.close();

        return eResult::INVALID_DATA;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();
        mFileWriter.close();

        return eResult::INVALID_FILE;
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": CRC Error, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();
        mFileWriter.close();

        return eResult::INVALID_FILE;
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = mCurrentFile.string();
        msg += ": Unexpected EOF, ";
        msg += e.what();
        console_message(msg);

        mFileReader.close();
        mFileWriter.close();

        return eResult::INVALID_FILE;
    }
    catch (const std::exception& e)
    {
        if (!mFileReader.eof())
        {
            std::string msg = mCurrentFile.string();
            msg += ": Failed due to ";
            msg += e.what();
            console_message(msg);

            mFileReader.close();
            mFileWriter.close();

            return eResult::INVALID_FILE;
        }
    }

    mFileReader.close();
    mFileWriter.close();

    return eResult::VALID;
}

//-----------------------------------------------------------------------------
cDataRepair::eResult cDataRepair::pass3()
{
    update_prefix_progress(mID, "Verifing", 0);

    cBlockDataFileReader fileReader;

    fileReader.open(mTemporaryFile.string());

    if (!fileReader.isOpen())
    {
        throw std::logic_error("No file is open for verification.");
    }

    mFileSize = fileReader.file_size();

    try
    {
        while (!fileReader.eof())
        {
            if (fileReader.fail())
            {
                fileReader.close();
                return eResult::INVALID_FILE;
            }

            fileReader.processBlock();

            auto file_pos = static_cast<double>(fileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const bdf::invalid_data& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Invalid Data, ";
        msg += e.what();
        console_message(msg);

        return eResult::INVALID_DATA;
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": ";
        msg += e.what();
        console_message(msg);

        return eResult::INVALID_FILE;
    }
    catch (const bdf::formatting_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Formatting Error, ";
        msg += e.what();
        console_message(msg);

        return eResult::INVALID_FILE;
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Stream Error, ";
        msg += e.what();
        console_message(msg);

        return eResult::INVALID_FILE;
    }
    catch (const bdf::io_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": IO Error, ";
        msg += e.what();
        console_message(msg);

        return eResult::INVALID_FILE;
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = mTemporaryFile.filename().string();
        msg += ": Runtime Error, ";
        msg += e.what();
        console_message(msg);

        return eResult::INVALID_FILE;
    }
    catch (const std::exception& e)
    {
        if (!fileReader.eof())
        {
            std::string msg = mTemporaryFile.filename().string();
            msg += ": std::exception, ";
            msg += e.what();
            console_message(msg);

            return eResult::INVALID_FILE;
        }
    }

    fileReader.close();

    return eResult::VALID;
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
void cDataRepair::deleteTemporaryFile()
{
    if (mFileReader.isOpen())
        mFileReader.close();

    if (mFileWriter.isOpen())
        mFileWriter.close();

    std::filesystem::remove(mTemporaryFile);
}


//-----------------------------------------------------------------------------
bool cDataRepair::moveRepairedFile()
{
    // Move the fully repaired data file back into the main data
    // directory.
    auto dataDirectory = mTemporaryFile.parent_path();

    if (!std::filesystem::exists(dataDirectory))
        return false;

    std::filesystem::path dest = dataDirectory / mTemporaryFile.filename();

    if (std::filesystem::exists(dest))
        return false;

    std::filesystem::rename(mTemporaryFile, dest);

    return true;
}


