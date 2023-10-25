/**
 * @file
 */
#pragma once

#include "BlockDataFileRecovery.hpp"

#include <filesystem>
#include <string>

class cDataFileRecovery : private cBlockDataFileRecovery
{
public:
	explicit cDataFileRecovery(int id, std::filesystem::path temporary_dir);
	cDataFileRecovery(int id, std::filesystem::path file_to_recover,
						std::filesystem::path temporary_dir);
    ~cDataFileRecovery();

	std::filesystem::path tempFileName();

	bool open(std::filesystem::path file_to_recover);

	bool run();

protected:

	// Recovered the file if possible
	bool pass1();

	// Validate the recovered file
	bool pass2();

private:
    void processBlock(const cBlockID& id) override;
    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len) override;

    bool removeFailedFile();

private:
	const int mID;

	std::uintmax_t mOriginalFileSize = 0;
	std::uintmax_t mRecoveredFileSize = 0;

	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mTemporaryDirectory;
	std::filesystem::path mCurrentFile;
	std::filesystem::path mTemporaryFile;
};

