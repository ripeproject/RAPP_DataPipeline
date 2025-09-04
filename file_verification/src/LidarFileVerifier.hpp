/**
 * @file
 */
#pragma once

#include "AbstractFileVerifier.hpp"

#include <filesystem>
#include <string>
#include <memory>

namespace v1
{
	class cBlockDataFileReader;
};

class cLidarFileVerifier : public cAbstractFileVerifier
{
public:
	explicit cLidarFileVerifier(int id, std::filesystem::path failed_dir);
	cLidarFileVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cLidarFileVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	void process_file() override;

	bool open(std::filesystem::path file_to_check);
	cLidarFileVerifier::eRETURN_TYPE run();

protected:
	bool moveFileToFailed();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
	std::unique_ptr<v1::cBlockDataFileReader> mpFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mFileToCheck;
};

