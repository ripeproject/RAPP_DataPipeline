
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>

// Forward Declarations
class cPlotDataConfigFilter;


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


class cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter() = default;
	~cPlotDataConfigFilter() = default;

	bool isDirty() const;

protected:
	void setDirty(bool dirty);

	virtual void load(const nlohmann::json& jdoc) = 0;
	virtual void save(nlohmann::json& jdoc) = 0;

protected:
	bool mDirty = false;

	friend class cPlotDataConfigFile;
	friend class cPlotDataConfigFilters;
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

private:
	int mMinBinCount = 5;

	friend class cPlotDataConfigFile;
};


