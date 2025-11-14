
#pragma once

#include <filesystem>
#include <string>
#include <memory>


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	virtual ~cFileProcessor();

	virtual void setVnirRgb(float red_nm, float green_nm, float blue_nm);
	virtual void setSwirRgb(float red_nm, float green_nm, float blue_nm);

	virtual bool open(std::filesystem::path out) = 0;

	virtual void process_file() = 0;
	virtual void run() = 0;

protected:
	const int mID;

	std::uintmax_t mFileSize;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	float mVnirRed_nm   = 641.07f;
	float mVnirGreen_nm = 547.95f;
	float mVnirBlue_nm  = 463.30f;

	float mSwirRed_nm   = 1220.19f;
	float mSwirGreen_nm = 1655.20f;
	float mSwirBlue_nm  = 2144.57f;
};
