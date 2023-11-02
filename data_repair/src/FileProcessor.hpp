
#pragma once

#include "DataRepair.hpp"

#include <filesystem>
#include <string>
#include <memory>


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::path temp_dir,
		std::filesystem::path failed_dir,
		std::filesystem::path repaired_dir);

	~cFileProcessor();

	bool setFileToRepair(std::filesystem::directory_entry file_to_repair);

	void process_file();

protected:
	void run();

private:
	const int mID;

	const std::filesystem::path mTemporaryDirectory;
	std::filesystem::path mTemporaryFile;

	const std::filesystem::path mRepairedDirectory;
	const std::filesystem::path mFailedDirectory;

	std::filesystem::path mFileToRepair;

	std::unique_ptr<cDataRepair>		mDataRepair;
};
