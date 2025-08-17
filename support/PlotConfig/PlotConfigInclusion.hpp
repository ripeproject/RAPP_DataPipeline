
#pragma once

#include "datatypes.hpp"

#include "PlotSplitDataTypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotConfigInclusion
{
public:
	cPlotConfigInclusion();
	~cPlotConfigInclusion() = default;

	void clear();

	bool empty() const;

	bool isDirty() const;

	rfm::sPlotBoundingBox_t getBoundingBox() const;

	rfm::rappPoint2D_t center() const;

	bool contains(rfm::rappPoint2D_t point) const;
	bool contains(std::int32_t x_mm, std::int32_t y_mm) const;

	operator rfm::sPlotBoundingBox_t() const;
	bool operator==(const cPlotConfigInclusion& rhs) const;
	bool operator!=(const cPlotConfigInclusion& rhs) const;

	void setCenterX_mm(uint32_t x_mm);
	void setCenterY_mm(uint32_t y_mm);
	void setCenter(uint32_t x_mm, uint32_t y_mm);

	void setRadius_mm(uint32_t radius_mm);

	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	uint32_t mCenterX_mm = 0;
	uint32_t mCenterY_mm = 0;

	uint32_t mRadius_mm = 0;

	friend class cPlotConfigCorrection;
};


