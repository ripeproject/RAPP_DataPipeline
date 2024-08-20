
#pragma once

#include <cbdf/BlockDataFile.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cHySpexVNIR3000N_2_Png;
class cHySpexSWIR384_2_Png;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void setVnirRgb(float red_nm, float green_nm, float blue_nm);
	void setSwirRgb(float red_nm, float green_nm, float blue_nm);

	bool open(std::filesystem::path out);

	void process_file();
	void run();

private:
	const int mID;

	std::uintmax_t mFileSize;
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cHySpexVNIR3000N_2_Png> mVnirConverter;
	std::unique_ptr<cHySpexSWIR384_2_Png>   mSwirConverter;
};
