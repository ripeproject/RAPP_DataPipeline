
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
	friend class cPlotDataConfigLAI;
};


class cPlotDataConfigFilter_TrimBelow : public cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter_TrimBelow() = default;
	virtual ~cPlotDataConfigFilter_TrimBelow() = default;

	int lowerBound_mm() const;

protected:
	void load(const nlohmann::json& jdoc) override;
	void save(nlohmann::json& jdoc) override;

	std::vector<std::string> info() override;
	void apply(cPlotPointCloud& plot) override;

private:
	int mLowerBound_mm = 0;

	friend class cPlotDataConfigFile;
};


class cPlotDataConfigFilter_TrimAbove : public cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter_TrimAbove() = default;
	virtual ~cPlotDataConfigFilter_TrimAbove() = default;

	int upperBound_mm() const;

protected:
	void load(const nlohmann::json& jdoc) override;
	void save(nlohmann::json& jdoc) override;

	std::vector<std::string> info() override;
	void apply(cPlotPointCloud& plot) override;

private:
	int mUpperBound_mm = 0;

	friend class cPlotDataConfigFile;
};


class cPlotDataConfigFilter_Histogram : public cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter_Histogram() = default;
	virtual ~cPlotDataConfigFilter_Histogram() = default;

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


class cPlotDataConfigFilter_Grubbs : public cPlotDataConfigFilter
{
public:
	cPlotDataConfigFilter_Grubbs() = default;
	virtual ~cPlotDataConfigFilter_Grubbs() = default;

	double alpha() const;

protected:
	void load(const nlohmann::json& jdoc) override;
	void save(nlohmann::json& jdoc) override;

	std::vector<std::string> info() override;
	void apply(cPlotPointCloud& plot) override;

private:
	double mAlpha = 0.05;

	friend class cPlotDataConfigFile;
};


