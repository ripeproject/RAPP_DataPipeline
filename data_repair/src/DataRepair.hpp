/**
 * @file
 */
#pragma once

#include "ExperimentInfoRepairParser.hpp"
#include "OusterRepairParser.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>


class cDataRepair
{
public:
	enum class eResult { VALID, INVALID_DATA, INVALID_FILE, MISSING_DATA };

public:
	explicit cDataRepair(int id, std::filesystem::path temporary_dir, 
				std::filesystem::path exp_file);

    cDataRepair(int id, std::filesystem::path file_to_repair,
				std::filesystem::path temporary_dir, 
				std::filesystem::path exp_file);

    ~cDataRepair();

	std::filesystem::path tempFileName();

	bool open(std::filesystem::path file_to_repair);

	// Check for missing experiment data
	eResult pass1();

	// Repair data in the recovered file if possible
	eResult pass2();

	// Validate the repaired file
	eResult pass3();

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
	std::filesystem::path mExperimentFile;

	std::unique_ptr<cExperimentInfoRepairParser>	mExperimentInfoRepairParser;
	std::unique_ptr<cOusterRepairParser>			mOusterRepairParser;
};

