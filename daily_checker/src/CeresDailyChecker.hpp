/**
 * @file
 */
#pragma once

#include <file_verificator/CeresFileVerifier.hpp>
#include <file_verificator/LidarFileVerifier.hpp>

#include <file_repair/FileRepairProcessor.hpp>

#include <data_verificator/CeresDataVerifier.hpp>
#include <data_verificator/LidarDataVerifier.hpp>

#include <data_repair/DataRepairProcessor.hpp>


#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <filesystem>
#include <string>
#include <memory>


class cCeresDailyChecker
{
public:
	cCeresDailyChecker(int id, std::filesystem::path invalid_dir, std::filesystem::path exp_file);

	cCeresDailyChecker(int id, std::filesystem::directory_entry file_to_check,
		std::filesystem::path invalid_dir, std::filesystem::path exp_file);

    ~cCeresDailyChecker();

	bool setFileToCheck(std::filesystem::directory_entry file_to_check);

	void process_file();

protected:

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
    cBlockDataFileReader mFileReader;

};

