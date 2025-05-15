
#pragma once

#include "datatypes.hpp"

#include "PlotDataConfigFilters.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


enum class eLaiAlgorithmType {OCT_TREE, VOXEL_GRID, OPEN3D, CONVEX_HULL};

class cPlotDataConfigLAI
{
public:
	cPlotDataConfigLAI();
	~cPlotDataConfigLAI() = default;

	void clear();

	bool isDirty() const;

	eLaiAlgorithmType getAlgorithmType() const;

	double getGroundLevelBound_mm() const;
	double getVoxelSize_mm() const;

	int getMinBinCount() const;

	void setAlgorithmType(eLaiAlgorithmType algorithm_type);
	void setGroundLevelBound_mm(double ground_level_mm);
	void setVoxelSize_mm(double voxel_size_mm);
	void setMinBinCount(int min_bin_count);

	bool hasFilters() const;
	const std::vector<cPlotDataConfigFilter*>& getFilters() const;

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	eLaiAlgorithmType mAlgorithmType = eLaiAlgorithmType::OCT_TREE;

	double mGroundLevelBound_mm = 0.0;

	double mVoxelSize_mm = 1.0;

	int mMinBinCount = 1;

	cPlotDataConfigFilters mFilters;

	friend class cPlotDataConfigFile;
};


