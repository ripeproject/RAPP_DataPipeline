
#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <atomic>

namespace ceres_data_repair
{
	extern std::atomic<uint32_t> g_num_partial_data_files;
	extern std::atomic<uint32_t> g_num_repaired_data_files;
}

//Forward Declarations
class cDataRepair;


class cDataRepairProcessor
{
public:
	enum class eRETURN_TYPE { PASSED, COULD_NOT_OPEN_FILE, INVALID_FILE, INVALID_DATA };

public:
	cDataRepairProcessor(int id, std::filesystem::path temp_dir,
		std::filesystem::path failed_dir,
		std::filesystem::path repaired_dir,
		std::filesystem::path exp_file);

	~cDataRepairProcessor();

	bool setFileToRepair(std::filesystem::directory_entry file_to_repair);
	void process_file();

	bool open(std::filesystem::path file_to_repair);
	eRETURN_TYPE run();


private:
	const int mID;

	const std::filesystem::path mTemporaryDirectory;
	std::filesystem::path mTemporaryFile;

	const std::filesystem::path mRepairedDirectory;
	const std::filesystem::path mFailedDirectory;
	const std::filesystem::path mExperimentFile;

	std::filesystem::path mFileToRepair;

	std::unique_ptr<cDataRepair>		mDataRepair;
};
