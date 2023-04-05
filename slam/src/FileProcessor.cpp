
#include "FileProcessor.hpp"
#include "pointcloud2slam.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);


cFileProcessor::cFileProcessor() : mConverter{new cPointCloud2Slam}
{}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

bool cFileProcessor::open(std::filesystem::directory_entry in,
							std::filesystem::path out)
{
    mInputFile = in;

    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;
    testFile.replace_extension(".0.ply");
    if (std::filesystem::exists(testFile))
    {
        return false;
    }

//BAF    mConverter->setOutputPath(outFile);

    mFileReader.open(mInputFile.string());

    return mFileReader.isOpen();
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

    cPointCloud2Slam* p = mConverter.get();
	mFileReader.attach(static_cast<cPointCloudParser*>(p));
    mFileReader.attach(static_cast<cSpidercamParser*>(p));
    mFileReader.attach(static_cast<cSsnxParser*>(p));

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

            //if (p->mLidarFrameId > 5)
            //{
            //    mFileReader.close();
            //    return;
            //}
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


