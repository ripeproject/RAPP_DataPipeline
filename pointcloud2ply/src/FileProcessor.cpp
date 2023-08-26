
#include "FileProcessor.hpp"
#include "pointcloud2ply.hpp"

#include "../wxCustomWidgets/FileProgressCtrl.hpp"
#include <cbdf/BlockDataFileExceptions.hpp>

#include <wx/event.h>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id), mConverter{new cPointCloud2Ply}
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

void cFileProcessor::setEventHandler(wxEvtHandler* handler)
{
    mpEventHandler = handler;
}

bool cFileProcessor::open(std::filesystem::path out)
{
    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;

    if (mConverter->mIndividualPlyFiles)
        testFile.replace_extension(".0.ply");
    else
        testFile.replace_extension(".ply");

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    mConverter->setOutputPath(outFile);

    mFileReader.open(mInputFile.string());
    mFileSize = mFileReader.size();

    return mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open(mOutputFile))
    {
        std::string msg = "Processing ";
        msg += mInputFile.string();
        msg += "...";
//        console_message(msg);
        if (mpEventHandler)
        {
            auto event = new cFileProgressEvent(NEW_FILE_PROGRESS);
            event->SetFileProcessID(mID);
            event->SetFileName(mInputFile.string());

            wxQueueEvent(mpEventHandler, event);
        }

        run();
    }
}

void cFileProcessor::run()
{
	if (!mFileReader.isOpen())
	{
        throw std::logic_error("No file is open for reading.");
	}

    cPointCloud2Ply* p = mConverter.get();
	mFileReader.attach(static_cast<cPointCloudParser*>(p));
    mFileReader.attach(static_cast<cSpidercamParser*>(p));

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

            if (mpEventHandler)
            {
                auto file_pos = static_cast<double>(mFileReader.filePosition());
                file_pos = 100.0 * (file_pos / mFileSize);

                auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
                event->SetFileProcessID(mID);
                event->SetProgress_pct(static_cast<int>(file_pos));

                wxQueueEvent(mpEventHandler, event);
            }
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

    if (mpEventHandler)
    {
        auto event = new cFileProgressEvent(UPDATE_FILE_PROGRESS);
        event->SetFileProcessID(mID);
        event->SetProgress_pct(100);

        wxQueueEvent(mpEventHandler, event);
    }
}


