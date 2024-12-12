
#pragma once

#include "datatypes.hpp"
#include "PointCloudTypes.hpp"

#include "RappPointCloud.hpp"

#include <string>
#include <vector>


double distance_mm(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t p2);
double distance_mm(rfm::rappPoint_t p1, rfm::rappPoint_t p2);

/**
 * Computes the center of height (similiar to center of mass) of a point cloud.
 * The height threshold % will set the height level used in the computation.
 * A zero height threshold % means to include all points.
 */
rfm::rappPoint_t compute_center_of_height(const cRappPointCloud& pc, double height_threshold_pct = 0.0);

struct sLine_t
{
	double slope = 0;
	double intercept = 0;
	bool vertical = false;
};

sLine_t computeLineParameters(rfm::rappPoint2D_t p1, rfm::rappPoint2D_t, bool swapAxis = false);

cRappPointCloud trim_outside(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box);

std::vector<cRappPointCloud::value_type> sliceAtGivenX(const cRappPointCloud& pc, double x_mm, double tolerance_mm);

std::vector<cRappPointCloud::value_type> sliceAtGivenY(const cRappPointCloud& pc, double y_mm, double tolerance_mm);

cRappPointCloud isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box);

std::vector<cRappPointCloud> isolate_basic(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm);

cRappPointCloud isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box, 
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0);

std::vector<cRappPointCloud> isolate_center_of_height(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	int numSubPlots, ePlotOrientation orientation, std::int32_t plot_width_mm, std::int32_t plot_length_mm,
	double height_threshold_pct = 0);

cRappPointCloud isolate_iterative(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double tolerance_mm, double bound_pct = 50.0);

cRappPointCloud isolate_pour(const cRappPointCloud& pc);

cRappPointCloud isolate_find_center(const cRappPointCloud& pc, rfm::sPlotBoundingBox_t box,
	std::int32_t plot_width_mm, std::int32_t plot_length_mm, double height_threshold_pct = 0);
