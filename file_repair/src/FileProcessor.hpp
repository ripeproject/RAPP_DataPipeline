
#pragma once

#include "DataFileRecovery.hpp"

#include <filesystem>
#include <string>
#include <memory>


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::path temp_dir,
					std::filesystem::path partial_repaired_dir, 
					std::filesystem::path fully_repaired_dir);

	~cFileProcessor();

	bool setFileToRepair(std::filesystem::directory_entry file_to_repair);

	void process_file();

protected:
	void run();

private:
	void moveToPartialRepaired();
	void moveToFullyRepaired();

private:
	const int mID;

	const std::filesystem::path mTemporaryDirectory;
	std::filesystem::path mTemporaryFile;

	const std::filesystem::path mPartialRepairedDirectory;
	const std::filesystem::path mFullyRepairedDirectory;

	std::filesystem::path mInputFile;

	std::unique_ptr<cDataFileRecovery>	mDataFileRecovery;
};
