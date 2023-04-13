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
	explicit cDataRepair(std::filesystem::path repaired_dir);

    cDataRepair(std::filesystem::path file_to_repair,
				std::filesystem::path repaired_dir);

    ~cDataRepair();

	bool open(std::filesystem::path file_to_repair);

	void run();

private:
    void processBlock(const cBlockID& id);
    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

    bool moveFile(bool size_check = true);

private:
	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;
	
	std::filesystem::path mRepairedDirectory;
	std::filesystem::path mCurrentFile;
	std::filesystem::path mRepairedFile;

	std::unique_ptr<cOusterRepairParser> mOusterRepairParser;
};

