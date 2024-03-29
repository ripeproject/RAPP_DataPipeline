
#include "PlotBoundaries.hpp"

#include "Constants.hpp"
#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cPlotBoundary::cPlotBoundary()
{}

bool cPlotBoundary::load(const nlohmann::json& plot_info)
{
	mPlotNumber = plot_info["number"];

	if (plot_info.contains("name"))
		mPlotName = plot_info["name"];
	//else
	//{
	//	mPlotName = "Plot ";
	//	mPlotName += std::to_string(mPlotNumber);
	//}

	if (plot_info.contains("event"))
		mEvent = plot_info["event"];

	if (plot_info.contains("description"))
		mDescription = plot_info["description"];

	auto corners = plot_info["corners"];

	if (corners.size() != 4)
	{
		return false;
	}

	std::array<rfm::rappPoint2D_t, 4> points;

	int i = 0;
	for (auto corner : corners)
	{
		rfm::rappPoint2D_t p;

		if (corner.contains("x (m)"))
		{
			p.x_mm = corner["x (m)"].get<double>() * nConstants::M_TO_MM;
		}
		else
			p.x_mm = corner["x (mm)"];

		if (corner.contains("y (m)"))
		{
			p.y_mm = corner["y (m)"].get<double>() * nConstants::M_TO_MM;
		}
		else
			p.y_mm = corner["y (mm)"];

		points[i++] = p;
	}

	initialize(points);

	return true;
}

void cPlotBoundary::initialize(std::array<rfm::rappPoint2D_t, 4> points)
{
	std::sort(points.begin(), points.end(), [](auto a, auto b) { return a.x_mm < b.x_mm; });

	mNorthBoundary_mm = points[0].x_mm;
	mSouthBoundary_mm = points[3].x_mm;

	if (points[1].y_mm < points[0].y_mm)
		std::swap(points[0], points[1]);

	mNorthWestCorner = points[0];
	mNorthEastCorner = points[1];

	if (points[3].y_mm < points[2].y_mm)
		std::swap(points[2], points[3]);

	mSouthWestCorner = points[2];
	mSouthEastCorner = points[3];

	std::sort(points.begin(), points.end(), [](auto a, auto b) { return a.y_mm < b.y_mm; });

	mWestBoundary_mm = points[0].y_mm;
	mEastBoundary_mm = points[3].y_mm;

	{
		double dx = mNorthEastCorner.y_mm - mNorthWestCorner.y_mm;
		double dy = mNorthEastCorner.x_mm - mNorthWestCorner.x_mm;

		mNorthSide.m = dy / dx;
		mNorthSide.b = mNorthEastCorner.x_mm - mNorthSide.m * mNorthEastCorner.y_mm;
	}

	{
		double dx = mSouthEastCorner.y_mm - mSouthWestCorner.y_mm;
		double dy = mSouthEastCorner.x_mm - mSouthWestCorner.x_mm;

		mSouthSide.m = dy / dx;
		mSouthSide.b = mSouthEastCorner.x_mm - mSouthSide.m * mSouthEastCorner.y_mm;
	}

	{
		double dx = mSouthWestCorner.x_mm - mNorthWestCorner.x_mm;
		double dy = mSouthWestCorner.y_mm - mNorthWestCorner.y_mm;

		mWestSide.m = dy / dx;
		mWestSide.b = mSouthWestCorner.y_mm - mWestSide.m * mSouthWestCorner.x_mm;
	}

	{
		double dx = mSouthEastCorner.x_mm - mNorthEastCorner.x_mm;
		double dy = mSouthEastCorner.y_mm - mNorthEastCorner.y_mm;

		mEastSide.m = dy / dx;
		mEastSide.b = mSouthEastCorner.y_mm - mEastSide.m * mSouthEastCorner.x_mm;
	}
}

int32_t cPlotBoundary::getPlotNumber() const
{
	return mPlotNumber;
}

const std::string& cPlotBoundary::getPlotName() const
{
	return mPlotName;
}

const std::string& cPlotBoundary::getEvent() const
{
	return mEvent;
}

const std::string& cPlotBoundary::getDescription() const
{
	return mDescription;
}

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


pointcloud::sBoundingBox_t cPlotBoundary::getBoundingBox() const
{
	pointcloud::sBoundingBox_t box;

	box.points[0] = { getNorthBoundary_m(), getWestBoundary_m()};
	box.points[1] = { getSouthBoundary_m(), getWestBoundary_m()};
	box.points[2] = { getSouthBoundary_m(), getEastBoundary_m()};
	box.points[3] = { getNorthBoundary_m(), getEastBoundary_m()};

	return box;
}

