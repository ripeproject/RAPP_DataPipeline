
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cPointCloud2Ply;


class cFileProcessor
{
public:
	cFileProcessor(std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	bool open(std::filesystem::path out);

	void process_file();
	void run();

private:
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cPointCloud2Ply> mConverter;
};
