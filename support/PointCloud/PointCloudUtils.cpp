
#include "PointCloudUtils.hpp"

#include <algorithm>


double pointcloud::distance_m(sPoint2D_t p1, sPoint2D_t p2)
{
	double dx = p1.X_m - p2.X_m;
	double dy = p1.Y_m - p2.Y_m;

	return sqrt(dx*dx + dy*dy);
}

double pointcloud::distance_m(sPoint3D_t p1, sPoint3D_t p2)
{
	double dx = p1.X_m - p2.X_m;
	double dy = p1.Y_m - p2.Y_m;
	double dz = p1.Z_m - p2.Z_m;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

void pointcloud::orderBoundingBox(sBoundingBox_t& box)
{
	std::sort(box.points.begin(), box.points.end(), [](sPoint2D_t a, sPoint2D_t b)
		{
			return a.X_m < b.X_m;
		});

	if (box.points[1].Y_m < box.points[0].Y_m)
	{
		std::swap(box.points[0], box.points[1]);
	}

	double d1 = distance_m(box.points[1], box.points[2]);

	double d2 = distance_m(box.points[1], box.points[3]);

	if (d2 < d1)
	{
		std::swap(box.points[2], box.points[3]);
	}

}

pointcloud::sLine_t pointcloud::computeLineParameters(sPoint2D_t p1, sPoint2D_t p2, bool swapAxis)
{
	sLine_t result;

	if (swapAxis)
	{
		std::swap(p1.X_m, p1.Y_m);
		std::swap(p2.X_m, p2.Y_m);
	}

	if (p1.X_m == p2.X_m)
	{
		result.vertical = true;
		result.intercept = p1.X_m;
		return result;
	}

	result.slope = (p2.Y_m - p1.Y_m) / (p2.X_m - p1.X_m);
	result.intercept = p1.Y_m - result.slope * p1.X_m;

	return result;
}
