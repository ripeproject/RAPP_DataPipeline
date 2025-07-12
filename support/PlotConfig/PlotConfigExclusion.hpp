
#pragma once

#include "datatypes.hpp"

#include "PlotSplitDataTypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotConfigExclusion
{
public:
	cPlotConfigExclusion();
	~cPlotConfigExclusion() = default;

	void clear();

	bool empty() const;

	bool isDirty() const;

	rfm::sPlotBoundingBox_t getBoundingBox() const;

	rfm::rappPoint2D_t center() const;

	bool contains(rfm::rappPoint2D_t point) const;
	bool contains(std::int32_t x_mm, std::int32_t y_mm) const;

	operator rfm::sPlotBoundingBox_t() const;
	bool operator==(const cPlotConfigExclusion& rhs) const;
	bool operator!=(const cPlotConfigExclusion& rhs) const;

	void setCenterX_mm(uint32_t x_mm);
	void setCenterY_mm(uint32_t y_mm);
	void setCenter(uint32_t x_mm, uint32_t y_mm);

	void setLength_mm(uint32_t length_mm);
	void setWidth_mm(uint32_t width_mm);

	void setOrientation_deg(double angle_deg);

	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	uint32_t mCenterX_mm = 0;
	uint32_t mCenterY_mm = 0;

	uint32_t mLength_mm = 0;
	uint32_t mWidth_mm = 0;

	// Note: 0 degrees is due south
	double mOrientation_deg = 0;

	friend class cPlotConfigFile;
	friend class cPlotConfigPlotInfo;
};


