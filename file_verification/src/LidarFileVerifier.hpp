/**
 * @file
 */
#pragma once

#include "AbstractFileVerifier.hpp"

#include "bdf_v1/BlockDataFile.hpp"

#include <filesystem>
#include <string>


class cLidarFileVerifier : public cAbstractFileVerifier
{
public:
	explicit cLidarFileVerifier(int id, std::filesystem::path failed_dir);
	cLidarFileVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cLidarFileVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	void process_file() override;

protected:
	bool open(std::filesystem::path file_to_check);
	void run();

	void moveFileToFailed();

private:
	const int mID;

	std::uintmax_t mFileSize;
	v1::cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mFileToCheck;
};