pointcloud::sBoundingBox_t cPlotBoundary::getPlotBounds() const
{
	pointcloud::sBoundingBox_t box;

	box.points[0] = { mNorthWestCorner.x_mm * nConstants::MM_TO_M, mNorthWestCorner.y_mm * nConstants::MM_TO_M };
	box.points[1] = { mSouthWestCorner.x_mm * nConstants::MM_TO_M, mSouthWestCorner.y_mm * nConstants::MM_TO_M };
	box.points[2] = { mSouthEastCorner.x_mm * nConstants::MM_TO_M, mSouthEastCorner.y_mm * nConstants::MM_TO_M };
	box.points[3] = { mNorthEastCorner.x_mm * nConstants::MM_TO_M, mNorthEastCorner.y_mm * nConstants::MM_TO_M };

	return box;
}

bool cPlotBoundary::inPlot(rfm::rappPoint2D_t point) const
{
	if ((point.x_mm < mNorthBoundary_mm) || (point.x_mm > mSouthBoundary_mm))
		return false;

	if ((point.y_mm > mEastBoundary_mm) || (point.y_mm < mWestBoundary_mm))
		return false;

	{
		double x = mNorthSide.m * point.y_mm + mNorthSide.b;
		if (point.x_mm < x)
			return false;
	}

	{
		double x = mSouthSide.m * point.y_mm + mSouthSide.b;
		if (point.x_mm > x)
			return false;
	}

	{
		double y = mWestSide.m * point.x_mm + mWestSide.b;
		if (point.y_mm < y)
			return false;
	}

	{
		double y = mEastSide.m * point.x_mm + mEastSide.b;
		if (point.y_mm > y)
			return false;
	}

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
{}

cPlotBoundaries::~cPlotBoundaries()
{
	for (auto plot : mPlotBoundaries)
		delete plot;

	mPlotBoundaries.clear();
}

void cPlotBoundaries::clear()
{
	for (auto plot : mPlotBoundaries)
		delete plot;

	mPlotBoundaries.clear();
}

bool cPlotBoundaries::empty() const
{
	return mPlotBoundaries.empty();
}

std::size_t cPlotBoundaries::size() const
{
	return mPlotBoundaries.size();
}

const std::vector<cPlotBoundary*>& cPlotBoundaries::getPlots() const
{
	return mPlotBoundaries;
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

pointcloud::sBoundingBox_t cPlotBoundaries::getBoundingBox() const
{
	pointcloud::sBoundingBox_t box;

	box.points[0] = { mNorthBoundary_m, mWestBoundary_m };
	box.points[1] = { mSouthBoundary_m, mWestBoundary_m };
	box.points[2] = { mSouthBoundary_m, mEastBoundary_m };
	box.points[3] = { mNorthBoundary_m, mEastBoundary_m };

	return box;
}

bool cPlotBoundaries::load(const nlohmann::json& plot_info)
{
	if (!plot_info.contains("plots"))
	{
		return false;
	}

	auto plots = plot_info["plots"];
	for (auto plot : plots)
	{
		cPlotBoundary* pPlot = new cPlotBoundary();

		if (pPlot->load(plot))
			mPlotBoundaries.push_back(pPlot);
	}

	if (mPlotBoundaries.empty())
		return false;

	mEastBoundary_m = mPlotBoundaries[0]->getEastBoundary_m();
	mWestBoundary_m = mPlotBoundaries[0]->getWestBoundary_m();
	mNorthBoundary_m = mPlotBoundaries[0]->getNorthBoundary_m();
	mSouthBoundary_m = mPlotBoundaries[0]->getSouthBoundary_m();

	for (int i = 1; i < mPlotBoundaries.size(); ++i)
	{
		if (mPlotBoundaries[i]->getNorthBoundary_m() < mNorthBoundary_m)
			mNorthBoundary_m = mPlotBoundaries[i]->getNorthBoundary_m();

		if (mPlotBoundaries[i]->getWestBoundary_m() < mWestBoundary_m)
			mWestBoundary_m = mPlotBoundaries[i]->getWestBoundary_m();

		if (mPlotBoundaries[i]->getSouthBoundary_m() > mSouthBoundary_m)
			mSouthBoundary_m = mPlotBoundaries[i]->getSouthBoundary_m();

		if (mPlotBoundaries[i]->getEastBoundary_m() > mEastBoundary_m)
			mEastBoundary_m = mPlotBoundaries[i]->getEastBoundary_m();
	}

	return true;
}
