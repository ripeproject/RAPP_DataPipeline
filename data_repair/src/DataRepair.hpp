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
	enum class eResult { VALID, INVALID_DATA, INVALID_FILE };

public:
	explicit cDataRepair(int id, std::filesystem::path temporary_dir);

    cDataRepair(int id, std::filesystem::path file_to_repair,
				std::filesystem::path temporary_dir);

    ~cDataRepair();

	std::filesystem::path tempFileName();

	bool open(std::filesystem::path file_to_repair);

	// Repair data in the recovered file if possible
	eResult pass1();

	// Validate the repaired file
	eResult pass2();

	void deleteTemporaryFile();

private:
    void processBlock(const cBlockID& id);
    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

	bool moveRepairedFile();

private:
	const int mID;

	std::uintmax_t mFileSize = 0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;
	
	std::filesystem::path mTemporaryDirectory;
	std::filesystem::path mCurrentFile;
	std::filesystem::path mTemporaryFile;

	std::unique_ptr<cOusterRepairParser> mOusterRepairParser;
};

