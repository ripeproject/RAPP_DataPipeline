
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>


class cPlotBoundary
{
public:
	cPlotBoundary();
	virtual ~cPlotBoundary();

	rfm::rappPoint2D_t getNorthEastCorner() const;
	rfm::rappPoint2D_t getNorthWestCorner() const;
	rfm::rappPoint2D_t getSouthEastCorner() const;
	rfm::rappPoint2D_t getSouthWestCorner() const;

	double getEastBoundary_m() const;
	double getWestBoundary_m() const;

	double getNorthBoundary_m() const;
	double getSouthBoundary_m() const;

	bool inPlot(rfm::rappPoint2D_t point) const;
	bool inPlot(rfm::rappPoint_t point) const;

protected:
	std::string mPlotName;

	double mEastBoundary_mm = 0.0;
	double mWestBoundary_mm = 0.0;

	double mNorthBoundary_mm = 0.0;
	double mSouthBoundary_mm = 0.0;

	rfm::rappPoint2D_t mNorthEastCorner;
	rfm::rappPoint2D_t mNorthWestCorner;
	rfm::rappPoint2D_t mSouthEastCorner;
	rfm::rappPoint2D_t mSouthWestCorner;
};


class cPlotBoundaries
{
public:
	cPlotBoundaries();
	virtual ~cPlotBoundaries();

	void clear();
	bool empty() const;

	double getEastBoundary_m();
	double getWestBoundary_m();

	double getNorthBoundary_m();
	double getSouthBoundary_m();

	bool load(const nlohmann::json& plot_info);

protected:

	double mEastBoundary_m = 0.0;
	double mWestBoundary_m = 0.0;

	double mNorthBoundary_m = 0.0;
	double mSouthBoundary_m = 0.0;

	std::vector<cPlotBoundary> mPlotBoundaries;
};
