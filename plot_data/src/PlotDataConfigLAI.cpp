
#include "PlotDataConfigLAI.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

const char* LAI_SECTION_NAME = "lai";

cPlotDataConfigLAI::cPlotDataConfigLAI()
{}

void cPlotDataConfigLAI::clear()
{
	mDirty = false;

	mGroundLevelBound_mm = 0.0;
	mVoxelSize_mm = 1.0;
	mMinBinCount = 1;

	mFilters.clear();
}

bool cPlotDataConfigLAI::isDirty() const
{
	return mDirty;
}

eLaiAlgorithmType cPlotDataConfigLAI::getAlgorithmType() const
{
	return mAlgorithmType;
}

double cPlotDataConfigLAI::getGroundLevelBound_mm() const
{
	return mGroundLevelBound_mm;
}

double cPlotDataConfigLAI::getVoxelSize_mm() const
{
	return mVoxelSize_mm;
}

int cPlotDataConfigLAI::getMinBinCount() const
{
	return mMinBinCount;
}

void cPlotDataConfigLAI::setAlgorithmType(eLaiAlgorithmType algorithm_type)
{
	mDirty |= (mAlgorithmType != algorithm_type);
	mAlgorithmType = algorithm_type;
}

void cPlotDataConfigLAI::setGroundLevelBound_mm(double ground_level_mm)
{
	mDirty |= (mGroundLevelBound_mm != ground_level_mm);
	mGroundLevelBound_mm = ground_level_mm;
}

void cPlotDataConfigLAI::setVoxelSize_mm(double voxel_size_mm)
{
	mDirty |= (mVoxelSize_mm != voxel_size_mm);
	mVoxelSize_mm = voxel_size_mm;
}

void cPlotDataConfigLAI::setMinBinCount(int min_bin_count)
{
	mDirty |= (mMinBinCount != min_bin_count);
	mMinBinCount = min_bin_count;
}

bool cPlotDataConfigLAI::hasFilters() const
{
	return !mFilters.isEmpty();
}

const std::vector<cPlotDataConfigFilter*>& cPlotDataConfigLAI::getFilters() const
{
	return mFilters.getFilters();
}

void cPlotDataConfigLAI::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cPlotDataConfigLAI::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains(LAI_SECTION_NAME))
		return;
		
	auto lai = jdoc[LAI_SECTION_NAME];

	std::string type;
	
	if (lai.contains("algorithm type"))
		type = lai["algorithm type"];
	else if(lai.contains("algorithm_type"))
		type = lai["algorithm_type"];
	else
	{
		throw std::logic_error("Configuration file missing \"algorithm type\" entry!");
	}

	if ((type == "oct_tree") || (type == "oct tree"))
	{
		mAlgorithmType = eLaiAlgorithmType::OCT_TREE;
		if (lai.contains("min_bin_count"))
			mMinBinCount = lai["min_bin_count"];
		else
			throw std::logic_error("Algorithm type \"oct tree\" requires the \"min_bin_count\" parameter!");
	}
	else if ((type == "voxel_grid") || (type == "voxel grid"))
	{
		mAlgorithmType = eLaiAlgorithmType::VOXEL_GRID;
		if (lai.contains("min_bin_count"))
			mMinBinCount = lai["min_bin_count"];
		else
			mMinBinCount = 0;
	}
	else if (type == "open3d")
	{
		mAlgorithmType = eLaiAlgorithmType::OPEN3D;
	}
	else if (type == "convex_hull")
	{
		mAlgorithmType = eLaiAlgorithmType::CONVEX_HULL;
	}

	if (lai.contains("ground_level_bound_mm"))
		mGroundLevelBound_mm = lai["ground_level_bound_mm"];
	else
		mGroundLevelBound_mm = lai["ground level bound (mm)"];

	if (lai.contains("voxel_size_mm"))
		mVoxelSize_mm = lai["voxel_size_mm"];
	else
		mVoxelSize_mm = lai["voxel size (mm)"];

	mFilters.load(lai);
}

void cPlotDataConfigLAI::save(nlohmann::json& jdoc)
{
	nlohmann::json lai;

	switch (mAlgorithmType)
	{
	case eLaiAlgorithmType::OCT_TREE:
		lai["algorithm_type"] = "oct_tree";
		lai["min_bin_count"] = mMinBinCount;
		break;
	case eLaiAlgorithmType::VOXEL_GRID:
		lai["algorithm_type"] = "voxel_grid";
		if (mMinBinCount > 0)
			lai["min_bin_count"] = mMinBinCount;
		break;
	case eLaiAlgorithmType::OPEN3D:
		lai["algorithm_type"] = "open3d";
		break;
	case eLaiAlgorithmType::CONVEX_HULL:
		lai["algorithm_type"] = "convex_hull";
		break;
	};

	lai["ground level bound (mm)"] = mGroundLevelBound_mm;
	lai["voxel size (mm)"] = mVoxelSize_mm;

	mDirty = false;

	jdoc[LAI_SECTION_NAME] = lai;
}


