
#include "PlotDataConfigFilters.hpp"
#include "PlotDataUtils.hpp"
#include "PlotSplitUtils.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


const char* FILTERS_SECTION_NAME = "filters";


cPlotDataConfigFilters::cPlotDataConfigFilters()
{}

cPlotDataConfigFilters::~cPlotDataConfigFilters()
{
	clear();
}

void cPlotDataConfigFilters::clear()
{
	for (auto* filter : mFilters)
	{
		delete filter;
	}

	mFilters.clear();
}

bool cPlotDataConfigFilters::isDirty() const
{
	bool dirty = false;

	for (auto* filter : mFilters)
	{
		dirty |= filter->isDirty();
	}

	return dirty;
}

bool cPlotDataConfigFilters::isEmpty() const
{
	return mFilters.empty();
}

const std::vector<cPlotDataConfigFilter*>& cPlotDataConfigFilters::getFilters() const
{
	return mFilters;
}

void cPlotDataConfigFilters::setDirty(bool dirty)
{
	for (auto* filter : mFilters)
	{
		filter->setDirty(dirty);
	}
}

void cPlotDataConfigFilters::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains(FILTERS_SECTION_NAME))
		return;
		
	auto filters = jdoc[FILTERS_SECTION_NAME];

	for (auto filter : filters)
	{
		std::string type = filter["type"];

		cPlotDataConfigFilter* pFilter = nullptr;

		if ((type == "trim_below") || (type == "trim below"))
		{
			pFilter = new cPlotDataConfigFilter_TrimBelow();
		}
		else if ((type == "trim_above") || (type == "trim above"))
		{
			pFilter = new cPlotDataConfigFilter_TrimAbove();
		}
		else if (type == "histogram")
		{
			pFilter = new cPlotDataConfigFilter_Histogram();
		}
		else if (type == "grubbs")
		{
			pFilter = new cPlotDataConfigFilter_Grubbs();
		}

		if (pFilter)
		{
			pFilter->load(filter);
			mFilters.push_back(pFilter);
		}
	}
}

void cPlotDataConfigFilters::save(nlohmann::json& jdoc)
{
	nlohmann::json filters;

	jdoc[FILTERS_SECTION_NAME] = filters;
}

/**
 * Different Types of Plot Data Config Filters
 **/

bool cPlotDataConfigFilter::isDirty() const
{
	return mDirty;
}

void cPlotDataConfigFilter::setDirty(bool dirty)
{
	mDirty = dirty;
}


/*
 *  Trim Below: removes all points below a certain level
 */

int cPlotDataConfigFilter_TrimBelow::lowerBound_mm() const
{
	return mLowerBound_mm;
}

void cPlotDataConfigFilter_TrimBelow::load(const nlohmann::json& jdoc)
{
	mLowerBound_mm = jdoc["lower_bound_mm"];
}

void cPlotDataConfigFilter_TrimBelow::save(nlohmann::json& jdoc)
{
}

std::vector<std::string> cPlotDataConfigFilter_TrimBelow::info()
{
	std::vector<std::string> info;

	info.emplace_back("filter type: trim below");
	info.emplace_back("lower bound (mm): " + std::to_string(mLowerBound_mm));

	return info;
}

void cPlotDataConfigFilter_TrimBelow::apply(cPlotPointCloud& plot)
{
	plot = plot::trim_below(plot, mLowerBound_mm);
}


 /*
  *  Trim Above: removes all points above a certain level
  */

int cPlotDataConfigFilter_TrimAbove::upperBound_mm() const
{
	return mUpperBound_mm;
}

void cPlotDataConfigFilter_TrimAbove::load(const nlohmann::json& jdoc)
{
	mUpperBound_mm = jdoc["upper_bound_mm"];
}

void cPlotDataConfigFilter_TrimAbove::save(nlohmann::json& jdoc)
{
}

std::vector<std::string> cPlotDataConfigFilter_TrimAbove::info()
{
	std::vector<std::string> info;

	info.emplace_back("filter type: upper bound");
	info.emplace_back("upper bound (mm): " + std::to_string(mUpperBound_mm));

	return info;
}

void cPlotDataConfigFilter_TrimAbove::apply(cPlotPointCloud& plot)
{
	plot = plot::trim_above(plot, mUpperBound_mm);
}


/*
 *  Simple histogram filter
 */

int cPlotDataConfigFilter_Histogram::minBinCount() const
{
	return mMinBinCount;
}

void cPlotDataConfigFilter_Histogram::load(const nlohmann::json& jdoc)
{
	mMinBinCount = jdoc["min_bin_count"];
}

void cPlotDataConfigFilter_Histogram::save(nlohmann::json& jdoc)
{}

std::vector<std::string> cPlotDataConfigFilter_Histogram::info()
{
	std::vector<std::string> info;

	info.emplace_back("filter type: outlier removal");
	info.emplace_back("algorithm: histogram");
	info.emplace_back("min bin count: " + std::to_string(mMinBinCount));

	return info;
}

void cPlotDataConfigFilter_Histogram::apply(cPlotPointCloud& plot)
{
	nPlotUtils::removeHeightOutliers_Histogram(plot, mMinBinCount);
}


/*
 *  Smirnov Grubbs filter
 */
double cPlotDataConfigFilter_Grubbs::alpha() const
{
	return mAlpha;
}


void cPlotDataConfigFilter_Grubbs::load(const nlohmann::json& jdoc)
{
	mAlpha = jdoc["alpha"];
}

void cPlotDataConfigFilter_Grubbs::save(nlohmann::json& jdoc)
{
}

std::vector<std::string> cPlotDataConfigFilter_Grubbs::info()
{
	std::vector<std::string> info;

	info.emplace_back("filter type: outlier removal");
	info.emplace_back("algorithm: smirnov grubbs");
	info.emplace_back("alpha: " + std::to_string(mAlpha));

	return info;
}

void cPlotDataConfigFilter_Grubbs::apply(cPlotPointCloud& plot)
{
	nPlotUtils::removeHeightOutliers_Grubbs(plot, mAlpha);
}


