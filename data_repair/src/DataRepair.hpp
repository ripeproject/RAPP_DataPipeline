/**
 * @file
 */
#pragma once

#include "BlockDataFileRepair.hpp"

#include <filesystem>
#include <string>

class cDataRepair : private cBlockDataFileRepair
{
public:
	explicit cDataRepair(std::filesystem::path repaired_dir);
    cDataRepair(std::filesystem::directory_entry file_to_repair,
		std::filesystem::path repaired_dir);
    ~cDataRepair();

	bool open(std::filesystem::directory_entry file_to_repair);

	void process_file(std::filesystem::directory_entry file_to_repair);
	void run();

private:
    void processBlock(const cBlockID& id) override;
    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len) override;

    bool moveFileToRepaired(bool size_check = true);

private:
    cBlockDataFileWriter mFileWriter;

	std::filesystem::path mRepairedDirectory;
	std::filesystem::path mCurrentFile;
	std::filesystem::path mRepairedFile;
};

