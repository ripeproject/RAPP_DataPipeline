
#include "FileProcessor.hpp"
#include "export_jpegs.hpp"

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
    mConverter{new cExportJpegs }
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

bool cFileProcessor::open(std::filesystem::path out)
{
    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;

    testFile.replace_extension(".0.jpeg");

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    mConverter->setOutputPath(outFile);

    mFileReader.open(mInputFile.string());

    return mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open(mOutputFile))
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

    cExportJpegs* p = mConverter.get();
	mFileReader.attach(static_cast<cAxisCommunicationsParser*>(p));

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


