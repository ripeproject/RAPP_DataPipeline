
#include "FileProcessor.hpp"
#include "HySpexVNIR3000N_2_PNG.hpp"
#include "HySpexSWIR384_2_PNG.hpp"

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
    mID(id), mVnirConverter{new cHySpexVNIR3000N_2_Png()}, mSwirConverter {new cHySpexSWIR384_2_Png()
}
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


void cFileProcessor::setVnirRgb(float red_nm, float green_nm, float blue_nm)
{
    mVnirConverter->setRgbWavelengths_nm(red_nm, green_nm, blue_nm);
}

void cFileProcessor::setSwirRgb(float red_nm, float green_nm, float blue_nm)
{
    mSwirConverter->setRgbWavelengths_nm(red_nm, green_nm, blue_nm);
}

bool cFileProcessor::open(std::filesystem::path out)
{
    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    mVnirConverter->setOutputPath(outFile);
    mSwirConverter->setOutputPath(outFile);

    mFileReader.open(mInputFile.string());
    mFileSize = mFileReader.size();

    return mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (open(mOutputFile))
    {
        std::string msg = "Processing ";
        msg = mInputFile.string();
        if (mpEventHandler)
        {
            auto event = new cFileProgressEvent(NEW_FILE_PROGRESS);
            event->SetFileProcessID(mID);
            event->SetFileName(msg);

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

    cHySpexVNIR3000N_2_Png* pVnir = mVnirConverter.get();
	mFileReader.attach(static_cast<cHySpexVNIR_3000N_Parser*>(pVnir));
    mFileReader.attach(static_cast<cSpidercamParser*>(pVnir));

    cHySpexSWIR384_2_Png* pSwir = mSwirConverter.get();
    mFileReader.attach(static_cast<cHySpexSWIR_384_Parser*>(pSwir));
    mFileReader.attach(static_cast<cSpidercamParser*>(pSwir));

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


