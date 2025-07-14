
#include "PlotConfigBoundary.hpp"

#include "Constants.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cPlotConfigBoundary::cPlotConfigBoundary()
{
	mNorthEastCorner.x_mm = -1;
	mNorthEastCorner.y_mm = -1;

	mNorthWestCorner.x_mm = -1;
	mNorthWestCorner.y_mm = -1;

	mSouthEastCorner.x_mm = -1;
	mSouthEastCorner.y_mm = -1;

	mSouthWestCorner.x_mm = -1;
	mSouthWestCorner.y_mm = -1;
}

cPlotConfigBoundary& cPlotConfigBoundary::operator=(const cPlotConfigBoundary& rhs)
{
	setNorthEastCorner(rhs.mNorthEastCorner);
	setNorthWestCorner(rhs.mNorthWestCorner);
	setSouthEastCorner(rhs.mSouthEastCorner);
	setSouthWestCorner(rhs.mSouthWestCorner);
	setNumOfSubPlots(rhs.mNumOfSubPlots);
	setSubPlotOrientation(rhs.mSubPlotOrientation);

	return *this;
}

void cPlotConfigBoundary::clear()
{
	mDirty = false;
	mUseMeters = false;

	mNorthEastCorner.x_mm = -1;
	mNorthEastCorner.y_mm = -1;

	mNorthWestCorner.x_mm = -1;
	mNorthWestCorner.y_mm = -1;

	mSouthEastCorner.x_mm = -1;
	mSouthEastCorner.y_mm = -1;

	mSouthWestCorner.x_mm = -1;
	mSouthWestCorner.y_mm = -1;
}

bool cPlotConfigBoundary::empty() const
{
	return !mDirty && (mNorthEastCorner.x_mm <= 0) && (mNorthEastCorner.y_mm <0)
		&& (mNorthWestCorner.x_mm <= 0) && (mNorthWestCorner.y_mm <= 0)
		&& (mSouthEastCorner.x_mm <= 0) && (mSouthEastCorner.y_mm <= 0)
		&& (mSouthWestCorner.x_mm <= 0) && (mSouthWestCorner.y_mm <= 0);
}

bool cPlotConfigBoundary::isDirty() const
{
	return mDirty;
}

cPlotConfigBoundary::operator rfm::sPlotBoundingBox_t() const
{
	rfm::sPlotBoundingBox_t box;

	box.plotNumber = -1;
	box.northEastCorner = mNorthEastCorner;
	box.northWestCorner = mNorthWestCorner;
	box.southEastCorner = mSouthEastCorner;
	box.southWestCorner = mSouthWestCorner;

	return box;
}

bool cPlotConfigBoundary::operator==(const cPlotConfigBoundary& rhs) const
{
	return (mNorthEastCorner.x_mm == rhs.mNorthEastCorner.x_mm)
		&& (mNorthEastCorner.y_mm == rhs.mNorthEastCorner.y_mm)
		&& (mNorthWestCorner.x_mm == rhs.mNorthWestCorner.x_mm)
		&& (mNorthWestCorner.y_mm == rhs.mNorthWestCorner.y_mm)
		&& (mSouthEastCorner.x_mm == rhs.mSouthEastCorner.x_mm)
		&& (mSouthEastCorner.y_mm == rhs.mSouthEastCorner.y_mm)
		&& (mSouthWestCorner.x_mm == rhs.mSouthWestCorner.x_mm)
		&& (mSouthWestCorner.y_mm == rhs.mSouthWestCorner.y_mm);
}

bool cPlotConfigBoundary::operator!=(const cPlotConfigBoundary& rhs) const
{
	return !operator==(rhs);
}

rfm::sPlotBoundingBox_t cPlotConfigBoundary::getBoundingBox() const
{
	rfm::sPlotBoundingBox_t box;
	box.northWestCorner = mNorthWestCorner;
	box.northEastCorner = mNorthEastCorner;
	box.southWestCorner = mSouthWestCorner;
	box.southEastCorner = mSouthEastCorner;
	return box;
}

rfm::rappPoint2D_t cPlotConfigBoundary::getNorthEastCorner() const
{
	return mNorthEastCorner;
}

rfm::rappPoint2D_t cPlotConfigBoundary::getNorthWestCorner() const
{
	return mNorthWestCorner;
}

rfm::rappPoint2D_t cPlotConfigBoundary::getSouthEastCorner() const
{
	return mSouthEastCorner;
}

rfm::rappPoint2D_t cPlotConfigBoundary::getSouthWestCorner() const
{
	return mSouthWestCorner;
}

