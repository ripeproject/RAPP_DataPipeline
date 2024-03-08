
#pragma once

#include <utility>
#include <vector>
#include <string>


class cPlotBoundary
{
public:
	cPlotBoundary();
	virtual ~cPlotBoundary();

	std::pair<double, double> getNorthEastCorner();
	std::pair<double, double> getNorthWestCorner();
	std::pair<double, double> getSouthEastCorner();
	std::pair<double, double> getSouthWestCorner();

	double getEastBoundary_m();
	double getWestBoundary_m();

	double getNorthBoundary_m();
	double getSouthBoundary_m();

	bool inPlot(double southPos_m, double eastPos_m) const;

protected:
	std::string mPlotName;

	double mEastBoundary_m = 0.0;
	double mWestBoundary_m = 0.0;

	double mNorthBoundary_m = 0.0;
	double mSouthBoundary_m = 0.0;
};


class cPlotBoundaries
{
public:
	cPlotBoundaries();
	virtual ~cPlotBoundaries();

	void clear();
	bool empty() const;

	bool load(const std::string& filename);

protected:
	std::vector<cPlotBoundary> mPlotBoundaries;
};
