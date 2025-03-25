
#include "PlotSplitUtils.hpp"

#include "RappPointCloud.hpp"

#include <array>
#include <iterator>
#include <algorithm>
#include <numeric>


template<typename POINT>
plot::sPoint3D_t to_point(const POINT& point)
{
	return { point.x_mm, point.y_mm, point.z_mm,
		point.range_mm, point.signal, point.reflectivity, point.nir,
		point.frameID, point.chnNum, point.pixelNum };
}

plot::sPoint3D_t to_plot_point(const rfm::sPoint3D_t& point)
{
	return { point.x_mm, point.y_mm, point.z_mm,
		point.range_mm, point.signal, point.reflectivity, point.nir,
		point.frameID, point.chnNum, point.pixelNum };
}

rfm::sPoint3D_t to_rfm_point(const plot::sPoint3D_t& point)
{
	return { point.x_mm, point.y_mm, point.z_mm, 0,
		point.range_mm, point.signal, point.reflectivity, point.nir,
		point.frameID, point.chnNum, point.pixelNum };
}

std::vector<plot::sPoint3D_t> to_plot_points(const std::vector<rfm::sPoint3D_t>& points)
{ 
	std::vector<plot::sPoint3D_t> result(points.size());

	std::transform(points.begin(), points.end(), result.begin(), to_plot_point);

	return result;
}

std::vector<rfm::sPoint3D_t>  to_rfm_points(const std::vector<plot::sPoint3D_t>& points)
{
	std::vector<rfm::sPoint3D_t> result(points.size());

	std::transform(points.begin(), points.end(), result.begin(), to_rfm_point);

	return result;
}

cPlotPointCloud to_plot_point_cloud(const cRappPointCloud& pc)
{
	cPlotPointCloud result;

	std::vector<plot::sPoint3D_t> plotPoints(pc.data().size());

	std::transform(pc.data().begin(), pc.data().end(), plotPoints.begin(), to_plot_point);

	result.assign(plotPoints);

	result.setVegetationOnly(pc.vegetationOnly());

	if (!pc.hasFrameIDs())
	{
		result.disableFrameIDs();
		result.disablePixelInfo();
	}
	else if (!pc.hasPixelInfo())
	{
		result.disablePixelInfo();
	}

	return result;
}

double plot::distance_mm(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t p2)
{
	double dx = p1.x_mm - p2.x_mm;
	double dy = p1.y_mm - p2.y_mm;

	return sqrt(dx * dx + dy * dy);
}

