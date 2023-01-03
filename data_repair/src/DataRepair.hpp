/**
 * @file
 */
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>

class cDataRepair : private cBlockDataFileReader
{
public:
	explicit cDataRepair(std::filesystem::path repaired_dir);
    cDataRepair(std::filesystem::directory_entry file_to_repair,
		std::filesystem::path repaired_dir);
    ~cDataRepair();

	bool open(std::filesystem::directory_entry file_to_repair);

	void process_file(std::filesystem::directory_entry file_to_repair);
	void run();

//signals:
//    void statusMessage(QString msg);
//    void fileResults(int id, bool valid, QString msg);

private:
//    void processBlock(const cBlockID& id) override;
//    void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len) override;

    bool moveFileToRepaired(bool size_check = true);

private:
    const int mId;
    cBlockDataFileReader mFileReader;
    cBlockDataFileWriter mFileWriter;

//    QString     mCurrentDataDirectory;
//    QString     mRepairedDataDirectory;
//    QString     mCurrentFileName;
//    QString     mRepairedFileName;

	std::filesystem::path mFailedDirectory;
	std::filesystem::path mCurrentFile;
};

