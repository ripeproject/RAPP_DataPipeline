
#include "PlotDataConfigOptions.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

const char* OPTIONS_SECTION_NAME = "options";

cPlotDataConfigOptions::cPlotDataConfigOptions()
{}

void cPlotDataConfigOptions::clear()
{
	mDirty = false;

	mSaveDataRowMajor = false;
}

bool cPlotDataConfigOptions::isDirty() const
{
	return mDirty;
}

bool cPlotDataConfigOptions::getSaveDataRowMajor() const
{
	return mSaveDataRowMajor;
}



void cPlotDataConfigOptions::setSaveDataRowMajor(bool row_major)
{
	mDirty |= (mSaveDataRowMajor != row_major);
	mSaveDataRowMajor = row_major;
}

void cPlotDataConfigOptions::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cPlotDataConfigOptions::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains(OPTIONS_SECTION_NAME))
		return;
		
	auto options = jdoc[OPTIONS_SECTION_NAME];

	mSaveDataRowMajor = options["save row major"];
}

void cPlotDataConfigOptions::save(nlohmann::json& jdoc)
{
	nlohmann::json options;

	options["save row major"] = mSaveDataRowMajor;

	mDirty = false;

	jdoc[OPTIONS_SECTION_NAME] = options;
}


