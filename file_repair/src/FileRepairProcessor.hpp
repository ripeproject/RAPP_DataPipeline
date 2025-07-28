
#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <atomic>

namespace ceres_file_repair
{
	extern std::atomic<uint32_t> g_num_partial_files;
	extern std::atomic<uint32_t> g_num_repaired_files;
}

// Forward Declarations
class cDataFileRecovery;

class cFileRepairProcessor
{
public:
	enum class eRETURN_TYPE { REPAIRED, COULD_NOT_OPEN_FILE, FAILED };

public:
	cFileRepairProcessor(int id, std::filesystem::path temp_dir,
					std::filesystem::path partial_repaired_dir, 
					std::filesystem::path fully_repaired_dir);

	~cFileRepairProcessor();

	bool setFileToRepair(std::filesystem::directory_entry file_to_repair);

	eRETURN_TYPE process_file();

protected:
	enum class eResult { REPAIRED, PARTIAL_REPAIR };

	eResult run();

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
