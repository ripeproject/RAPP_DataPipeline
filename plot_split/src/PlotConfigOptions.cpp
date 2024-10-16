
#include "PlotConfigOptions.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cPlotConfigOptions::cPlotConfigOptions()
{}

void cPlotConfigOptions::clear()
{
	mDirty = false;

	mSavePlotsInSingleFile = true;
	mSavePlyFiles = false;
	mPlysUseBinaryFormat = true;
	mSaveFrameIds = false;
	mSavePixelInfo = false;
}

bool cPlotConfigOptions::isDirty() const
{
	return mDirty;
}

bool cPlotConfigOptions::getSavePlotsInSingleFile() const
{
	return mSavePlotsInSingleFile;
}

bool cPlotConfigOptions::getSavePlyFiles() const
{
	return mSavePlyFiles;
}

bool cPlotConfigOptions::getPlysUseBinaryFormat() const
{
	return mPlysUseBinaryFormat;
}

bool cPlotConfigOptions::getSaveFrameIds() const
{
	return mSaveFrameIds;
}

bool cPlotConfigOptions::getSavePixelInfo() const
{
	return mSavePixelInfo;
}


void cPlotConfigOptions::setSavePlotsInSingleFile(bool option)
{
	mDirty |= (mSavePlotsInSingleFile != option);
	mSavePlotsInSingleFile = option;
}

void cPlotConfigOptions::setSavePlyFiles(bool option)
{
	mDirty |= (mSavePlyFiles != option);
	mSavePlyFiles = option;
}

void cPlotConfigOptions::setPlysUseBinaryFormat(bool option)
{
	mDirty |= (mPlysUseBinaryFormat != option);
	mPlysUseBinaryFormat = option;
}

void cPlotConfigOptions::setSaveFrameIds(bool option)
{
	mDirty |= (mSaveFrameIds != option);
	mSaveFrameIds = option;
}

void cPlotConfigOptions::setSavePixelInfo(bool option)
{
	mDirty |= (mSavePixelInfo != option);
	mSavePixelInfo = option;
}

void cPlotConfigOptions::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cPlotConfigOptions::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("options"))
		return;
		
	auto options = jdoc["options"];

	if (options.contains("create plys"))
		mSavePlyFiles = options["create plys"];

	if (options.contains("save plots in single file"))
		mSavePlotsInSingleFile = options["save plots in single file"];

	if (options.contains("plys use binary format"))
		mPlysUseBinaryFormat = options["plys use binary format"];

	if (options.contains("save frame id info"))
		mSaveFrameIds = options["save frame id info"];

	if (options.contains("save pixel info"))
		mSavePixelInfo = options["save pixel info"];
}

nlohmann::json cPlotConfigOptions::save()
{
	nlohmann::json options;

	options["save plots in single file"] = mSavePlotsInSingleFile;
	options["save frame id info"] = mSaveFrameIds;
	options["save pixel info"] = mSavePixelInfo;

	options["create plys"] = mSavePlyFiles;
	options["plys use binary format"] = mPlysUseBinaryFormat;

	mDirty = false;

	return options;
}


