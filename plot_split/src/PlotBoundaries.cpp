
#include "PlotBoundaries.hpp"


cPlotBoundaries::cPlotBoundaries()
{}

cPlotBoundaries::~cPlotBoundaries()
{}

std::pair<double, double> cPlotBoundaries::getNorthEastCorner()
{
	return std::make_pair(mNorthBoundary_m, mEastBoundary_m);
}

std::pair<double, double> cPlotBoundaries::getNorthWestCorner()
{
	return std::make_pair(mNorthBoundary_m, mWestBoundary_m);
}

std::pair<double, double> cPlotBoundaries::getSouthEastCorner()
{
	return std::make_pair(mSouthBoundary_m, mEastBoundary_m);
}

std::pair<double, double> cPlotBoundaries::getSouthWestCorner()
{
	return std::make_pair(mSouthBoundary_m, mWestBoundary_m);
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

