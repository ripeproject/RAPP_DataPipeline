
#include "FileProcessor.hpp"
#include "export_jpegs.hpp"

#include "StringUtils.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_file_progress(const int id, const int progress_pct);


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id), mConverter{new cExportJpegs }
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

void cFileProcessor::setPlotFile(std::shared_ptr<cPlotConfigFile>& plot_file)
{
    mConverter->setPlotFile(plot_file);
}

bool cFileProcessor::open(std::filesystem::path out)
{
//    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path outPath = out.remove_filename();

    auto temp = nStringUtils::removeMeasurementTimestamp(mInputFile.filename().string());

    std::string timestamp;
    nStringUtils::extractMeasurementTimestamp(mInputFile.filename().string(), timestamp);

    std::string root_filename = temp.filename;

//    std::filesystem::path testFile = outFile / root_filename;
    std::filesystem::path testFile = outPath / root_filename;

    testFile.replace_extension("*.jpeg");

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

//    mConverter->setOutputPath(outFile);
    mConverter->setOutputPath(outPath);
    mConverter->setRootFileName(root_filename); 
    mConverter->setTimeStamp(timestamp);

    mFileReader.open(mInputFile.string());

    return mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open(mOutputFile))
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

    cExportJpegs* p = mConverter.get();
    mFileReader.attach(static_cast<cExperimentParser*>(p));
    mFileReader.attach(static_cast<cAxisCommunicationsParser*>(p));
    mFileReader.attach(static_cast<cSsnxParser*>(p));
    mFileReader.attach(static_cast<cSpidercamParser*>(p));

    mFileSize = mFileReader.file_size();

	try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_file_progress(mID, static_cast<int>(file_pos));

            if (mConverter->abort())
                break;
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

    update_file_progress(mID, 100);
}


