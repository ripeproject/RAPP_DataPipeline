
#include "FileProcessor.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);


cFileProcessor::cFileProcessor(std::filesystem::path recovered_dir, std::filesystem::path repaired_dir)
    : mRecoveredDirectory(recovered_dir), mRepairedDirectory(repaired_dir)
{
}

cFileProcessor::~cFileProcessor()
{
}

bool cFileProcessor::setFileToRepair(std::filesystem::directory_entry file_to_repair)
{
    if (file_to_repair.exists())
    {
        mInputFile = file_to_repair.path();
        return true;
    }

    return false;
}

void cFileProcessor::process_file()
{
    mDataFileRecovery = std::make_unique<cDataFileRecovery>(mRecoveredDirectory);
    
    if (mDataFileRecovery->open(mInputFile))
    {
        mRecoveredFile = mDataFileRecovery->recoveredFileName();

        mDataRepair = std::make_unique<cDataRepair>(mRepairedDirectory);

        run();
    }
}

void cFileProcessor::run()
{
    // Try to recover the data file
    if (!mDataFileRecovery->run())
    {
        std::string msg = "Warning: the file ";
        msg += mInputFile.filename().string();
        msg += " could not be completely recovered!";
        console_message(msg);
    }

    if (mDataRepair->open(mRecoveredFile))
    {
        mDataRepair->run();
    }
}

