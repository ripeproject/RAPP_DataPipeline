
#include "PlotSplitUtils.hpp"

#include "PointCloudUtils.hpp"


double distance_mm(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t p2)
{
	double dx = p1.x_mm - p2.x_mm;
	double dy = p1.y_mm - p2.y_mm;

	return sqrt(dx * dx + dy * dy);
}

double distance_mm(rfm::rappPoint_t p1, rfm::rappPoint_t p2)
{
	double dx = p1.x_mm - p2.x_mm;
	double dy = p1.y_mm - p2.y_mm;
	double dz = p1.z_mm - p2.z_mm;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

void orderBoundingBox(std::array<rfm::rappPoint2D_t, 4>& box)
{
	std::sort(box.begin(), box.end(), [](auto a, auto b)
		{
			return a.x_mm < b.x_mm;
		});

	if (box[1].y_mm < box[0].y_mm)
	{
		std::swap(box[0], box[1]);
	}

	double d1 = distance_mm(box[1], box[2]);

	double d2 = distance_mm(box[1], box[3]);

	if (d2 < d1)
	{
		std::swap(box[2], box[3]);
	}
}

rfm::rappPoint_t compute_center_of_height(const cRappPointCloud& pc, double height_threshold_pct)
{
	double total_height = 0.0;
	double Mx = 0.0;
	double My = 0.0;

	double min_x = pc.minX_mm();
	double min_y = pc.minY_mm();
	double z = pc.minZ_mm() + (pc.maxZ_mm() - pc.minZ_mm()) * (height_threshold_pct/100.0);

	for (auto& point : pc)
	{
		if (point.z_mm >= z)
		{
			total_height += point.z_mm;
			Mx += (point.y_mm - min_y) * point.z_mm;
			My += (point.x_mm - min_x) * point.z_mm;
		}
	}

	if (total_height == 0.0)
		return rfm::rappPoint_t();

	double x = My / total_height;
	double y = Mx / total_height;

	rfm::rappPoint_t point;

	point.x_mm = static_cast<int32_t>(x + min_x);
	point.y_mm = static_cast<int32_t>(y + min_y);
	point.z_mm = static_cast<int32_t>(total_height / pc.size());

	return point;
}

sLine_t computeLineParameters(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t p2, bool swapAxis)
{
	sLine_t result;

	if (swapAxis)
	{
		std::swap(p1.x_mm, p1.y_mm);
		std::swap(p2.x_mm, p2.y_mm);
	}

	if (p1.x_mm == p2.x_mm)
	{
		result.vertical = true;
		result.intercept = p1.x_mm;
		return result;
	}

	result.slope = (p2.y_mm - p1.y_mm) / (p2.x_mm - p1.x_mm);
	result.intercept = p1.y_mm - result.slope * p1.x_mm;

	return result;
}

cRappPointCloud trim_outside(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box)
{
	cRappPointCloud result;

	std::array<rfm::rappPoint2D_t, 4> points = {box.northEastCorner, box.northWestCorner, box.southEastCorner, box.southWestCorner};

	orderBoundingBox(points);

	auto line1 = computeLineParameters(points[0], points[1], true);
	auto line2 = computeLineParameters(points[1], points[2]);
	auto line3 = computeLineParameters(points[2], points[3], true);
	auto line4 = computeLineParameters(points[3], points[0]);

	double x, y;

	for (const auto& point : pc)
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

std::vector<cRappPointCloud::value_type> sliceAtGivenX(const cRappPointCloud& pc, double x_mm, double tolerance_mm)
{
	std::vector<cRappPointCloud::value_type> result;
	if (pc.empty())
		return result;

	double x_min = x_mm - tolerance_mm;
	double x_max = x_mm + tolerance_mm;

	std::copy_if(pc.begin(), pc.end(), std::back_inserter(result), [x_min, x_max](auto a)
		{ return (x_min < a.x_mm) && (a.x_mm < x_max); });

	return result;
}

std::vector<cRappPointCloud::value_type> sliceAtGivenY(const cRappPointCloud& pc, double y_mm, double tolerance_mm)
{
	std::vector<cRappPointCloud::value_type> result;
	if (pc.empty())
		return result;

	//	std::sort(pc.begin(), pc.end(), [](POINT a, POINT b) { return a.y_mm < b.y_mm; });

	double y_min = y_mm - tolerance_mm;
	double y_max = y_mm + tolerance_mm;

	std::copy_if(pc.begin(), pc.end(), std::back_inserter(result), [y_min, y_max](auto a)
		{ return (y_min < a.y_mm) && (a.y_mm < y_max); });

	return result;
}

cRappPointCloud isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box)
{
	auto result = trim_outside(pc, box);
	result.recomputeBounds();
	return result;
}

std::vector<cRappPointCloud> isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm)
{
	int half_length_mm = plot_length_mm / 2;
	int half_width_mm = plot_width_mm / 2;

	auto temp = trim_outside(pc, box);
	temp.recomputeBounds();

	std::vector<cRappPointCloud> result;

	if (numSubPlots < 1)
	{
		rfm::sPlotBoundingBox_t plot_bounds;

		int cx_mm = (temp.maxX_mm() + temp.minX_mm()) / 2;
		int cy_mm = (temp.maxY_mm() + temp.minY_mm()) / 2;

		switch (orientation)
		{
		case ePlotOrientation::eWEST_TO_EAST:
		case ePlotOrientation::eEAST_TO_WEST:
		{
			plot_bounds.northWestCorner = { cx_mm - half_length_mm, cy_mm - half_width_mm };
			plot_bounds.southWestCorner = { cx_mm + half_length_mm, cy_mm - half_width_mm };
			plot_bounds.northEastCorner = { cx_mm - half_length_mm, cy_mm + half_width_mm };
			plot_bounds.southEastCorner = { cx_mm + half_length_mm, cy_mm + half_width_mm };
			break;
		}
		case ePlotOrientation::eNORTH_TO_SOUTH:
		case ePlotOrientation::eSOUTH_TO_NORTH:
		{
			plot_bounds.northWestCorner = { cx_mm - half_width_mm, cy_mm - half_length_mm };
			plot_bounds.southWestCorner = { cx_mm + half_width_mm, cy_mm - half_length_mm };
			plot_bounds.northEastCorner = { cx_mm - half_width_mm, cy_mm + half_length_mm };
			plot_bounds.southEastCorner = { cx_mm + half_width_mm, cy_mm + half_length_mm };
			break;
		}
		}

		temp.trim_outside(plot_bounds);
		temp.recomputeBounds();

		result.push_back(temp);
		return result;
	}

	int dx_mm = temp.maxX_mm() - temp.minX_mm();
	int dy_mm = temp.maxX_mm() - temp.minX_mm();

	switch (orientation)
	{
	case ePlotOrientation::eWEST_TO_EAST:
	case ePlotOrientation::eEAST_TO_WEST:
	{
		dx_mm /= numSubPlots;
		break;
	}
	case ePlotOrientation::eNORTH_TO_SOUTH:
	case ePlotOrientation::eSOUTH_TO_NORTH:
	{
		dy_mm /= numSubPlots;
		break;
	}
	}

	for (int i = 0; i < numSubPlots; ++i)
	{
		rfm::sPlotBoundingBox_t plot_bounds;

		switch (orientation)
		{
		case ePlotOrientation::eWEST_TO_EAST:
		case ePlotOrientation::eEAST_TO_WEST:
		{
			int cx_mm = (dx_mm / 2) + temp.minX_mm() + (i * dx_mm);
			int cy_mm = (temp.maxY_mm() + temp.minY_mm()) / 2;

			plot_bounds.northWestCorner = { cx_mm - half_length_mm, cy_mm - half_width_mm };
			plot_bounds.southWestCorner = { cx_mm + half_length_mm, cy_mm - half_width_mm };
			plot_bounds.northEastCorner = { cx_mm - half_length_mm, cy_mm + half_width_mm };
			plot_bounds.southEastCorner = { cx_mm + half_length_mm, cy_mm + half_width_mm };
			break;
		}
		case ePlotOrientation::eNORTH_TO_SOUTH:
		case ePlotOrientation::eSOUTH_TO_NORTH:
		{
			int cx_mm = (temp.maxX_mm() + temp.minX_mm()) / 2;
			int cy_mm = (dy_mm / 2) + temp.minY_mm() + (i * dy_mm);

			plot_bounds.northWestCorner = { cx_mm - half_width_mm, cy_mm - half_length_mm };
			plot_bounds.southWestCorner = { cx_mm + half_width_mm, cy_mm - half_length_mm };
			plot_bounds.northEastCorner = { cx_mm - half_width_mm, cy_mm + half_length_mm };
			plot_bounds.southEastCorner = { cx_mm + half_width_mm, cy_mm + half_length_mm };
			break;
		}
		}

		auto plot = trim_outside(temp, plot_bounds);
		plot.recomputeBounds();
		result.push_back(plot);
	}

	return result;
}

