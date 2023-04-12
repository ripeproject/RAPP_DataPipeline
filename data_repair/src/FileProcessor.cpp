
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

void cFileProcessor::process_file(std::filesystem::directory_entry file_to_repair)
{
    mInputFile = file_to_repair.path();
}

void cFileProcessor::run()
{
}

