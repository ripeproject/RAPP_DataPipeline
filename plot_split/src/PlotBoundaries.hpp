
#pragma once

#include "datatypes.hpp"

#include "PointCloudTypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotBoundary
{
public:
	int32_t getPlotNumber() const;

	const std::string& getPlotName() const;
	const std::string& getEvent() const;
	const std::string& getDescription() const;

	rfm::rappPoint2D_t getNorthEastCorner() const;
	rfm::rappPoint2D_t getNorthWestCorner() const;
	rfm::rappPoint2D_t getSouthEastCorner() const;
	rfm::rappPoint2D_t getSouthWestCorner() const;

	double getEastBoundary_m() const;
	double getWestBoundary_m() const;

	double getNorthBoundary_m() const;
	double getSouthBoundary_m() const;

	pointcloud::sBoundingBox_t getBoundingBox() const;
	pointcloud::sBoundingBox_t getPlotBounds() const;

	bool inPlot(rfm::rappPoint2D_t point) const;
	bool inPlot(rfm::rappPoint_t point) const;

protected:
	cPlotBoundary();
	~cPlotBoundary() = default;

	bool load(const nlohmann::json& plot_info);

private:
	void initialize(std::array<rfm::rappPoint2D_t, 4> points);

protected:
	int32_t		mPlotNumber = 0;
	std::string mPlotName;
	std::string mEvent;
	std::string mDescription;

	std::int32_t mEastBoundary_mm = 0;
	std::int32_t mWestBoundary_mm = 0;

	std::int32_t mNorthBoundary_mm = 0;
	std::int32_t mSouthBoundary_mm = 0;

	struct sLine_t
	{
		double m = 0.0;
		double b = 0.0;
	};

	sLine_t mNorthSide;
	sLine_t mSouthSide;

	sLine_t mWestSide;
	sLine_t mEastSide;

	rfm::rappPoint2D_t mNorthEastCorner;
	rfm::rappPoint2D_t mNorthWestCorner;
	rfm::rappPoint2D_t mSouthEastCorner;
	rfm::rappPoint2D_t mSouthWestCorner;

	friend class cPlotBoundaries;
};


class cPlotBoundaries
{
public:
	cPlotBoundaries();
	~cPlotBoundaries();

	void clear();
	bool empty() const;

	std::size_t size() const;
	const std::vector<cPlotBoundary*>& getPlots() const;

	double getEastBoundary_m();
	double getWestBoundary_m();

	double getNorthBoundary_m();
	double getSouthBoundary_m();

	pointcloud::sBoundingBox_t getBoundingBox() const;

	bool load(const nlohmann::json& plot_info);

protected:

	double mEastBoundary_m = 0.0;
	double mWestBoundary_m = 0.0;

	double mNorthBoundary_m = 0.0;
	double mSouthBoundary_m = 0.0;

	std::vector<cPlotBoundary*> mPlotBoundaries;
};
