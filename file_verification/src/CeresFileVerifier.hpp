/**
 * @file
 */
#pragma once

#include "AbstractFileVerifier.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>

namespace ceres_file_verifier
{
	extern std::atomic<uint32_t> g_num_failed_files;
}


class cCeresFileVerifier : public cAbstractFileVerifier
{
public:
	explicit cCeresFileVerifier(int id, std::filesystem::path failed_dir);
	cCeresFileVerifier(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path failed_dir);
    ~cCeresFileVerifier();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	cCeresFileVerifier::eRETURN_TYPE process_file() override;

protected:
	enum class eResult { VALID, INVALID_DATA, INVALID_FILE };

	bool open(std::filesystem::path file_to_check);
	eResult run();

	// Pass1: A simple pass looking for CRC or other stream errors
	bool pass1();

	// Pass2: A pass through the file doing a data validation
	bool pass2();

	void moveFileToFailed();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
    cBlockDataFileReader mFileReader;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mFileToCheck;
};

