
#pragma once

#include "datatypes.hpp"

#include "PlotDataConfigFilters.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotDataConfigHeight
{
public:
	cPlotDataConfigHeight();
	~cPlotDataConfigHeight() = default;

	void clear();

	bool isDirty() const;

	double getGroundLevelBound_mm() const;
	double getHeightPercentile() const;

	void setGroundLevelBound_mm(double ground_level_mm);
	void setHeightPercentile(double height_percentile);

	bool hasFilters() const;
	const std::vector<cPlotDataConfigFilter*>& getFilters() const;

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	double mGroundLevelBound_mm = 0.0;

	double mHeightPercentile = 95.0;

	cPlotDataConfigFilters mFilters;

	friend class cPlotDataConfigFile;
};


