
#pragma once

#include "datatypes.hpp"

#include "PlotDataConfigFilters.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotDataConfigBiomass
{
public:
	cPlotDataConfigBiomass();
	~cPlotDataConfigBiomass() = default;

	void clear();

	bool isDirty() const;

	double getGroundLevelBound_mm() const;
	double getVoxelSize_mm() const;

	void setGroundLevelBound_mm(double ground_level_mm);
	void setVoxelSize_mm(double voxel_size_mm);

	bool hasFilters() const;
	const std::vector<cPlotDataConfigFilter*>& getFilters() const;

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	double mGroundLevelBound_mm = 0.0;

	double mVoxelSize_mm = 1.0;

	cPlotDataConfigFilters mFilters;

	friend class cPlotDataConfigFile;
};