cRappPointCloud isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct)
{
	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	auto result = trim_outside(pc, box);
	auto c1 = compute_center_of_height(result, height_threshold_pct);

	std::int32_t north_mm = c1.x_mm - half_width_mm;
	std::int32_t south_mm = c1.x_mm + half_width_mm;
	std::int32_t east_mm = c1.y_mm + half_length_mm;
	std::int32_t west_mm = c1.y_mm - half_length_mm;

	rfm::sPlotBoundingBox_t plot;
	plot.northWestCorner.x_mm = north_mm;
	plot.northWestCorner.y_mm = west_mm;

	plot.southWestCorner.x_mm = south_mm;
	plot.southWestCorner.y_mm = west_mm;

	plot.northEastCorner.x_mm = north_mm;
	plot.northEastCorner.y_mm = east_mm;

	plot.southEastCorner.x_mm = south_mm;
	plot.southEastCorner.y_mm = east_mm;

	result = trim_outside(pc, plot);
	result.recomputeBounds();

	return result;
}


std::vector<cRappPointCloud> isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
	double height_threshold_pct)
{
	int half_length_mm = plot_length_mm / 2;
	int half_width_mm = plot_width_mm / 2;

	auto temp = trim_outside(pc, box);
	temp.recomputeBounds();

	std::vector<cRappPointCloud> result;

	if (numSubPlots < 1)
	{
		rfm::sPlotBoundingBox_t plot_bounds;

		auto coh = compute_center_of_height(temp, height_threshold_pct);

		switch (orientation)
		{
		case ePlotOrientation::eWEST_TO_EAST:
		case ePlotOrientation::eEAST_TO_WEST:
		{
			plot_bounds.northWestCorner = { coh.x_mm - half_length_mm, coh.y_mm - half_width_mm };
			plot_bounds.southWestCorner = { coh.x_mm + half_length_mm, coh.y_mm - half_width_mm };
			plot_bounds.northEastCorner = { coh.x_mm - half_length_mm, coh.y_mm + half_width_mm };
			plot_bounds.southEastCorner = { coh.x_mm + half_length_mm, coh.y_mm + half_width_mm };
			break;
		}
		case ePlotOrientation::eNORTH_TO_SOUTH:
		case ePlotOrientation::eSOUTH_TO_NORTH:
		{
			plot_bounds.northWestCorner = { coh.x_mm - half_width_mm, coh.y_mm - half_length_mm };
			plot_bounds.southWestCorner = { coh.x_mm + half_width_mm, coh.y_mm - half_length_mm };
			plot_bounds.northEastCorner = { coh.x_mm - half_width_mm, coh.y_mm + half_length_mm };
			plot_bounds.southEastCorner = { coh.x_mm + half_width_mm, coh.y_mm + half_length_mm };
			break;
		}
		}

		auto plot = trim_outside(pc, plot_bounds);
		plot.recomputeBounds();

		result.push_back(plot);
		return result;
	}

	int dx_mm = temp.maxX_mm() - temp.minX_mm();
	int dy_mm = temp.maxX_mm() - temp.minX_mm();

	switch (orientation)
	{
	case ePlotOrientation::eWEST_TO_EAST:
	case ePlotOrientation::eEAST_TO_WEST:
	{
		dx_mm /= numSubPlots;
		break;
	}
	case ePlotOrientation::eNORTH_TO_SOUTH:
	case ePlotOrientation::eSOUTH_TO_NORTH:
	{
		dy_mm /= numSubPlots;
		break;
	}
	}

	for (int i = 0; i < numSubPlots; ++i)
	{
		rfm::sPlotBoundingBox_t plot_bounds;

		switch (orientation)
		{
		case ePlotOrientation::eWEST_TO_EAST:
		case ePlotOrientation::eEAST_TO_WEST:
		{
			int cx_mm = (dx_mm / 2) + temp.minX_mm() + (i * dx_mm);
			int cy_mm = (temp.maxY_mm() + temp.minY_mm()) / 2;

			plot_bounds.northWestCorner = { cx_mm - half_length_mm, cy_mm - half_width_mm };
			plot_bounds.southWestCorner = { cx_mm + half_length_mm, cy_mm - half_width_mm };
			plot_bounds.northEastCorner = { cx_mm - half_length_mm, cy_mm + half_width_mm };
			plot_bounds.southEastCorner = { cx_mm + half_length_mm, cy_mm + half_width_mm };
			break;
		}
		case ePlotOrientation::eNORTH_TO_SOUTH:
		case ePlotOrientation::eSOUTH_TO_NORTH:
		{
			int cx_mm = (temp.maxX_mm() + temp.minX_mm()) / 2;
			int cy_mm = (dy_mm / 2) + temp.minY_mm() + (i * dy_mm);

			plot_bounds.northWestCorner = { cx_mm - half_width_mm, cy_mm - half_length_mm };
			plot_bounds.southWestCorner = { cx_mm + half_width_mm, cy_mm - half_length_mm };
			plot_bounds.northEastCorner = { cx_mm - half_width_mm, cy_mm + half_length_mm };
			plot_bounds.southEastCorner = { cx_mm + half_width_mm, cy_mm + half_length_mm };
			break;
		}
		}

		auto region = trim_outside(temp, plot_bounds);
		region.recomputeBounds();

		auto coh = compute_center_of_height(region, height_threshold_pct);

		switch (orientation)
		{
		case ePlotOrientation::eWEST_TO_EAST:
		case ePlotOrientation::eEAST_TO_WEST:
		{
			plot_bounds.northWestCorner = { coh.x_mm - half_length_mm, coh.y_mm - half_width_mm };
			plot_bounds.southWestCorner = { coh.x_mm + half_length_mm, coh.y_mm - half_width_mm };
			plot_bounds.northEastCorner = { coh.x_mm - half_length_mm, coh.y_mm + half_width_mm };
			plot_bounds.southEastCorner = { coh.x_mm + half_length_mm, coh.y_mm + half_width_mm };
			break;
		}
		case ePlotOrientation::eNORTH_TO_SOUTH:
		case ePlotOrientation::eSOUTH_TO_NORTH:
		{
			plot_bounds.northWestCorner = { coh.x_mm - half_width_mm, coh.y_mm - half_length_mm };
			plot_bounds.southWestCorner = { coh.x_mm + half_width_mm, coh.y_mm - half_length_mm };
			plot_bounds.northEastCorner = { coh.x_mm - half_width_mm, coh.y_mm + half_length_mm };
			plot_bounds.southEastCorner = { coh.x_mm + half_width_mm, coh.y_mm + half_length_mm };
			break;
		}
		}

		auto plot = trim_outside(pc, plot_bounds);
		plot.recomputeBounds();

		result.push_back(plot);
	}

	return result;
}


