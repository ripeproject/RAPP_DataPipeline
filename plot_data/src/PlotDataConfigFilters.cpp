
#include "PlotDataConfigFilters.hpp"
#include "PlotDataUtils.hpp"

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

		if (type == "histogram")
		{
			pFilter = new cPlotDataConfigFilter_Histogram();

			pFilter->load(filter);
		}

		if (pFilter)
			mFilters.push_back(pFilter);
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

void cPlotDataConfigFilter_Histogram::apply(cPlotPointCloud& plot)
{
	nPlotUtils::removeHeightOutliers_Histogram(plot, mMinBinCount);
}



