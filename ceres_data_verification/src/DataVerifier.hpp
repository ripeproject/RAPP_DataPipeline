/**
 * @file
 */
#pragma once

#include <CBDF/BlockDataFile.hpp>
//#include "AxisCommunicationsParser.hpp"

#include <filesystem>
#include <string>


class cDataVerifier 
{
public:
    cDataVerifier(std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cDataVerifier();

    void run();

protected:
	void moveFileToFailed();

private:

private:
    cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mCurrentFile;
};

