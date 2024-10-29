
#include "PlotDataConfigHeight.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

const char* PLOT_HEIGHT_SECTION_NAME = "plot height";

cPlotDataConfigHeight::cPlotDataConfigHeight()
{}

void cPlotDataConfigHeight::clear()
{
	mDirty = false;

	mGroundLevelBound_mm = 0.0;

	mHeightPercentile = 95.0;
}

bool cPlotDataConfigHeight::isDirty() const
{
	return mDirty;
}

double cPlotDataConfigHeight::getGroundLevelBound_mm() const
{
	return mGroundLevelBound_mm;
}

double cPlotDataConfigHeight::getHeightPercentile() const
{
	return mHeightPercentile;
}

void cPlotDataConfigHeight::setGroundLevelBound_mm(double ground_level_mm)
{
	mDirty |= (mGroundLevelBound_mm != ground_level_mm);
	mGroundLevelBound_mm = ground_level_mm;
}

void cPlotDataConfigHeight::setHeightPercentile(double height_percentile)
{
	mDirty |= (mHeightPercentile != height_percentile);
	mHeightPercentile = height_percentile;
}

void cPlotDataConfigHeight::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cPlotDataConfigHeight::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains(PLOT_HEIGHT_SECTION_NAME))
		return;
		
	auto height = jdoc[PLOT_HEIGHT_SECTION_NAME];

	mGroundLevelBound_mm = height["ground level bound (mm)"];
	mHeightPercentile = height["height percentile"];
}

void cPlotDataConfigHeight::save(nlohmann::json& jdoc)
{
	nlohmann::json height;

	height["ground level bound (mm)"] = mGroundLevelBound_mm;
	height["height percentile"] = mHeightPercentile;

	mDirty = false;

	jdoc[PLOT_HEIGHT_SECTION_NAME] = height;
}