bool cPlotConfigBoundary::hasSubPlots() const
{
	return mNumOfSubPlots > 0;
}

uint8_t cPlotConfigBoundary::getNumOfSubPlots() const
{
	return mNumOfSubPlots;
}

ePlotOrientation cPlotConfigBoundary::getSubPlotOrientation() const
{
	return mSubPlotOrientation;
}

rfm::rappPoint2D_t cPlotConfigBoundary::center() const
{
	double x = (((mSouthEastCorner.x_mm + mNorthEastCorner.x_mm) / 2.0) + ((mSouthWestCorner.x_mm + mNorthWestCorner.x_mm) / 2.0)) / 2.0;
	double y = (((mNorthEastCorner.y_mm + mNorthWestCorner.y_mm) / 2.0) + ((mSouthEastCorner.y_mm + mSouthWestCorner.y_mm) / 2.0)) / 2.0;

	return {static_cast<std::int32_t>(x), static_cast<std::int32_t>(y)};
}

bool cPlotConfigBoundary::contains(rfm::rappPoint2D_t point) const
{
	return contains(point.x_mm, point.y_mm);
}

bool cPlotConfigBoundary::contains(std::int32_t x_mm, std::int32_t y_mm) const
{
	if (x_mm < mNorthEastCorner.x_mm) return false;
	if (x_mm < mNorthWestCorner.x_mm) return false;

	if (x_mm > mSouthEastCorner.x_mm) return false;
	if (x_mm > mSouthWestCorner.x_mm) return false;

	if (y_mm < mNorthWestCorner.y_mm) return false;
	if (y_mm < mSouthWestCorner.y_mm) return false;

	if (y_mm > mNorthEastCorner.y_mm) return false;
	if (y_mm > mSouthEastCorner.y_mm) return false;

	return true;
}


void cPlotConfigBoundary::setNorthEastCorner(rfm::rappPoint2D_t point)
{
	setNorthEastCorner(point.x_mm, point.y_mm);
}

void cPlotConfigBoundary::setNorthWestCorner(rfm::rappPoint2D_t point)
{
	setNorthWestCorner(point.x_mm, point.y_mm);
}

void cPlotConfigBoundary::setSouthEastCorner(rfm::rappPoint2D_t point)
{
	setSouthEastCorner(point.x_mm, point.y_mm);
}

void cPlotConfigBoundary::setSouthWestCorner(rfm::rappPoint2D_t point)
{
	setSouthWestCorner(point.x_mm, point.y_mm);
}

void cPlotConfigBoundary::setNorthEastCorner(std::int32_t x_mm, std::int32_t y_mm)
{
	mDirty |= (mNorthEastCorner.x_mm != x_mm) || (mNorthEastCorner.y_mm != y_mm);
	mNorthEastCorner.x_mm = x_mm;
	mNorthEastCorner.y_mm = y_mm;
}

void cPlotConfigBoundary::setNorthWestCorner(std::int32_t x_mm, std::int32_t y_mm)
{
	mDirty |= (mNorthWestCorner.x_mm != x_mm) || (mNorthWestCorner.y_mm != y_mm);
	mNorthWestCorner.x_mm = x_mm;
	mNorthWestCorner.y_mm = y_mm;
}

void cPlotConfigBoundary::setSouthEastCorner(std::int32_t x_mm, std::int32_t y_mm)
{
	mDirty |= (mSouthEastCorner.x_mm != x_mm) || (mSouthEastCorner.y_mm != y_mm);
	mSouthEastCorner.x_mm = x_mm;
	mSouthEastCorner.y_mm = y_mm;
}

void cPlotConfigBoundary::setSouthWestCorner(std::int32_t x_mm, std::int32_t y_mm)
{
	mDirty |= (mSouthWestCorner.x_mm != x_mm) || (mSouthWestCorner.y_mm != y_mm);
	mSouthWestCorner.x_mm = x_mm;
	mSouthWestCorner.y_mm = y_mm;
}

void cPlotConfigBoundary::setNumOfSubPlots(uint8_t num)
{
	mDirty |= (mNumOfSubPlots != num);
	mNumOfSubPlots = num;
}

void cPlotConfigBoundary::setSubPlotOrientation(ePlotOrientation orientation)
{
	mDirty |= (mSubPlotOrientation != orientation);
	mSubPlotOrientation = orientation;
}

void cPlotConfigBoundary::clearDirtyFlag()
{
	mDirty = false;
}

void cPlotConfigBoundary::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
}

