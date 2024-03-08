
#include "PlotBoundaries.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>


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

void cPlotBoundaries::clear()
{
	mPlotBoundaries.clear();
}

bool cPlotBoundaries::empty() const
{
	return mPlotBoundaries.empty();
}

bool cPlotBoundaries::load(const std::string& filename)
{
	std::ifstream in;
	in.open(filename);
	if (!in.is_open())
		return false;

	nlohmann::json configDoc;
	try
	{
		in >> configDoc;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		auto msg = e.what();
		return false;
	}
	catch (const std::exception& e)
	{
		return false;
	}

	if (configDoc.contains("scans"))
	{
		auto scans = configDoc["scans"];
	}

}
