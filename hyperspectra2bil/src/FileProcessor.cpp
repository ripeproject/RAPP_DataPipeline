
#include "FileProcessor.hpp"

#include "HySpexVNIR3000N_BIL_ArcMap.hpp"
#include "HySpexVNIR3000N_BIP_ArcMap.hpp"
#include "HySpexVNIR3000N_BSQ_ArcMap.hpp"

#include "HySpexVNIR3000N_BIL_ENVI.hpp"
#include "HySpexVNIR3000N_BIP_ENVI.hpp"
#include "HySpexVNIR3000N_BSQ_ENVI.hpp"

#include "HySpexSWIR384_BIL_ArcMap.hpp"
#include "HySpexSWIR384_BIP_ArcMap.hpp"
#include "HySpexSWIR384_BSQ_ArcMap.hpp"

#include "HySpexSWIR384_BIL_ENVI.hpp"
#include "HySpexSWIR384_BIP_ENVI.hpp"
#include "HySpexSWIR384_BSQ_ENVI.hpp"

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
    mID(id)
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

void cFileProcessor::setFormat(eExportFormat format)
{
    switch (format)
    {
    case eExportFormat::BIL:
        mVnirConverter.reset(new cHySpexVNIR3000N_BIL_ENVI());
        mSwirConverter.reset(new cHySpexSWIR384_BIL_ENVI());
        break;
    case eExportFormat::BIP:
        mVnirConverter.reset(new cHySpexVNIR3000N_BIP_ENVI());
        mSwirConverter.reset(new cHySpexSWIR384_BIP_ENVI());
        break;
    case eExportFormat::BSQ:
        mVnirConverter.reset(new cHySpexVNIR3000N_BSQ_ENVI());
        mSwirConverter.reset(new cHySpexSWIR384_BSQ_ENVI());
        break;
    };
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
    mFileSize = mFileReader.file_size();

    return mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (!mVnirConverter || !mSwirConverter)
    {
        throw std::logic_error("File format was not set.");
    }

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

    cHySpexVNIR3000N_File* pVnir = mVnirConverter.get();
	mFileReader.attach(static_cast<cHySpexVNIR_3000N_Parser*>(pVnir));
    mFileReader.attach(static_cast<cSpidercamParser*>(pVnir));

    cHySpexSWIR384_File* pSwir = mSwirConverter.get();
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

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_file_progress(mID, static_cast<int>(file_pos));
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