cRappPointCloud isolate_iterative(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box, 
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double tolerance_mm, double bound_pct)
{
	std::int32_t width_mm = box.southWestCorner.x_mm - box.northWestCorner.x_mm;
	double width_limit_mm = width_mm * (bound_pct / 100.0);
	double half_width_mm = width_mm / 2.0;

	std::int32_t length_mm = box.southEastCorner.y_mm - box.southWestCorner.y_mm;
	double length_limit_mm = length_mm * (bound_pct / 100.0);
	double half_length_mm = length_mm / 2.0;

	std::int32_t center_x_mm = (box.southEastCorner.x_mm + box.northEastCorner.x_mm) / 2;
	std::int32_t center_y_mm = (box.northWestCorner.y_mm + box.northEastCorner.y_mm) / 2;

	std::int32_t north_limit_mm = center_x_mm - (width_limit_mm / 2.0);
	std::int32_t south_limit_mm = center_x_mm + (width_limit_mm / 2.0);
	std::int32_t east_limit_mm = center_y_mm + (length_limit_mm / 2.0);
	std::int32_t west_limit_mm = center_y_mm - (length_limit_mm / 2.0);

	auto test1 = trim_outside(pc, box);
	auto c1 = compute_center_of_height(test1);
	c1.z_mm = 0;
	std::int32_t north_mm = c1.x_mm - half_width_mm;
	std::int32_t south_mm = c1.x_mm + half_width_mm;
	std::int32_t east_mm = c1.y_mm + half_length_mm;
	std::int32_t west_mm = c1.y_mm - half_length_mm;

	rfm::sPlotBoundingBox_t box1;
	box1.northWestCorner.x_mm = north_mm;
	box1.northWestCorner.y_mm = west_mm;

	box1.southWestCorner.x_mm = south_mm;
	box1.southWestCorner.y_mm = west_mm;

	box1.northEastCorner.x_mm = north_mm;
	box1.northEastCorner.y_mm = east_mm;

	box1.southEastCorner.x_mm = south_mm;
	box1.southEastCorner.y_mm = east_mm;

	auto test2 = trim_outside(pc, box1);
	auto c2 = compute_center_of_height(test2);
	c2.z_mm = 0;

	double d = distance_mm(c1, c2);
	while (d > tolerance_mm)
	{
		test1 = test2;
		c1 = c2;

		if ((c1.x_mm < north_limit_mm) || (c1.x_mm > south_limit_mm))
			break;

		if ((c1.y_mm < west_limit_mm) || (c1.y_mm > east_limit_mm))
			break;

		std::int32_t north_mm = c1.x_mm - half_width_mm;
		std::int32_t south_mm = c1.x_mm + half_width_mm;
		std::int32_t east_mm = c1.y_mm + half_length_mm;
		std::int32_t west_mm = c1.y_mm - half_length_mm;

		rfm::sPlotBoundingBox_t box1;
		box1.northWestCorner.x_mm = north_mm;
		box1.northWestCorner.y_mm = west_mm;

		box1.southWestCorner.x_mm = south_mm;
		box1.southWestCorner.y_mm = west_mm;

		box1.northEastCorner.x_mm = north_mm;
		box1.northEastCorner.y_mm = east_mm;

		box1.southEastCorner.x_mm = south_mm;
		box1.southEastCorner.y_mm = east_mm;

		test2 = trim_outside(pc, box1);
		c2 = compute_center_of_height(test2);
		c2.z_mm = 0;
		d = distance_mm(c1, c2);
	}

	half_length_mm = plot_length_mm / 2.0;
	half_width_mm = plot_width_mm / 2.0;

	north_mm = c2.x_mm - half_width_mm;
	south_mm = c2.x_mm + half_width_mm;
	east_mm = c2.y_mm + half_length_mm;
	west_mm = c2.y_mm - half_length_mm;

	rfm::sPlotBoundingBox_t plot;
	plot.northWestCorner.x_mm = north_mm;
	plot.northWestCorner.y_mm = west_mm;

	plot.southWestCorner.x_mm = south_mm;
	plot.southWestCorner.y_mm = west_mm;

	plot.northEastCorner.x_mm = north_mm;
	plot.northEastCorner.y_mm = east_mm;

	plot.southEastCorner.x_mm = south_mm;
	plot.southEastCorner.y_mm = east_mm;

	test2 = trim_outside(pc, plot);

	test2.recomputeBounds();
	return test2;
}

cRappPointCloud isolate_pour(const cRappPointCloud& pc)
{
	return pc;
}


cRappPointCloud isolate_find_center(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct)
{
	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	auto result = trim_outside(pc, box);
	auto c1 = compute_center_of_height(result, height_threshold_pct);

	std::int32_t north_mm = c1.x_mm - half_width_mm;
	std::int32_t south_mm = c1.x_mm + half_width_mm;
	std::int32_t east_mm = c1.y_mm + half_length_mm;
	std::int32_t west_mm = c1.y_mm - half_length_mm;

	rfm::sPlotBoundingBox_t plot;
	plot.northWestCorner.x_mm = north_mm;
	plot.northWestCorner.y_mm = west_mm;

	plot.southWestCorner.x_mm = south_mm;
	plot.southWestCorner.y_mm = west_mm;

	plot.northEastCorner.x_mm = north_mm;
	plot.northEastCorner.y_mm = east_mm;

	plot.southEastCorner.x_mm = south_mm;
	plot.southEastCorner.y_mm = east_mm;

	result = trim_outside(pc, plot);
	result.recomputeBounds();

	return result;

}