void cPlotConfigBoundary::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("sub plots"))
	{
		auto sub_plots = jdoc["sub plots"];

		mNumOfSubPlots = sub_plots["number"];
		std::string orientation = sub_plots["orientation"];

		if (orientation.find("east") != std::string::npos)
		{
			mSubPlotOrientation = ePlotOrientation::eEAST_TO_WEST;
		}
		else if (orientation.find("north") != std::string::npos)
		{
			mSubPlotOrientation = ePlotOrientation::eNORTH_TO_SOUTH;
		}
		else
		{
			mSubPlotOrientation = ePlotOrientation::eNORTH_TO_SOUTH;
		}
	}

	if (!jdoc.contains("corners"))
		return;
		
	auto corners = jdoc["corners"];

	if (corners.size() != 4)
	{
		return;
	}

	std::array<rfm::rappPoint2D_t, 4> points;

	int i = 0;
	for (auto corner : corners)
	{
		rfm::rappPoint2D_t p;

		if (corner.contains("x (m)"))
		{
			p.x_mm = corner["x (m)"].get<double>() * nConstants::M_TO_MM;
			mUseMeters = true;
		}
		else
			p.x_mm = corner["x (mm)"];

		if (corner.contains("y (m)"))
		{
			p.y_mm = corner["y (m)"].get<double>() * nConstants::M_TO_MM;
			mUseMeters = true;
		}
		else
			p.y_mm = corner["y (mm)"];

		points[i++] = p;
	}

	std::sort(points.begin(), points.end(), [](auto a, auto b) { return a.x_mm < b.x_mm; });

	if (points[1].y_mm < points[0].y_mm)
		std::swap(points[0], points[1]);

	mNorthWestCorner = points[0];
	mNorthEastCorner = points[1];

	if (points[3].y_mm < points[2].y_mm)
		std::swap(points[2], points[3]);

	mSouthWestCorner = points[2];
	mSouthEastCorner = points[3];
}

void cPlotConfigBoundary::save(nlohmann::json& jdoc)
{
	if (mNumOfSubPlots > 1)
	{
		nlohmann::json sub_plots;

		sub_plots["number"] = mNumOfSubPlots;

		if ((mSubPlotOrientation == ePlotOrientation::eEAST_TO_WEST)
			|| (mSubPlotOrientation == ePlotOrientation::eWEST_TO_EAST))
		{
			sub_plots["orientation"] = "east/west";
		}
		else
		{
			sub_plots["orientation"] = "north/south";
		}

		jdoc["sub plots"] = sub_plots;
	}

	nlohmann::json corners;

	if (mUseMeters)
	{
		nlohmann::json corner;
		corner["x (m)"] = mNorthEastCorner.x_mm * nConstants::MM_TO_M;
		corner["y (m)"] = mNorthEastCorner.y_mm * nConstants::MM_TO_M;

		corners.emplace_back(corner);
		corner.clear();

		corner["x (m)"] = mNorthWestCorner.x_mm * nConstants::MM_TO_M;
		corner["y (m)"] = mNorthWestCorner.y_mm * nConstants::MM_TO_M;

		corners.emplace_back(corner);
		corner.clear();

		corner["x (m)"] = mSouthWestCorner.x_mm * nConstants::MM_TO_M;
		corner["y (m)"] = mSouthWestCorner.y_mm * nConstants::MM_TO_M;

		corners.emplace_back(corner);
		corner.clear();

		corner["x (m)"] = mSouthEastCorner.x_mm * nConstants::MM_TO_M;
		corner["y (m)"] = mSouthEastCorner.y_mm * nConstants::MM_TO_M;

		corners.emplace_back(corner);
	}
	else
	{
		nlohmann::json corner;
		corner["x (mm)"] = mNorthEastCorner.x_mm;
		corner["y (mm)"] = mNorthEastCorner.y_mm;

		corners.emplace_back(corner);
		corner.clear();

		corner["x (mm)"] = mNorthWestCorner.x_mm;
		corner["y (mm)"] = mNorthWestCorner.y_mm;

		corners.emplace_back(corner);
		corner.clear();

		corner["x (mm)"] = mSouthWestCorner.x_mm;
		corner["y (mm)"] = mSouthWestCorner.y_mm;

		corners.emplace_back(corner);
		corner.clear();

		corner["x (mm)"] = mSouthEastCorner.x_mm;
		corner["y (mm)"] = mSouthEastCorner.y_mm;

		corners.emplace_back(corner);
	}

	mDirty = false;

	jdoc["corners"] = corners;
}


