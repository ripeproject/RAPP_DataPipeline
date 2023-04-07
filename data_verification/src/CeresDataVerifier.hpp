/**
 * @file
 */
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>


class cCeresDataVerifier 
{
public:
	explicit cCeresDataVerifier(std::filesystem::path failed_dir);
	cCeresDataVerifier(std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cCeresDataVerifier();

	bool open(std::filesystem::directory_entry file_to_check);

	void process_file(std::filesystem::directory_entry file_to_check);
	void run();

protected:
	void moveFileToFailed();

private:

private:
    cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mCurrentFile;
};

