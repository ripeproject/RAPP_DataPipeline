
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>


class cPlotBoundary
{
public:
	cPlotBoundary(const std::string& name, rfm::rappPoint2D_t p1,
		rfm::rappPoint2D_t p2, rfm::rappPoint2D_t p3, rfm::rappPoint2D_t p4);

	cPlotBoundary(const std::string& name, 
		const std::array<rfm::rappPoint2D_t, 4>& points);

	~cPlotBoundary() = default;

	const std::string& getPlotName() const;

	rfm::rappPoint2D_t getNorthEastCorner() const;
	rfm::rappPoint2D_t getNorthWestCorner() const;
	rfm::rappPoint2D_t getSouthEastCorner() const;
	rfm::rappPoint2D_t getSouthWestCorner() const;

	double getEastBoundary_m() const;
	double getWestBoundary_m() const;

	double getNorthBoundary_m() const;
	double getSouthBoundary_m() const;

	bool inPlot(rfm::rappPoint2D_t point) const;
	bool inPlot(rfm::rappPoint_t point) const;

private:
	void initialize(std::array<rfm::rappPoint2D_t, 4> points);

protected:
	std::string mPlotName;

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
};


class cPlotBoundaries
{
public:
	cPlotBoundaries();
	virtual ~cPlotBoundaries();

	void clear();
	bool empty() const;

	double getEastBoundary_m();
	double getWestBoundary_m();

	double getNorthBoundary_m();
	double getSouthBoundary_m();

	bool load(const nlohmann::json& plot_info);

protected:

	double mEastBoundary_m = 0.0;
	double mWestBoundary_m = 0.0;

	double mNorthBoundary_m = 0.0;
	double mSouthBoundary_m = 0.0;

	std::vector<cPlotBoundary> mPlotBoundaries;
};
