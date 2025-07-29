/**
 * @file
 */
#pragma once

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <filesystem>
#include <string>
#include <memory>


namespace ceres_data_verifier
{
	extern std::atomic<uint32_t> g_num_failed_files;
	extern std::atomic<uint32_t> g_num_invalid_files;
	extern std::atomic<uint32_t> g_num_missing_data;
}


class cCeresDataVerifier 
{
public:
	enum class eRETURN_TYPE {PASSED, COULD_NOT_OPEN_FILE, INVALID_DATA, INVALID_FILE };


public:
	cCeresDataVerifier(int id, std::filesystem::path invalid_dir, std::filesystem::path exp_file);

	cCeresDataVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path invalid_dir, std::filesystem::path exp_file);

    ~cCeresDataVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);
	void process_file();

	bool open(std::filesystem::path file_to_check);
	eRETURN_TYPE run();

protected:
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

