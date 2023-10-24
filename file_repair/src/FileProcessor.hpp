
#pragma once

#include "DataFileRecovery.hpp"

#include <filesystem>
#include <string>
#include <memory>


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::path recovered_dir, std::filesystem::path repaired_dir);
	~cFileProcessor();

	bool setFileToRepair(std::filesystem::directory_entry file_to_repair);

	void process_file();

protected:
	void run();

private:
	const int mID;

	const std::filesystem::path mRecoveredDirectory;
	std::filesystem::path mRecoveredFile;

	const std::filesystem::path mRepairedDirectory;
	std::filesystem::path mRepairedFile;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cDataFileRecovery>	mDataFileRecovery;
};
