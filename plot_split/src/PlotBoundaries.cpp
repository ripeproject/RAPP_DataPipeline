
#include "PlotBoundaries.hpp"

#include "Constants.hpp"
#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>


cPlotBoundary::cPlotBoundary()
{}

cPlotBoundary::~cPlotBoundary()
{}

rfm::rappPoint2D_t cPlotBoundary::getNorthEastCorner() const
{
	return mNorthEastCorner;
}

rfm::rappPoint2D_t cPlotBoundary::getNorthWestCorner() const
{
	return mNorthWestCorner;
}

rfm::rappPoint2D_t cPlotBoundary::getSouthEastCorner() const
{
	return mSouthEastCorner;
}

rfm::rappPoint2D_t cPlotBoundary::getSouthWestCorner() const
{
	return mSouthWestCorner;
}

double cPlotBoundary::getEastBoundary_m() const
{
	return mEastBoundary_mm * nConstants::MM_TO_M;
}

double cPlotBoundary::getWestBoundary_m() const
{
	return mWestBoundary_mm * nConstants::MM_TO_M;
}

double cPlotBoundary::getNorthBoundary_m() const
{
	return mNorthBoundary_mm * nConstants::MM_TO_M;
}

double cPlotBoundary::getSouthBoundary_m() const
{
	return mSouthBoundary_mm * nConstants::MM_TO_M;
}

bool cPlotBoundary::inPlot(rfm::rappPoint2D_t point) const
{
	if ((point.x_mm < mNorthBoundary_mm) || (point.x_mm > mSouthBoundary_mm))
		return false;

	if ((point.y_mm > mEastBoundary_mm) || (point.y_mm < mWestBoundary_mm))
		return false;

	return true;
}

bool cPlotBoundary::inPlot(rfm::rappPoint_t point) const
{
	rfm::rappPoint2D_t p;
	p.x_mm = point.x_mm;
	p.y_mm = point.y_mm;

	return inPlot(p);
}


cPlotBoundaries::cPlotBoundaries()
{
}

cPlotBoundaries::~cPlotBoundaries()
{
}

void cPlotBoundaries::clear()
{
	mPlotBoundaries.clear();
}

bool cPlotBoundaries::empty() const
{
	return mPlotBoundaries.empty();
}

double cPlotBoundaries::getEastBoundary_m()
{
	return mEastBoundary_m;
}
double cPlotBoundaries::getWestBoundary_m()
{
	return mWestBoundary_m;
}

double cPlotBoundaries::getNorthBoundary_m()
{
	return mNorthBoundary_m;
}

double cPlotBoundaries::getSouthBoundary_m()
{
	return mSouthBoundary_m;
}

bool cPlotBoundaries::load(const nlohmann::json& plot_info)
{
	if (plot_info.contains("scans"))
	{
		auto scans = plot_info["scans"];
	}

	return true;
}
