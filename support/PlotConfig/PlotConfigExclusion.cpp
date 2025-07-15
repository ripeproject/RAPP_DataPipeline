
#include "PlotConfigExclusion.hpp"

#include "Constants.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cPlotConfigExclusion::cPlotConfigExclusion()
{
}

void cPlotConfigExclusion::clear()
{
	mDirty = false;
	
	mCenterX_mm = 0;
	mCenterY_mm = 0;

	mLength_mm = 0;
	mWidth_mm = 0;

	mOrientation_deg = 0;
}

bool cPlotConfigExclusion::empty() const
{
	return !mDirty && (mLength_mm == 0) && (mWidth_mm == 0);
}

bool cPlotConfigExclusion::isDirty() const
{
	return mDirty;
}

cPlotConfigExclusion::operator rfm::sPlotBoundingBox_t() const
{
	rfm::sPlotBoundingBox_t box;

	box.plotNumber = -1;

	return box;
}

bool cPlotConfigExclusion::operator==(const cPlotConfigExclusion& rhs) const
{
	return (mCenterX_mm == rhs.mCenterX_mm)
		&& (mCenterY_mm == rhs.mCenterY_mm)
		&& (mLength_mm == rhs.mLength_mm)
		&& (mWidth_mm == rhs.mWidth_mm)
		&& (mOrientation_deg == rhs.mOrientation_deg);
}

bool cPlotConfigExclusion::operator!=(const cPlotConfigExclusion& rhs) const
{
	return !operator==(rhs);
}

rfm::sPlotBoundingBox_t cPlotConfigExclusion::getBoundingBox() const
{
	rfm::sPlotBoundingBox_t box;
	return box;
}

rfm::rappPoint2D_t cPlotConfigExclusion::center() const
{
	return { static_cast<std::int32_t>(mCenterX_mm), static_cast<std::int32_t>(mCenterY_mm)};
}

bool cPlotConfigExclusion::contains(rfm::rappPoint2D_t point) const
{
	return contains(point.x_mm, point.y_mm);
}

bool cPlotConfigExclusion::contains(std::int32_t x_mm, std::int32_t y_mm) const
{
	return true;
}

void cPlotConfigExclusion::setCenterX_mm(uint32_t x_mm)
{
	mDirty |= mCenterX_mm != x_mm;
	mCenterX_mm = x_mm;
}

void cPlotConfigExclusion::setCenterY_mm(uint32_t y_mm)
{
	mDirty |= mCenterY_mm != y_mm;
	mCenterY_mm = y_mm;
}

void cPlotConfigExclusion::setCenter(uint32_t x_mm, uint32_t y_mm)
{
	setCenterX_mm(x_mm);
	setCenterY_mm(y_mm);
}

void cPlotConfigExclusion::setLength_mm(uint32_t length_mm)
{
	mDirty |= mLength_mm != length_mm;
	mLength_mm = length_mm;
}

void cPlotConfigExclusion::setWidth_mm(uint32_t width_mm)
{
	mDirty |= mWidth_mm != width_mm;
	mWidth_mm = width_mm;
}

void cPlotConfigExclusion::setOrientation_deg(double angle_deg)
{
	mDirty |= mOrientation_deg != angle_deg;
	mOrientation_deg = angle_deg;
}


void cPlotConfigExclusion::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
}

void cPlotConfigExclusion::load(const nlohmann::json& jdoc)
{
	auto center = jdoc["center"];
	mCenterX_mm = center["x (mm)"];
	mCenterY_mm = center["y (mm)"];

	mLength_mm = jdoc["length (mm)"];
	mWidth_mm = jdoc["width (mm)"];

	mOrientation_deg = jdoc["orientation (deg)"];
	mDirty = false;
}

nlohmann::json cPlotConfigExclusion::save()
{
	nlohmann::json jdoc;

	nlohmann::json center;
	center["x (mm)"] = mCenterX_mm;
	center["y (mm)"] = mCenterY_mm;

	jdoc["center"] = center;

	jdoc["length (mm)"] = mLength_mm;
	jdoc["width (mm)"] = mWidth_mm;

	jdoc["orientation (deg)"] = mOrientation_deg;
	mDirty = false;

	return jdoc;
}


