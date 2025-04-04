
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cHySpexVNIR3000N_File;
class cHySpexSWIR384_File;

enum class eExportFormat {BIL, BIP, BSQ};

class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void setFormat(eExportFormat format);

	void process_file();
	void run();

protected:
	bool open(std::filesystem::path out);

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cHySpexVNIR3000N_File> mVnirConverter;
	std::unique_ptr<cHySpexSWIR384_File>   mSwirConverter;
};
