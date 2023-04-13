
#pragma once

#include "DataFileRecovery.hpp"
#include "DataRepair.hpp"

#include <filesystem>
#include <string>
#include <memory>


class cFileProcessor
{
public:
	cFileProcessor(std::filesystem::path recovered_dir, std::filesystem::path repaired_dir);
	~cFileProcessor();

	void process_file(std::filesystem::directory_entry file_to_repair);

protected:
	void run();

private:
	const std::filesystem::path mRecoveredDirectory;
	std::filesystem::path mRecoveredFile;

	const std::filesystem::path mRepairedDirectory;
	std::filesystem::path mRepairedFile;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cDataFileRecovery>	mDataFileRecovery;
	std::unique_ptr<cDataRepair>		mDataRepair;
};
