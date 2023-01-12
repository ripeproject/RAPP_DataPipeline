
#include "FileProcessor.hpp"
#include "lidar2pointcloud.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);


cFileProcessor::cFileProcessor() : mConverter{new cLidar2PointCloud}
{}

cFileProcessor::~cFileProcessor()
{
    mFileWriter.close();
    mFileReader.close();
}

bool cFileProcessor::open(std::filesystem::directory_entry in,
							std::filesystem::path out)
{
    mInputFile = in;
    mOutputFile = out;

    if (std::filesystem::exists(mOutputFile))
    {
        return false;
    }

    mFileWriter.open(mOutputFile.string());
    mFileReader.open(mInputFile.string());

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

void cFileProcessor::process_file(std::filesystem::directory_entry in,
									std::filesystem::path out)
{
    if (open(in, out))
    {
        std::string msg = "Processing ";
        msg += in.path().string();
        msg += "...";
        console_message(msg);

        run();
    }
}

void cFileProcessor::run()
{
	if (!mFileReader.isOpen())
	{
        throw std::logic_error("No file is open for reading.");
	}

	mFileReader.registerCallback([this](const cBlockID& id){ this->processBlock(id); });
	mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len){ this->processBlock(id, buf, len); });
	mFileReader.attach(mConverter.get());
    mConverter.get()->attach(&mFileWriter);

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
        std::string msg = "Stream Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = "CRC Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = "Unexpected EOF: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const std::exception& e)
    {
        std::string msg = "Unknown Exception: ";
        msg += e.what();
        console_message(msg);
    }
}

void cFileProcessor::processBlock(const cBlockID& id)
{
	mFileWriter.writeBlock(id);
}

void cFileProcessor::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
	mFileWriter.writeBlock(id, buf, len);
}
