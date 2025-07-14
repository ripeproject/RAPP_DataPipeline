
#pragma once

#include "datatypes.hpp"

#include "PlotSplitDataTypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotConfigBoundary
{
public:
	cPlotConfigBoundary();
	~cPlotConfigBoundary() = default;

	cPlotConfigBoundary& operator=(const cPlotConfigBoundary& rhs);

	void clear();

	bool empty() const;

	bool isDirty() const;

	rfm::sPlotBoundingBox_t getBoundingBox() const;

	rfm::rappPoint2D_t getNorthEastCorner() const;
	rfm::rappPoint2D_t getNorthWestCorner() const;
	rfm::rappPoint2D_t getSouthEastCorner() const;
	rfm::rappPoint2D_t getSouthWestCorner() const;

	bool hasSubPlots() const;
	uint8_t getNumOfSubPlots() const;
	ePlotOrientation getSubPlotOrientation() const;

	rfm::rappPoint2D_t center() const;

	bool contains(rfm::rappPoint2D_t point) const;
	bool contains(std::int32_t x_mm, std::int32_t y_mm) const;

	void setNorthEastCorner(rfm::rappPoint2D_t point);
	void setNorthWestCorner(rfm::rappPoint2D_t point);
	void setSouthEastCorner(rfm::rappPoint2D_t point);
	void setSouthWestCorner(rfm::rappPoint2D_t point);

	void setNorthEastCorner(std::int32_t x_mm, std::int32_t y_mm);
	void setNorthWestCorner(std::int32_t x_mm, std::int32_t y_mm);
	void setSouthEastCorner(std::int32_t x_mm, std::int32_t y_mm);
	void setSouthWestCorner(std::int32_t x_mm, std::int32_t y_mm);

	void setNumOfSubPlots(uint8_t num);
	void setSubPlotOrientation(ePlotOrientation orientation);

	operator rfm::sPlotBoundingBox_t() const;
	bool operator==(const cPlotConfigBoundary& rhs) const;
	bool operator!=(const cPlotConfigBoundary& rhs) const;

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	bool mUseMeters = false;

	rfm::rappPoint2D_t mNorthEastCorner;
	rfm::rappPoint2D_t mNorthWestCorner;
	rfm::rappPoint2D_t mSouthEastCorner;
	rfm::rappPoint2D_t mSouthWestCorner;

	uint8_t mNumOfSubPlots = 0;
	ePlotOrientation mSubPlotOrientation = ePlotOrientation::eNORTH_TO_SOUTH;

	friend class cPlotConfigFile;
	friend class cPlotConfigPlotInfo;
	friend class cPlotConfigCorrection;
};


