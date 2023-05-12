
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


cFileProcessor::cFileProcessor(std::filesystem::directory_entry in,
                                std::filesystem::path out)
:
    mConverter{new cLidar2PointCloud}, mSerializer(1024)
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileWriter.close();
    mFileReader.close();
}

/**
 * Set the kinematic type to apply to the pointcloud data.
 */
void cFileProcessor::setKinematicModel(std::unique_ptr<cKinematics> model)
{
    mConverter->setKinematicModel(std::move(model));
}

bool cFileProcessor::open()
{
    if (std::filesystem::exists(mOutputFile))
    {
        return false;
    }

    mFileWriter.open(mOutputFile.string());
    mFileReader.open(mInputFile.string());

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open())
    {
        std::string msg = "Processing ";
        msg += mInputFile.string();
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

    mSerializer.attach(&mFileWriter);

    mSerializer.write("lidar2pointcloud", processing_info::ePROCESSING_TYPE::POINT_CLOUD_GENERATION);

    mConverter->writeHeader();

    try
    {
        if (mConverter->requiresTelemetryPass())
        {
            mConverter->attachKinematicParsers(mFileReader);

            while (!mFileReader.eof())
            {
                if (mFileReader.fail())
                {
                    break;
                }

                mFileReader.processBlock();
            }

            mFileReader.gotoBeginning();
            mConverter->telemetryPassComplete();

            mConverter->detachKinematicParsers(mFileReader);
        }
        else
            mConverter->attachKinematicParsers(mFileReader);

	    mFileReader.registerCallback([this](const cBlockID& id){ this->processBlock(id); });
	    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len){ this->processBlock(id, buf, len); });
	    mFileReader.attach(mConverter.get());
        mConverter.get()->attach(&mFileWriter);

        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();

                mConverter->writeAndCloseData();

                mFileWriter.close();
                return;
            }

            mFileReader.processBlock();
        }

        mConverter->writeAndCloseData();
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
    catch (const cKinematicNoData& e)
    {
        std::string msg = e.what();
        console_message(msg);
        deleteOutputFile();
    }
    catch (const cKinematicException& e)
    {
        std::string msg = "Kinematic Error: ";
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

void cFileProcessor::deleteOutputFile()
{
    mFileWriter.close();
    std::filesystem::remove(mOutputFile);
}
