
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

eBiomassAlgorithmType cPlotDataConfigBiomass::getAlgorithmType() const
{
	return mAlgorithmType;
}

double cPlotDataConfigBiomass::getGroundLevelBound_mm() const
{
	return mGroundLevelBound_mm;
}

double cPlotDataConfigBiomass::getVoxelSize_mm() const
{
	return mVoxelSize_mm;
}

int cPlotDataConfigBiomass::getMinBinCount() const
{
	return mMinBinCount;
}

void cPlotDataConfigBiomass::setAlgorithmType(eBiomassAlgorithmType algorithm_type)
{
	mDirty |= (mAlgorithmType != algorithm_type);
	mAlgorithmType = algorithm_type;
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

void cPlotDataConfigBiomass::setMinBinCount(int min_bin_count)
{
	mDirty |= (mMinBinCount != min_bin_count);
	mMinBinCount = min_bin_count;
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
		
	auto biomass = jdoc[BIOMASS_SECTION_NAME];

	std::string type = biomass["algorithm_type"];

	if (type == "oct_tree")
	{
		mAlgorithmType = eBiomassAlgorithmType::OCT_TREE;
		mMinBinCount = biomass["min_bin_count"];
	}
	else if (type == "pcl")
	{
		mAlgorithmType = eBiomassAlgorithmType::PCL;
	}
	else if (type == "open3d")
	{
		mAlgorithmType = eBiomassAlgorithmType::OPEN3D;
	}
	else if (type == "qhull")
	{
		mAlgorithmType = eBiomassAlgorithmType::QHULL;
	}

	mGroundLevelBound_mm = biomass["ground level bound (mm)"];
	mVoxelSize_mm = biomass["voxel size (mm)"];

	mFilters.load(biomass);
}

void cPlotDataConfigBiomass::save(nlohmann::json& jdoc)
{
	nlohmann::json biomass;

	switch (mAlgorithmType)
	{
	case eBiomassAlgorithmType::OCT_TREE:
		biomass["algorithm_type"] = "oct_tree";
		biomass["min_bin_count"] = mMinBinCount;
		break;
	case eBiomassAlgorithmType::PCL:
		biomass["algorithm_type"] = "pcl";
		break;
	case eBiomassAlgorithmType::OPEN3D:
		biomass["algorithm_type"] = "open3d";
		break;
	case eBiomassAlgorithmType::QHULL:
		biomass["algorithm_type"] = "qhull";
		break;
	};

	biomass["ground level bound (mm)"] = mGroundLevelBound_mm;
	biomass["voxel size (mm)"] = mVoxelSize_mm;

	mDirty = false;

	jdoc[BIOMASS_SECTION_NAME] = biomass;
}


