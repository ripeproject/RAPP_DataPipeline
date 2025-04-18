
#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

// Forward Declarations
class cPlotPointCloud;


class cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter() = default;
	virtual ~cPlotDataConfigFilter() = default;

	bool isDirty() const;

	virtual std::vector<std::string> info() = 0;

	virtual void apply(cPlotPointCloud& plot) = 0;

protected:
	void setDirty(bool dirty);

	virtual void load(const nlohmann::json& jdoc) = 0;
	virtual void save(nlohmann::json& jdoc) = 0;

protected:
	bool mDirty = false;

	friend class cPlotDataConfigFile;
	friend class cPlotDataConfigFilters;
};


