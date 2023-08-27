/**
 * @file
 */
#pragma once

#include "OusterRepairParser.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>


class cDataRepair
{
public:
	explicit cDataRepair(int id, std::filesystem::path repaired_dir);

    cDataRepair(int id, std::filesystem::path file_to_repair,
				std::filesystem::path repaired_dir);

    ~cDataRepair();

	bool open(std::filesystem::path file_to_repair);

	void run();

protected:
	// Repair data in the recovered file if possible
	bool pass1();

	// Validate the repaired file
	bool pass2();

private:
    void processBlock(const cBlockID& id);
    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

	bool removeRecoveryFile();
	bool moveRepairedFile();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;
	
	std::filesystem::path mRepairedDirectory;
	std::filesystem::path mCurrentFile;
	std::filesystem::path mRepairedFile;

	std::unique_ptr<cOusterRepairParser> mOusterRepairParser;
};

