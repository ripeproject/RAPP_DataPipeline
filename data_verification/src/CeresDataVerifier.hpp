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
	// Pass1: A simple pass looking for CRC or other stream errors
	bool pass1();

	// Pass2: A pass through the file doing a data validation
	bool pass2();

	void moveFileToFailed();

private:

private:
    cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mCurrentFile;
};

