
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
extern void new_file_progress(const int id, std::string filename);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);
extern void update_progress(const int id, const int progress_pct);
extern void complete_file_progress(const int id);


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id), mConverter{new cLidar2PointCloud}, mSerializer(1024)
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileWriter.close();
    mFileReader.close();
}

void cFileProcessor::setValidRange_m(double min_dist_m, double max_dist_m)
{
    mConverter->setValidRange_m(min_dist_m, max_dist_m);
}

void cFileProcessor::setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg)
{
    mConverter->setAzimuthWindow_deg(min_azimuth_deg, max_azimuth_deg);
}

void cFileProcessor::setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg)
{
    mConverter->setAltitudeWindow_deg(min_altitude_deg, max_altitude_deg);
}

void cFileProcessor::setOutputOption(cLidar2PointCloud::eOutputOptions options)
{
    mConverter->setOutputOption(options);
}

void cFileProcessor::setSaveOption(cLidar2PointCloud::eSaveOptions options)
{
    mConverter->setSaveOption(options);
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

    mFileSize = mFileReader.file_size();

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open())
    {
        new_file_progress(mID, mInputFile.string());

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
    mConverter->attach(&mFileWriter);

    mSerializer.write("lidar2pointcloud", processing_info::ePROCESSING_TYPE::POINT_CLOUD_GENERATION);

    mConverter->writeHeader();

    try
    {
        if (mConverter->requiresTelemetryPass())
        {
            update_prefix_progress(mID, "Telemetry Pass", 0);

            mConverter->attachKinematicParsers(mFileReader);

            while (!mFileReader.eof())
            {
                if (mFileReader.fail())
                {
                    break;
                }

                mFileReader.processBlock();

                auto file_pos = static_cast<double>(mFileReader.filePosition());
                file_pos = 100.0 * (file_pos / mFileSize);

                if (static_cast<int>(file_pos) > 1)
                    update_progress(mID, static_cast<int>(file_pos));
            }

            mFileReader.gotoBeginning();
            mConverter->telemetryPassComplete();

            mConverter->detachKinematicParsers(mFileReader);
        }

        mConverter->attachTransformParsers(mFileReader);
        mConverter->attachTransformSerializers(mFileWriter);

	    mFileReader.registerCallback([this](const cBlockID& id){ this->processBlock(id); });
	    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len){ this->processBlock(id, buf, len); });
	    mFileReader.attach(mConverter.get());
        mConverter.get()->attach(&mFileWriter);

        update_prefix_progress(mID, "Compute Pass", 0);

        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();

                mConverter->writeAndClearData();

                mFileWriter.close();

                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);

            if (static_cast<int>(file_pos) > 1)
                update_progress(mID, static_cast<int>(file_pos));
        }

        mConverter->writeAndClearData();
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

    complete_file_progress(mID);
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
