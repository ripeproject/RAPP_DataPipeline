
#include "PlotDataConfigLAI.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

const char* LAI_SECTION_NAME = "lai";

cPlotDataConfigLAI::cPlotDataConfigLAI()
{}

bool cPlotDataConfigLAI::hasData() const
{
	return mHasData;
}

void cPlotDataConfigLAI::clear()
{
	mHasData = false;

	mDirty = false;

	mExtinctionCoefficient = 0.0;
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

double cPlotDataConfigLAI::getExtinctionCoefficient() const
{
	return mExtinctionCoefficient;
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

void cPlotDataConfigLAI::setExtinctionCoefficient(double k)
{
	mDirty |= (mExtinctionCoefficient != k);
	mExtinctionCoefficient = k;
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

	if ((type == "light_penetration_index") || (type == "light penetration index") || (type == "lpi"))
	{
		mAlgorithmType = eLaiAlgorithmType::LPI;
	}

	if (lai.contains("extinction_coefficient"))
		mExtinctionCoefficient = lai["extinction_coefficient"];
	else
		mExtinctionCoefficient = lai["extinction coefficient"];

	if (lai.contains("ground_level_bound_mm"))
		mGroundLevelBound_mm = lai["ground_level_bound_mm"];
	else
		mGroundLevelBound_mm = lai["ground level bound (mm)"];

	if (lai.contains("voxel_size_mm"))
		mVoxelSize_mm = lai["voxel_size_mm"];
	else
		mVoxelSize_mm = lai["voxel size (mm)"];

	mFilters.load(lai);

	mHasData = true;
}

void cPlotDataConfigLAI::save(nlohmann::json& jdoc)
{
	nlohmann::json lai;

	switch (mAlgorithmType)
	{
	case eLaiAlgorithmType::LPI:
		lai["algorithm_type"] = "light_penetration_index";
		break;
	};

	lai["extinction coefficient"] = mExtinctionCoefficient;
	lai["ground level bound (mm)"] = mGroundLevelBound_mm;
	lai["voxel size (mm)"] = mVoxelSize_mm;

	mDirty = false;

	jdoc[LAI_SECTION_NAME] = lai;
}