double plot::distance_mm(rfm::rappPoint_t p1, rfm::rappPoint_t p2)
{
	double dx = p1.x_mm - p2.x_mm;
	double dy = p1.y_mm - p2.y_mm;
	double dz = p1.z_mm - p2.z_mm;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

bool plot::contains(const cRappPointCloud& pc, const rfm::sPlotBoundingBox_t& box)
{
	if ((box.northEastCorner.x_mm < pc.minX_mm())
		|| (box.northWestCorner.x_mm < pc.minX_mm()))
		return false;

	if ((box.southEastCorner.x_mm > pc.maxX_mm())
		|| (box.southWestCorner.x_mm > pc.maxX_mm()))
		return false;

	if ((box.northWestCorner.y_mm < pc.minY_mm())
		|| (box.southWestCorner.y_mm < pc.minY_mm()))
		return false;

	if ((box.northEastCorner.y_mm > pc.maxY_mm())
		|| (box.southEastCorner.y_mm > pc.maxY_mm()))
		return false;

	return true;
}

bool plot::contains(const cPlotPointCloud& pc, const rfm::sPlotBoundingBox_t& box)
{
	if ((box.northEastCorner.x_mm < pc.minX_mm())
		|| (box.northWestCorner.x_mm < pc.minX_mm()))
		return false;

	if ((box.southEastCorner.x_mm > pc.maxX_mm())
		|| (box.southWestCorner.x_mm > pc.maxX_mm()))
		return false;

	if ((box.northWestCorner.y_mm < pc.minY_mm())
		|| (box.southWestCorner.y_mm < pc.minY_mm()))
		return false;

	if ((box.northEastCorner.y_mm > pc.maxY_mm())
		|| (box.southEastCorner.y_mm > pc.maxY_mm()))
		return false;

	return true;
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

	double d1 = plot::distance_mm(box[1], box[2]);

	double d2 = plot::distance_mm(box[1], box[3]);

	if (d2 < d1)
	{
		std::swap(box[2], box[3]);
	}
}

namespace
{
	template<class PC>
	rfm::rappPoint_t compute_center_of_height(const PC& pc, double height_threshold_pct)
	{
		double total_height = 0.0;
		double Mx = 0.0;
		double My = 0.0;

		double min_x = pc.minX_mm();
		double min_y = pc.minY_mm();

		double z = pc.minZ_mm() + (pc.maxZ_mm() - pc.minZ_mm()) * (height_threshold_pct / 100.0);

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

	template<class POINT>
	rfm::rappPoint_t compute_center_of_height(const std::vector<POINT>& points, double height_threshold_pct)
	{
		double total_height = 0.0;
		double Mx = 0.0;
		double My = 0.0;

		double min_x = std::numeric_limits<double>::max();
		double min_y = std::numeric_limits<double>::max();
		double min_z = std::numeric_limits<double>::max();
		double max_z = -min_z;

		for (const auto& point : points)
		{
			if (point.x_mm < min_x) min_x = point.x_mm;
			if (point.y_mm < min_y) min_y = point.y_mm;
			if (point.z_mm < min_z) min_z = point.z_mm;
			if (point.z_mm > max_z) max_z = point.z_mm;
		}

		double z = min_z + (max_z - min_z) * (height_threshold_pct / 100.0);

		for (auto& point : points)
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
		point.z_mm = static_cast<int32_t>(total_height / points.size());

		return point;
	}
}

rfm::rappPoint_t plot::compute_center_of_height(const cRappPointCloud& pc, double height_threshold_pct)
{
	return ::compute_center_of_height<cRappPointCloud>(pc, height_threshold_pct);
}

rfm::rappPoint_t plot::compute_center_of_height(const cPlotPointCloud& pc, double height_threshold_pct)
{
	return ::compute_center_of_height<cPlotPointCloud>(pc, height_threshold_pct);
}

rfm::rappPoint_t plot::compute_center_of_height(const std::vector<plot::sPoint3D_t>& points, double height_threshold_pct)
{
	return ::compute_center_of_height<plot::sPoint3D_t>(points, height_threshold_pct);
}

rfm::rappPoint_t plot::compute_center_of_height(const std::vector<rfm::sPoint3D_t>& points, double height_threshold_pct)
{
	return ::compute_center_of_height<rfm::sPoint3D_t>(points, height_threshold_pct);
}

rfm::sPlotBoundingBox_t plot::compute_bounding_box_center_of_plot(rfm::sPlotBoundingBox_t box, std::int32_t plot_width_mm, std::int32_t plot_length_mm)
{
	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	plot::rappPoint_t c1;
	c1.x_mm = static_cast<std::int32_t>((((box.southEastCorner.x_mm + box.northEastCorner.x_mm) / 2.0)
		+ ((box.southWestCorner.x_mm + box.northWestCorner.x_mm) / 2.0)) / 2.0);

	c1.y_mm = static_cast<std::int32_t>((((box.northEastCorner.y_mm + box.northWestCorner.y_mm) / 2.0)
		+ ((box.southEastCorner.y_mm + box.southWestCorner.y_mm) / 2.0)) / 2.0);

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

	return plot;
}

namespace
{
	template<class POINT>
	rfm::sPlotBoundingBox_t compute_bounding_box_center_of_height(const std::vector<POINT>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
	{
		double width_mm = ((box.southEastCorner.x_mm - box.northEastCorner.x_mm) + (box.southWestCorner.x_mm - box.northWestCorner.x_mm)) / 2.0;
		double length_mm = ((box.northEastCorner.y_mm - box.northWestCorner.y_mm) + (box.southEastCorner.y_mm - box.southWestCorner.y_mm)) / 2.0;

		double max_shift_x_mm = ((max_displacement_pct / 100.0) * width_mm) / 2.0;
		double max_shift_y_mm = ((max_displacement_pct / 100.0) * length_mm) / 2.0;

		double half_width_mm = plot_width_mm / 2.0;
		double half_length_mm = plot_length_mm / 2.0;

		auto result = ::trim_outside(points, box, false, {});
		result.recomputeBounds();

		auto c0 = result.center();

		auto c1 = compute_center_of_height(result, height_threshold_pct);

		auto dx = c0.x_mm - c1.x_mm;
		auto dy = c0.y_mm - c1.y_mm;

		if (std::abs(dx) > max_shift_x_mm)
		{
			if (dx < 0)
				c1.x_mm = c0.x_mm + max_shift_x_mm;
			else
				c1.x_mm = c0.x_mm - max_shift_x_mm;
		}


		if (std::abs(dy) > max_shift_y_mm)
		{
			if (dx < 0)
				c1.y_mm = c0.y_mm + max_shift_y_mm;
			else
				c1.y_mm = c0.y_mm - max_shift_y_mm;
		}


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

		return plot;
	}
}

rfm::sPlotBoundingBox_t plot::compute_bounding_box_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
{
	return compute_bounding_box_center_of_height(pc.data(), box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);
}

rfm::sPlotBoundingBox_t plot::compute_bounding_box_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
{
	return compute_bounding_box_center_of_height(pc.data(), box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);
}

rfm::sPlotBoundingBox_t plot::compute_bounding_box_center_of_height(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
{
	return ::compute_bounding_box_center_of_height(points, box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);
}

rfm::sPlotBoundingBox_t plot::compute_bounding_box_center_of_height(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
{
	return ::compute_bounding_box_center_of_height(points, box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);
}


plot::sLine_t plot::computeLineParameters(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t p2, bool swapAxis)
{
	plot::sLine_t result;

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

namespace
{
	template<typename POINT>
	cPlotPointCloud trim_outside(const std::vector<POINT>& points, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm)
	{
		cPlotPointCloud result;

		std::array<rfm::rappPoint2D_t, 4> corners = { box.northEastCorner, box.northWestCorner, box.southEastCorner, box.southWestCorner };

		orderBoundingBox(corners);

		auto line1 = plot::computeLineParameters(corners[0], corners[1], true);
		auto line2 = plot::computeLineParameters(corners[1], corners[2]);
		auto line3 = plot::computeLineParameters(corners[2], corners[3], true);
		auto line4 = plot::computeLineParameters(corners[3], corners[0]);

		double x, y;

		for (const auto& point : points)
		{
			x = line1.slope * point.y_mm + line1.intercept;

			if (point.x_mm >= x)
			{
				x = line3.slope * point.y_mm + line3.intercept;

				if (point.x_mm <= x)
				{
					y = line2.slope * point.x_mm + line2.intercept;

					if (point.y_mm <= y)
					{
						y = line4.slope * point.x_mm + line4.intercept;

						if (point.y_mm >= y)
						{
							result.push_back(to_point(point));
						}
					}
				}
			}
		}

		result.setVegetationOnly(vegetationOnly);

		if (groundLevel_mm.has_value())
			result.setGroundLevel_mm(groundLevel_mm.value());
		else
			result.clearGroundLevel_mm();

		return result;
	}

	template<typename POINT>
	cPlotPointCloud trim_below(std::vector<POINT> points, int z_mm, bool vegetationOnly, std::optional<double> groundLevel_mm)
	{
		cPlotPointCloud result;

		auto it = std::remove_if(points.begin(), points.end(), [z_mm](auto a)
			{
				return a.z_mm < z_mm;
			});

		points.erase(it, points.end());

		result.assign(points);

		result.setVegetationOnly(vegetationOnly);

		if (groundLevel_mm.has_value())
			result.setGroundLevel_mm(groundLevel_mm.value());
		else
			result.clearGroundLevel_mm();
	
		return result;
	}

	template<typename POINT>
	cPlotPointCloud trim_above(std::vector<POINT> points, int z_mm, bool vegetationOnly, std::optional<double> groundLevel_mm)
	{
		cPlotPointCloud result;

		auto it = std::remove_if(points.begin(), points.end(), [z_mm](auto a)
			{
				return a.z_mm > z_mm;
			});

		points.erase(it, points.end());

		result.assign(points);

		result.setVegetationOnly(vegetationOnly);

		if (groundLevel_mm.has_value())
			result.setGroundLevel_mm(groundLevel_mm.value());
		else
			result.clearGroundLevel_mm();

		return result;
	}
}

cPlotPointCloud plot::trim_outside(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box)
{
	return ::trim_outside(pc.data(), box, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::trim_outside(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box)
{
	return ::trim_outside(pc.data(), box, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::trim_outside(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	return ::trim_outside(points, box, vegetationOnly, groundLevel_mm);
}

cPlotPointCloud plot::trim_outside(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	return ::trim_outside(points, box, vegetationOnly, groundLevel_mm);
}

cPlotPointCloud plot::trim_below(const cRappPointCloud& pc, int z_mm)
{
	return ::trim_below(to_plot_points(pc.data()), z_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::trim_below(const cPlotPointCloud& pc, int z_mm)
{
	return ::trim_below(pc.data(), z_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

void plot::trim_below_in_place(cPlotPointCloud& pc, int z_mm)
{
	auto points = pc.data();

	auto it = std::remove_if(points.begin(), points.end(), [z_mm](auto a)
		{
			return a.z_mm < z_mm;
		});

	points.erase(it, points.end());

	pc.assign(points);

	pc.recomputeBounds();
}

cPlotPointCloud plot::trim_above(const cRappPointCloud& pc, int z_mm)
{
	return ::trim_above(to_plot_points(pc.data()), z_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::trim_above(const cPlotPointCloud& pc, int z_mm)
{
	return ::trim_above(pc.data(), z_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

void plot::translate(cPlotPointCloud& pc, int dx_mm, int dy_mm, int dz_mm)
{
	for (auto& point : pc)
	{
		point.x_mm += dx_mm;
		point.y_mm += dy_mm;
		point.z_mm += dz_mm;
	}

	pc.recomputeBounds();
}

std::vector<cPlotPointCloud::value_type> plot::sliceAtGivenX(const cPlotPointCloud& pc, double x_mm, double tolerance_mm)
{
	std::vector<cPlotPointCloud::value_type> result;
	if (pc.empty())
		return result;

	double x_min = x_mm - tolerance_mm;
	double x_max = x_mm + tolerance_mm;

	std::copy_if(pc.begin(), pc.end(), std::back_inserter(result), [x_min, x_max](auto a)
		{ return (x_min < a.x_mm) && (a.x_mm < x_max); });

	return result;
}

std::vector<cPlotPointCloud::value_type> plot::sliceAtGivenY(const cPlotPointCloud& pc, double y_mm, double tolerance_mm)
{
	std::vector<cPlotPointCloud::value_type> result;
	if (pc.empty())
		return result;

	double y_min = y_mm - tolerance_mm;
	double y_max = y_mm + tolerance_mm;

	std::copy_if(pc.begin(), pc.end(), std::back_inserter(result), [y_min, y_max](auto a)
		{ return (y_min < a.y_mm) && (a.y_mm < y_max); });

	return result;
}

cPlotPointCloud plot::isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box)
{
	return isolate_basic(pc.data(), box, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::isolate_basic(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box)
{
	return isolate_basic(pc.data(), box, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::isolate_basic(const std::vector<plot::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	auto result = trim_outside(pc, box, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();
	return result;
}

cPlotPointCloud plot::isolate_basic(const std::vector<rfm::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	auto result = trim_outside(pc, box, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();
	return result;
}

std::vector<cPlotPointCloud> plot::isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm)
{
	return isolate_basic(pc.data(), box, numSubPlots, orientation, plot_width_mm, plot_length_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

std::vector<cPlotPointCloud> plot::isolate_basic(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm)
{
	return isolate_basic(pc.data(), box, numSubPlots, orientation, plot_width_mm, plot_length_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

std::vector<cPlotPointCloud> plot::isolate_basic(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	int half_length_mm = plot_length_mm / 2;
	int half_width_mm = plot_width_mm / 2;

	auto temp = trim_outside(points, box, vegetationOnly, groundLevel_mm);
	temp.recomputeBounds();

	std::vector<cPlotPointCloud> result;

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

		temp = trim_outside(temp, plot_bounds);
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

std::vector<cPlotPointCloud> plot::isolate_basic(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	std::vector<plot::sPoint3D_t> plotPoints(points.size());

	std::transform(points.begin(), points.end(), plotPoints.begin(), to_plot_point);

	return isolate_basic(plotPoints, box, numSubPlots, orientation, plot_width_mm, plot_length_mm, vegetationOnly, groundLevel_mm);
}


cPlotPointCloud plot::isolate_center_of_plot(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm)
{
	return isolate_center_of_plot(pc.data(), box, plot_width_mm, plot_length_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::isolate_center_of_plot(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm)
{
	return isolate_center_of_plot(pc.data(), box, plot_width_mm, plot_length_mm, pc.vegetationOnly(), pc.groundLevel_mm());
}

cPlotPointCloud plot::isolate_center_of_plot(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	rfm::sPlotBoundingBox_t plot = compute_bounding_box_center_of_plot(box, plot_width_mm, plot_length_mm);

	auto result = ::trim_outside(points, plot, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	return result;
}

cPlotPointCloud plot::isolate_center_of_plot(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm)
{
	rfm::sPlotBoundingBox_t plot = compute_bounding_box_center_of_plot(box, plot_width_mm, plot_length_mm);

	auto result = ::trim_outside(points, plot, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	return result;
}


cPlotPointCloud plot::isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
{
	return isolate_center_of_height(pc.data(), box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);
}

cPlotPointCloud plot::isolate_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct, double max_displacement_pct)
{
	return isolate_center_of_height(pc.data(), box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);
}

cPlotPointCloud plot::isolate_center_of_height(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm, double height_threshold_pct, double max_displacement_pct)
{
	rfm::sPlotBoundingBox_t plot = ::compute_bounding_box_center_of_height(points, box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);

	auto result = trim_outside(points, plot, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	return result;
}

cPlotPointCloud plot::isolate_center_of_height(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm, double height_threshold_pct, double max_displacement_pct)
{
	rfm::sPlotBoundingBox_t plot = ::compute_bounding_box_center_of_height(points, box, plot_width_mm, plot_length_mm, height_threshold_pct, max_displacement_pct);

	auto result = trim_outside(points, plot, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	return result;
}


cPlotPointCloud plot::isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	const cRappPointCloud& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
	double height_threshold_pct, double max_displacement_pct)
{
	double width_mm = ((box.southEastCorner.x_mm - box.northEastCorner.x_mm) + (box.southWestCorner.x_mm - box.northWestCorner.x_mm)) / 2.0;
	double length_mm = ((box.northEastCorner.y_mm - box.northWestCorner.y_mm) + (box.southEastCorner.y_mm - box.southWestCorner.y_mm)) / 2.0;

	double max_shift_x_mm = ((max_displacement_pct / 100.0) * width_mm) / 2.0;
	double max_shift_y_mm = ((max_displacement_pct / 100.0) * length_mm) / 2.0;

	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	cPlotPointCloud result = trim_outside(full_pc, box);
	result.recomputeBounds();

	auto c0 = result.center();

	auto c1 = plot::compute_center_of_height(result, height_threshold_pct);

	auto dx = c0.x_mm - c1.x_mm;
	auto dy = c0.y_mm - c1.y_mm;

	if (std::abs(dx) > max_shift_x_mm)
	{
		if (dx < 0)
			c1.x_mm = c0.x_mm + max_shift_x_mm;
		else
			c1.x_mm = c0.x_mm - max_shift_x_mm;
	}


	if (std::abs(dy) > max_shift_y_mm)
	{
		if (dx < 0)
			c1.y_mm = c0.y_mm + max_shift_y_mm;
		else
			c1.y_mm = c0.y_mm - max_shift_y_mm;
	}


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

cPlotPointCloud plot::isolate_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
	const cPlotPointCloud& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
	double height_threshold_pct, double max_displacement_pct)
{
	double width_mm = ((box.southEastCorner.x_mm - box.northEastCorner.x_mm) + (box.southWestCorner.x_mm - box.northWestCorner.x_mm)) / 2.0;
	double length_mm = ((box.northEastCorner.y_mm - box.northWestCorner.y_mm) + (box.southEastCorner.y_mm - box.southWestCorner.y_mm)) / 2.0;

	double max_shift_x_mm = ((max_displacement_pct / 100.0) * width_mm) / 2.0;
	double max_shift_y_mm = ((max_displacement_pct / 100.0) * length_mm) / 2.0;

	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	auto result = trim_outside(full_pc, box);
	result.recomputeBounds();

	auto c0 = result.center();

	auto c1 = compute_center_of_height(result, height_threshold_pct);

	auto dx = c0.x_mm - c1.x_mm;
	auto dy = c0.y_mm - c1.y_mm;

	if (std::abs(dx) > max_shift_x_mm)
	{
		if (dx < 0)
			c1.x_mm = c0.x_mm + max_shift_x_mm;
		else
			c1.x_mm = c0.x_mm - max_shift_x_mm;
	}


	if (std::abs(dy) > max_shift_y_mm)
	{
		if (dx < 0)
			c1.y_mm = c0.y_mm + max_shift_y_mm;
		else
			c1.y_mm = c0.y_mm - max_shift_y_mm;
	}


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

cPlotPointCloud plot::isolate_center_of_height(const std::vector<plot::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box,
	const std::vector<plot::sPoint3D_t>& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
	double height_threshold_pct, double max_displacement_pct)
{
	double width_mm = ((box.southEastCorner.x_mm - box.northEastCorner.x_mm) + (box.southWestCorner.x_mm - box.northWestCorner.x_mm)) / 2.0;
	double length_mm = ((box.northEastCorner.y_mm - box.northWestCorner.y_mm) + (box.southEastCorner.y_mm - box.southWestCorner.y_mm)) / 2.0;

	double max_shift_x_mm = ((max_displacement_pct / 100.0) * width_mm) / 2.0;
	double max_shift_y_mm = ((max_displacement_pct / 100.0) * length_mm) / 2.0;

	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	auto result = trim_outside(full_pc, box, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	auto c0 = result.center();

	auto c1 = compute_center_of_height(result, height_threshold_pct);

	auto dx = c0.x_mm - c1.x_mm;
	auto dy = c0.y_mm - c1.y_mm;

	if (std::abs(dx) > max_shift_x_mm)
	{
		if (dx < 0)
			c1.x_mm = c0.x_mm + max_shift_x_mm;
		else
			c1.x_mm = c0.x_mm - max_shift_x_mm;
	}


	if (std::abs(dy) > max_shift_y_mm)
	{
		if (dx < 0)
			c1.y_mm = c0.y_mm + max_shift_y_mm;
		else
			c1.y_mm = c0.y_mm - max_shift_y_mm;
	}


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

	result = trim_outside(pc, plot, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	return result;
}

cPlotPointCloud plot::isolate_center_of_height(const std::vector<rfm::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box,
	const std::vector<rfm::sPoint3D_t>& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
	double height_threshold_pct, double max_displacement_pct)
{
	double width_mm = ((box.southEastCorner.x_mm - box.northEastCorner.x_mm) + (box.southWestCorner.x_mm - box.northWestCorner.x_mm)) / 2.0;
	double length_mm = ((box.northEastCorner.y_mm - box.northWestCorner.y_mm) + (box.southEastCorner.y_mm - box.southWestCorner.y_mm)) / 2.0;

	double max_shift_x_mm = ((max_displacement_pct / 100.0) * width_mm) / 2.0;
	double max_shift_y_mm = ((max_displacement_pct / 100.0) * length_mm) / 2.0;

	double half_width_mm = plot_width_mm / 2.0;
	double half_length_mm = plot_length_mm / 2.0;

	auto result = trim_outside(full_pc, box, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	auto c0 = result.center();

	auto c1 = compute_center_of_height(result, height_threshold_pct);

	auto dx = c0.x_mm - c1.x_mm;
	auto dy = c0.y_mm - c1.y_mm;

	if (std::abs(dx) > max_shift_x_mm)
	{
		if (dx < 0)
			c1.x_mm = c0.x_mm + max_shift_x_mm;
		else
			c1.x_mm = c0.x_mm - max_shift_x_mm;
	}


	if (std::abs(dy) > max_shift_y_mm)
	{
		if (dx < 0)
			c1.y_mm = c0.y_mm + max_shift_y_mm;
		else
			c1.y_mm = c0.y_mm - max_shift_y_mm;
	}


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

	result = trim_outside(pc, plot, vegetationOnly, groundLevel_mm);
	result.recomputeBounds();

	return result;
}


std::vector<cPlotPointCloud> plot::isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
	double height_threshold_pct)
{
	return isolate_center_of_height(pc.data(), box, numSubPlots, orientation, plot_width_mm, plot_length_mm,
		pc.vegetationOnly(), pc.groundLevel_mm(), height_threshold_pct);
}

std::vector<cPlotPointCloud> plot::isolate_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
	double height_threshold_pct)
{
	return isolate_center_of_height(pc.data(), box, numSubPlots, orientation, plot_width_mm, plot_length_mm, 
		pc.vegetationOnly(), pc.groundLevel_mm(), height_threshold_pct);
}

std::vector<cPlotPointCloud> plot::isolate_center_of_height(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
	double height_threshold_pct)
{
	int half_length_mm = plot_length_mm / 2;
	int half_width_mm = plot_width_mm / 2;

	auto temp = trim_outside(points, box, vegetationOnly, groundLevel_mm);
	temp.recomputeBounds();

	std::vector<cPlotPointCloud> result;

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

		auto plot = trim_outside(points, plot_bounds, vegetationOnly, groundLevel_mm);
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

		auto plot = trim_outside(points, plot_bounds, vegetationOnly, groundLevel_mm);
		plot.recomputeBounds();

		result.push_back(plot);
	}

	return result;
}

std::vector<cPlotPointCloud> plot::isolate_center_of_height(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
	double height_threshold_pct)
{
	std::vector<plot::sPoint3D_t> plotPoints;
	plotPoints.reserve(points.size());

	std::transform(points.begin(), points.end(), plotPoints.begin(), to_plot_point);

	return isolate_center_of_height(plotPoints, box, numSubPlots, orientation, plot_width_mm, plot_length_mm, vegetationOnly, groundLevel_mm, height_threshold_pct);
}


cPlotPointCloud plot::isolate_iterative(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
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

cPlotPointCloud plot::isolate_iterative(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
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


cPlotPointCloud plot::isolate_pour(const cRappPointCloud& pc)
{
	return to_plot_point_cloud(pc);
}

cPlotPointCloud plot::isolate_pour(const cPlotPointCloud& pc)
{
	return pc;
}


cPlotPointCloud plot::isolate_find_center(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
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

cPlotPointCloud plot::isolate_find_center(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
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


