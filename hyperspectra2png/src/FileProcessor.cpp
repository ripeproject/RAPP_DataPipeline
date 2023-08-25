
#include "FileProcessor.hpp"
#include "HySpexVNIR3000N_2_RGB.hpp"
#include "HySpexSWIR384_2_RGB.hpp"

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
    mVnirConverter{new cHySpexVNIR3000N_2_Rgb()}, mSwirConverter {new cHySpexSWIR384_2_Rgb()
}
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
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
    auto pos = mFileReader.filePosition();

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

    cHySpexVNIR3000N_2_Rgb* p = mVnirConverter.get();
	mFileReader.attach(static_cast<cHySpexVNIR_3000N_Parser*>(p));
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


