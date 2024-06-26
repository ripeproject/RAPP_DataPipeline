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
	explicit cLidarDataVerifier(int id, std::filesystem::path invalid_dir);

	cLidarDataVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path invalid_dir);

    ~cLidarDataVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	void process_file();

protected:
	bool open(std::filesystem::path file_to_check);
	void run();
	void moveFileToInvalid();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
	v1::cBlockDataFileReader mFileReader;

	std::filesystem::path mInvalidDirectory;
	std::filesystem::path mFileToCheck;
};

