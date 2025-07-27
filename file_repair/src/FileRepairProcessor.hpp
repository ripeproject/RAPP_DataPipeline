
#pragma once

#include <filesystem>
#include <string>
#include <memory>


extern std::atomic<uint32_t> g_num_partial_files;
extern std::atomic<uint32_t> g_num_repaired_files;

// Forward Declarations
class cDataFileRecovery;

class cFileRepairProcessor
{
public:
	cFileRepairProcessor(int id, std::filesystem::path temp_dir,
					std::filesystem::path partial_repaired_dir, 
					std::filesystem::path fully_repaired_dir);

	~cFileRepairProcessor();

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
