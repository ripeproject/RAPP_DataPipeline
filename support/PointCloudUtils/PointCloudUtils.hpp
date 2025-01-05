
#pragma once

#include "Constants.hpp"

#include <cbdf/PointCloudTypes.hpp>
#include <cbdf/PointCloud.hpp>

#include <vector>
#include <algorithm>
#include <iterator>

// Forward Declares



namespace pointcloud
{
	double distance_m(sPoint2D_t p1, sPoint2D_t p2);
	double distance_m(sPoint3D_t p1, sPoint3D_t p2);

	void orderBoundingBox(sBoundingBox_t& box);

	struct sLine_t
	{
		double slope = 0;
		double intercept = 0;
		bool vertical = false;
	};

	sLine_t computeLineParameters(sPoint2D_t p1, sPoint2D_t p2, bool swapAxis = false);

	template<typename POINT>
	std::vector<POINT> trim_outside(const std::vector<POINT>& pc, sBoundingBox_t box);

	template<typename POINT>
	std::vector<POINT> sliceAtGivenX(std::vector<POINT> pc, double x_mm, double tolerance_mm);

	template<typename POINT>
	std::vector<POINT> sliceAtGivenY(std::vector<POINT> pc, double y_mm, double tolerance_mm);

	template<class POINT>
	sPoint3D_t center(const cBasePointCloud<POINT>& pc);
}

/*****************************************************************************
* 
* Implementation
* 
******************************************************************************/

template<typename POINT>
std::vector<POINT> pointcloud::trim_outside(const std::vector<POINT>& pc, pointcloud::sBoundingBox_t box)
{
	std::vector<POINT> result;

	orderBoundingBox(box);

	// Convert bounding box coordinates (m) to spidercam coordinates (mm)

	for (auto& point : box.points)
	{
		point.X_m *= nConstants::M_TO_MM;
		point.Y_m *= nConstants::M_TO_MM;
	}

	auto line1 = computeLineParameters(box.points[0], box.points[1], true);
	auto line2 = computeLineParameters(box.points[1], box.points[2]);
	auto line3 = computeLineParameters(box.points[2], box.points[3], true);
	auto line4 = computeLineParameters(box.points[3], box.points[0]);

	double x, y;

	for (const POINT& point : pc)
	{
		x = line1.slope * point.y_mm + line1.intercept;

		if (point.x_mm > x)
		{
			x = line3.slope * point.y_mm + line3.intercept;

			if (point.x_mm < x)
			{
				y = line2.slope * point.x_mm + line2.intercept;

				if (point.y_mm < y)
				{
					y = line4.slope * point.x_mm + line4.intercept;

					if (point.y_mm > y)
					{
						result.push_back(point);
					}
				}
			}
		}
	}

	return result;
}


template<typename POINT>
std::vector<POINT> pointcloud::sliceAtGivenX(std::vector<POINT> pc, double x_mm, double tolerance_mm)
{
	std::vector<POINT> result;
	if (pc.empty())
		return result;

//	std::sort(pc.begin(), pc.end(), [](POINT a, POINT b) { return a.x_mm < b.x_mm; });

	double x_min = x_mm - tolerance_mm;
	double x_max = x_mm + tolerance_mm;

	std::copy_if(pc.begin(), pc.end(), std::back_inserter(result), [x_min, x_max](POINT a)
		{ return (x_min < a.x_mm) && (a.x_mm < x_max); });

	return result;
}



template<typename POINT>
std::vector<POINT> pointcloud::sliceAtGivenY(std::vector<POINT> pc, double y_mm, double tolerance_mm)
{
	std::vector<POINT> result;
	if (pc.empty())
		return result;

//	std::sort(pc.begin(), pc.end(), [](POINT a, POINT b) { return a.y_mm < b.y_mm; });

	double y_min = y_mm - tolerance_mm;
	double y_max = y_mm + tolerance_mm;

	std::copy_if(pc.begin(), pc.end(), std::back_inserter(result), [y_min, y_max](POINT a)
		{ return (y_min < a.y_mm) && (a.y_mm < y_max); });

	return result;
}


template<class POINT>
pointcloud::sPoint3D_t pointcloud::center(const cBasePointCloud<POINT>& pc)
{
	pointcloud::sPoint3D_t result;

	result.X_m = (pc.maxX_m() + pc.minX_m()) / 2.0;
	result.Y_m = (pc.maxY_m() + pc.minY_m()) / 2.0;
	result.Z_m = (pc.maxZ_m() + pc.minZ_m()) / 2.0;

	return result;
}
