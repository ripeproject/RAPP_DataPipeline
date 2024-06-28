/**
 * @file
 */
#pragma once

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <filesystem>
#include <string>
#include <memory>


class cCeresDataVerifier 
{
public:
	cCeresDataVerifier(int id, std::filesystem::path invalid_dir, std::filesystem::path exp_file);

	cCeresDataVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path invalid_dir, std::filesystem::path exp_file);

    ~cCeresDataVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	void process_file();

protected:
	enum class eResult {VALID, INVALID_DATA, INVALID_FILE};

	bool open(std::filesystem::path file_to_check);
	void run();

	void moveFileToInvalid();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
    cBlockDataFileReader mFileReader;

	std::shared_ptr<cExperimentInfo> mExperimentInfo;

	std::filesystem::path mInvalidDirectory;
	std::filesystem::path mFileToCheck;
	std::filesystem::path mExperimentFile;
};

