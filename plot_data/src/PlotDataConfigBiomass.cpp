
#include "PlotDataConfigBiomass.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

const char* BIOMASS_SECTION_NAME = "above-ground biomass";

cPlotDataConfigBiomass::cPlotDataConfigBiomass()
{}

void cPlotDataConfigBiomass::clear()
{
	mDirty = false;

	mGroundLevelBound_mm = 0.0;
	mVoxelSize_mm = 1.0;
}

bool cPlotDataConfigBiomass::isDirty() const
{
	return mDirty;
}

double cPlotDataConfigBiomass::getGroundLevelBound_mm() const
{
	return mGroundLevelBound_mm;
}

double cPlotDataConfigBiomass::getVoxelSize_mm() const
{
	return mVoxelSize_mm;
}

void cPlotDataConfigBiomass::setGroundLevelBound_mm(double ground_level_mm)
{
	mDirty |= (mGroundLevelBound_mm != ground_level_mm);
	mGroundLevelBound_mm = ground_level_mm;
}

void cPlotDataConfigBiomass::setVoxelSize_mm(double voxel_size_mm)
{
	mDirty |= (mVoxelSize_mm != voxel_size_mm);
	mVoxelSize_mm = voxel_size_mm;
}

bool cPlotDataConfigBiomass::hasFilters() const
{
	return !mFilters.isEmpty();
}

const std::vector<cPlotDataConfigFilter*>& cPlotDataConfigBiomass::getFilters() const
{
	return mFilters.getFilters();
}

void cPlotDataConfigBiomass::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cPlotDataConfigBiomass::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains(BIOMASS_SECTION_NAME))
		return;
		
	auto height = jdoc[BIOMASS_SECTION_NAME];

	mGroundLevelBound_mm = height["ground level bound (mm)"];
	mVoxelSize_mm = height["voxel size (mm)"];

	mFilters.load(height);
}

void cPlotDataConfigBiomass::save(nlohmann::json& jdoc)
{
	nlohmann::json height;

	height["ground level bound (mm)"] = mGroundLevelBound_mm;
	height["voxel size (mm)"] = mVoxelSize_mm;

	mDirty = false;

	jdoc[BIOMASS_SECTION_NAME] = height;
}


