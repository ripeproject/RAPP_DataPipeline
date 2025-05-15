
#pragma once

#include "datatypes.hpp"
#include "PlotSplitDataTypes.hpp"

#include <cbdf/PlotPointCloud.hpp>

#include <string>
#include <vector>
#include <optional>

// Forward Declarations
class cRappPointCloud;


plot::sPoint3D_t to_plot_point(const rfm::sPoint3D_t& point);
rfm::sPoint3D_t to_rfm_point(const plot::sPoint3D_t& point);

std::vector<plot::sPoint3D_t> to_plot_points(const std::vector<rfm::sPoint3D_t>& points);
std::vector<rfm::sPoint3D_t>  to_rfm_points(const std::vector<plot::sPoint3D_t>& points);

cPlotPointCloud to_plot_point_cloud(const cRappPointCloud& pc);


namespace plot
{
	double distance_mm(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t p2);
	double distance_mm(rfm::rappPoint_t p1, rfm::rappPoint_t p2);

	bool contains(const cRappPointCloud& pc, const rfm::sPlotBoundingBox_t& box);
	bool contains(const cPlotPointCloud& pc, const rfm::sPlotBoundingBox_t& box);


	/**
	 * Computes the center of height (similiar to center of mass) of a point cloud.
	 * The height threshold % will set the height level used in the computation.
	 * A zero height threshold % means to include all points.
	 */
	rfm::rappPoint_t compute_center_of_height(const cRappPointCloud& pc, double height_threshold_pct = 0.0);
	rfm::rappPoint_t compute_center_of_height(const cPlotPointCloud& pc, double height_threshold_pct = 0.0);
	rfm::rappPoint_t compute_center_of_height(const std::vector<plot::sPoint3D_t>& points, double height_threshold_pct = 0.0);
	rfm::rappPoint_t compute_center_of_height(const std::vector<rfm::sPoint3D_t>& points, double height_threshold_pct = 0.0);

	rfm::sPlotBoundingBox_t compute_bounding_box_center_of_plot(rfm::sPlotBoundingBox_t box, std::int32_t plot_width_mm, std::int32_t plot_length_mm);

	rfm::sPlotBoundingBox_t compute_bounding_box_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	rfm::sPlotBoundingBox_t compute_bounding_box_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	rfm::sPlotBoundingBox_t compute_bounding_box_center_of_height(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	rfm::sPlotBoundingBox_t compute_bounding_box_center_of_height(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0, double max_displacement_pct = 25.0);

	struct sLine_t
	{
		double slope = 0;
		double intercept = 0;
		bool vertical = false;
	};

	sLine_t computeLineParameters(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t, bool swapAxis = false);

	cPlotPointCloud trim_outside(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box);
	cPlotPointCloud trim_outside(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box);
	cPlotPointCloud trim_outside(const std::vector<plot::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm);
	cPlotPointCloud trim_outside(const std::vector<rfm::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm);

	cPlotPointCloud trim_below(const cRappPointCloud& pc, int z_mm);
	cPlotPointCloud trim_below(const cPlotPointCloud& pc, int z_mm);

	void trim_below_in_place(cPlotPointCloud& pc, int z_mm);

	cPlotPointCloud trim_above(const cRappPointCloud& pc, int z_mm);
	cPlotPointCloud trim_above(const cPlotPointCloud& pc, int z_mm);

	void trim_above_in_place(cPlotPointCloud& pc, int z_mm);

	void translate(cPlotPointCloud& pc, int dx_mm, int dy_mm, int dz_mm);


	std::vector<cPlotPointCloud::value_type> sliceAtGivenX(const cPlotPointCloud& pc, double x_mm, double tolerance_mm);

	std::vector<cPlotPointCloud::value_type> sliceAtGivenY(const cPlotPointCloud& pc, double y_mm, double tolerance_mm);

	cPlotPointCloud isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box);
	cPlotPointCloud isolate_basic(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box);
	cPlotPointCloud isolate_basic(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm);
	cPlotPointCloud isolate_basic(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box, bool vegetationOnly, std::optional<double> groundLevel_mm);

	std::vector<cPlotPointCloud> isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm);
	std::vector<cPlotPointCloud> isolate_basic(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm);
	std::vector<cPlotPointCloud> isolate_basic(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm);
	std::vector<cPlotPointCloud> isolate_basic(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm);


	cPlotPointCloud isolate_center_of_plot(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm);
	cPlotPointCloud isolate_center_of_plot(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm);
	cPlotPointCloud isolate_center_of_plot(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm);
	cPlotPointCloud isolate_center_of_plot(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm);


	cPlotPointCloud isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0,
		double max_displacement_pct = 25.0);
	cPlotPointCloud isolate_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0,
		double max_displacement_pct = 25.0);
	cPlotPointCloud isolate_center_of_height(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
		double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	cPlotPointCloud isolate_center_of_height(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
		double height_threshold_pct = 0, double max_displacement_pct = 25.0);


	cPlotPointCloud isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		const cRappPointCloud& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
		double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	cPlotPointCloud isolate_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		const cPlotPointCloud& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
		double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	cPlotPointCloud isolate_center_of_height(const std::vector<plot::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box,
		const std::vector<plot::sPoint3D_t>& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
		double height_threshold_pct = 0, double max_displacement_pct = 25.0);
	cPlotPointCloud isolate_center_of_height(const std::vector<rfm::sPoint3D_t>& pc, rfm::sPlotBoundingBox_t box,
		const std::vector<rfm::sPoint3D_t>& full_pc, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
		double height_threshold_pct = 0, double max_displacement_pct = 25.0);


	std::vector<cPlotPointCloud> isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
		double height_threshold_pct = 0);
	std::vector<cPlotPointCloud> isolate_center_of_height(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
		double height_threshold_pct = 0);
	std::vector<cPlotPointCloud> isolate_center_of_height(const std::vector<plot::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
		double height_threshold_pct = 0);
	std::vector<cPlotPointCloud> isolate_center_of_height(const std::vector<rfm::sPoint3D_t>& points, rfm::sPlotBoundingBox_t box,
		int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm, bool vegetationOnly, std::optional<double> groundLevel_mm,
		double height_threshold_pct = 0);

	cPlotPointCloud isolate_iterative(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double tolerance_mm, double bound_pct = 50.0);
	cPlotPointCloud isolate_iterative(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double tolerance_mm, double bound_pct = 50.0);

	cPlotPointCloud isolate_pour(const cRappPointCloud& pc);
	cPlotPointCloud isolate_pour(const cPlotPointCloud& pc);

	cPlotPointCloud isolate_find_center(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0);
	cPlotPointCloud isolate_find_center(const cPlotPointCloud& pc, rfm::sPlotBoundingBox_t box,
		std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0);
}