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
	explicit cDataFileRecovery(int id, std::filesystem::path recovery_dir);
	cDataFileRecovery(int id, std::filesystem::path file_to_recover,
						std::filesystem::path recovery_dir);
    ~cDataFileRecovery();

	std::filesystem::path recoveredFileName();

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

	std::uintmax_t mFileSize = 0;

	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mRecoveryDirectory;
	std::filesystem::path mCurrentFile;
	std::filesystem::path mRecoveryFile;
};

