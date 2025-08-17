
#include "PlotConfigInclusion.hpp"

#include "Constants.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cPlotConfigInclusion::cPlotConfigInclusion()
{
}

void cPlotConfigInclusion::clear()
{
	mDirty = false;
	
	mCenterX_mm = 0;
	mCenterY_mm = 0;

	mRadius_mm = 0;
}

bool cPlotConfigInclusion::empty() const
{
	return !mDirty && (mRadius_mm == 0);
}

bool cPlotConfigInclusion::isDirty() const
{
	return mDirty;
}

cPlotConfigInclusion::operator rfm::sPlotBoundingBox_t() const
{
	rfm::sPlotBoundingBox_t box;

	box.plotNumber = -1;

	return box;
}

bool cPlotConfigInclusion::operator==(const cPlotConfigInclusion& rhs) const
{
	return (mCenterX_mm == rhs.mCenterX_mm)
		&& (mCenterY_mm == rhs.mCenterY_mm)
		&& (mRadius_mm == rhs.mRadius_mm);
}

bool cPlotConfigInclusion::operator!=(const cPlotConfigInclusion& rhs) const
{
	return !operator==(rhs);
}

rfm::sPlotBoundingBox_t cPlotConfigInclusion::getBoundingBox() const
{
	rfm::sPlotBoundingBox_t box;
	return box;
}

rfm::rappPoint2D_t cPlotConfigInclusion::center() const
{
	return { static_cast<std::int32_t>(mCenterX_mm), static_cast<std::int32_t>(mCenterY_mm)};
}

bool cPlotConfigInclusion::contains(rfm::rappPoint2D_t point) const
{
	return contains(point.x_mm, point.y_mm);
}

bool cPlotConfigInclusion::contains(std::int32_t x_mm, std::int32_t y_mm) const
{
	return true;
}

void cPlotConfigInclusion::setCenterX_mm(uint32_t x_mm)
{
	mDirty |= mCenterX_mm != x_mm;
	mCenterX_mm = x_mm;
}

void cPlotConfigInclusion::setCenterY_mm(uint32_t y_mm)
{
	mDirty |= mCenterY_mm != y_mm;
	mCenterY_mm = y_mm;
}

void cPlotConfigInclusion::setCenter(uint32_t x_mm, uint32_t y_mm)
{
	setCenterX_mm(x_mm);
	setCenterY_mm(y_mm);
}

void cPlotConfigInclusion::setRadius_mm(uint32_t radius_mm)
{
	mDirty |= mRadius_mm != radius_mm;
	mRadius_mm = radius_mm;
}

void cPlotConfigInclusion::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
}

void cPlotConfigInclusion::load(const nlohmann::json& jdoc)
{
	auto center = jdoc["center"];
	mCenterX_mm = center["x (mm)"];
	mCenterY_mm = center["y (mm)"];

	mRadius_mm = jdoc["radius (mm)"];

	mDirty = false;
}

nlohmann::json cPlotConfigInclusion::save()
{
	nlohmann::json jdoc;

	nlohmann::json center;
	center["x (mm)"] = mCenterX_mm;
	center["y (mm)"] = mCenterY_mm;

	jdoc["center"] = center;

	jdoc["radius (mm)"] = mRadius_mm;

	mDirty = false;

	return jdoc;
}


