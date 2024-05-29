
#include "LidarMapConfigOptions.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cLidarMapConfigOptions::cLidarMapConfigOptions()
{}

void cLidarMapConfigOptions::clear()
{
	mDirty = false;

	mSaveCompactDataFile = true;
	mSavePlyFiles = false;
	mPlysUseBinaryFormat = true;
	mSaveFrameIds = false;
	mSavePixelInfo = false;
}

bool cLidarMapConfigOptions::isDirty() const
{
	return mDirty;
}

bool cLidarMapConfigOptions::getSaveCompactDataFile() const
{
	return mSaveCompactDataFile;
}

bool cLidarMapConfigOptions::getSavePlyFiles() const
{
	return mSavePlyFiles;
}

bool cLidarMapConfigOptions::getPlysUseBinaryFormat() const
{
	return mPlysUseBinaryFormat;
}

bool cLidarMapConfigOptions::getSaveFrameIds() const
{
	return mSaveFrameIds;
}

bool cLidarMapConfigOptions::getSavePixelInfo() const
{
	return mSavePixelInfo;
}


void cLidarMapConfigOptions::setSaveCompactDataFile(bool option)
{
	mDirty |= (mSaveCompactDataFile != option);
	mSaveCompactDataFile = option;
}

void cLidarMapConfigOptions::setSavePlyFiles(bool option)
{
	mDirty |= (mSavePlyFiles != option);
	mSavePlyFiles = option;
}

void cLidarMapConfigOptions::setPlysUseBinaryFormat(bool option)
{
	mDirty |= (mPlysUseBinaryFormat != option);
	mPlysUseBinaryFormat = option;
}

void cLidarMapConfigOptions::setSaveFrameIds(bool option)
{
	mDirty |= (mSaveFrameIds != option);
	mSaveFrameIds = option;
}

void cLidarMapConfigOptions::setSavePixelInfo(bool option)
{
	mDirty |= (mSavePixelInfo != option);
	mSavePixelInfo = option;
}

void cLidarMapConfigOptions::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("options"))
		return;
		
	auto options = jdoc["options"];

	if (options.contains("create plys"))
		mSavePlyFiles = options["create plys"];

	if (options.contains("save compact data file"))
		mSaveCompactDataFile = options["save compact data file"];

	if (options.contains("plys use binary format"))
		mPlysUseBinaryFormat = options["plys use binary format"];

	if (options.contains("save frame id info"))
		mSaveFrameIds = options["save frame id info"];

	if (options.contains("save pixel info"))
		mSavePixelInfo = options["save pixel info"];
}

nlohmann::json cLidarMapConfigOptions::save()
{
	nlohmann::json options;

	options["save compact data file"] = mSaveCompactDataFile;
	options["save frame id info"] = mSaveFrameIds;
	options["save pixel info"] = mSavePixelInfo;

	options["create plys"] = mSavePlyFiles;
	options["plys use binary format"] = mPlysUseBinaryFormat;

	mDirty = false;

	return options;
}


