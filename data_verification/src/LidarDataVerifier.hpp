/**
 * @file
 */
#pragma once

#include <filesystem>
#include <string>
#include <memory>

namespace v1
{
	class cBlockDataFileReader;
}

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
	std::unique_ptr<v1::cBlockDataFileReader> mpFileReader;

	std::filesystem::path mInvalidDirectory;
	std::filesystem::path mFileToCheck;
};

