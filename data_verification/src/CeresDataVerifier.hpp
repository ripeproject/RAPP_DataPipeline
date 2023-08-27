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
	explicit cCeresDataVerifier(int id, std::filesystem::path failed_dir);
	cCeresDataVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cCeresDataVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	void process_file();

protected:
	bool open(std::filesystem::path file_to_check);
	void run();

	// Pass1: A simple pass looking for CRC or other stream errors
	bool pass1();

	// Pass2: A pass through the file doing a data validation
	bool pass2();

	void moveFileToFailed();

private:
	const int mID;

	std::uintmax_t mFileSize;
    cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mFileToCheck;
};

