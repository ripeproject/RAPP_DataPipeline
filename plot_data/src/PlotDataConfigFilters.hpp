
#pragma once

#include "datatypes.hpp"
#include "PlotDataConfigFilter.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotDataConfigFilters
{
public:
	cPlotDataConfigFilters();
	~cPlotDataConfigFilters();

	void clear();

	bool isDirty() const;

	bool isEmpty() const;
	const std::vector<cPlotDataConfigFilter*>& getFilters() const;

protected:
	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

protected:
	void setDirty(bool dirty);

private:
	std::vector<cPlotDataConfigFilter*> mFilters;

	friend class cPlotDataConfigBiomass;
	friend class cPlotDataConfigHeight;
};


class cPlotDataConfigFilter_Histogram : public cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter_Histogram() = default;
	~cPlotDataConfigFilter_Histogram() = default;

	int minBinCount() const;

protected:
	void load(const nlohmann::json& jdoc) override;
	void save(nlohmann::json& jdoc) override;

	std::vector<std::string> info() override;
	void apply(cPlotPointCloud& plot) override;

private:
	int mMinBinCount = 5;

	friend class cPlotDataConfigFile;
};


