/**
 * @file
 */
#pragma once

#include "bdf_v1/BlockDataFile.hpp"

#include <filesystem>
#include <string>


class cLidarDataVerifier 
{
public:
	explicit cLidarDataVerifier(std::filesystem::path failed_dir);
	cLidarDataVerifier(std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cLidarDataVerifier();

	bool open(std::filesystem::directory_entry file_to_check);

	void process_file(std::filesystem::directory_entry file_to_check);
	void run();

protected:
	void moveFileToFailed();

private:

private:
    v1::cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mCurrentFile;
};

