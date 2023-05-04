
#pragma once

#include <utility>


class cPlotBoundaries
{
public:
	cPlotBoundaries();
	virtual ~cPlotBoundaries();

	std::pair<double, double> getNorthEastCorner();
	std::pair<double, double> getNorthWestCorner();
	std::pair<double, double> getSouthEastCorner();
	std::pair<double, double> getSouthWestCorner();

	double getEastBoundary_m();
	double getWestBoundary_m();

	double getNorthBoundary_m();
	double getSouthBoundary_m();

protected:
	double mEastBoundary_m = 0.0;
	double mWestBoundary_m = 0.0;

	double mNorthBoundary_m = 0.0;
	double mSouthBoundary_m = 0.0;
};
