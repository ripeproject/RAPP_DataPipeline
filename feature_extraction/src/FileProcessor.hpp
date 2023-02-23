
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cExtractFeatures;


class cFileProcessor
{
public:
	cFileProcessor();
	~cFileProcessor();

	bool open(std::filesystem::directory_entry in, 
				std::filesystem::path out);

	void process_file(std::filesystem::directory_entry in,
				std::filesystem::path out);
	void run();

private:
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;

	std::unique_ptr<cExtractFeatures> mConverter;
};
