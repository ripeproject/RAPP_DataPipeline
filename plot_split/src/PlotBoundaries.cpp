
#include "PlotBoundaries.hpp"


cPlotBoundary::cPlotBoundary()
{}

cPlotBoundary::~cPlotBoundary()
{}

std::pair<double, double> cPlotBoundary::getNorthEastCorner()
{
	return std::make_pair(mNorthBoundary_m, mEastBoundary_m);
}

std::pair<double, double> cPlotBoundary::getNorthWestCorner()
{
	return std::make_pair(mNorthBoundary_m, mWestBoundary_m);
}

std::pair<double, double> cPlotBoundary::getSouthEastCorner()
{
	return std::make_pair(mSouthBoundary_m, mEastBoundary_m);
}

std::pair<double, double> cPlotBoundary::getSouthWestCorner()
{
	return std::make_pair(mSouthBoundary_m, mWestBoundary_m);
}

double cPlotBoundary::getEastBoundary_m() 
{
	return mEastBoundary_m;
}
double cPlotBoundary::getWestBoundary_m() 
{
	return mWestBoundary_m;
}

double cPlotBoundary::getNorthBoundary_m()
{
	return mNorthBoundary_m;
}

double cPlotBoundary::getSouthBoundary_m()
{
	return mSouthBoundary_m;
}

bool cPlotBoundary::inPlot(double southPos_m, double eastPos_m) const
{
	if ((southPos_m > mSouthBoundary_m) || (southPos_m < mNorthBoundary_m))
		return false;

	if ((eastPos_m > mEastBoundary_m) || (eastPos_m < mWestBoundary_m))
		return false;

	return true;
}


cPlotBoundaries::cPlotBoundaries()
{
}

cPlotBoundaries::~cPlotBoundaries()
{
}

