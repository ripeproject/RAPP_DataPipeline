
#pragma once

#include <cbdf/PlotPointCloud.hpp>

#include <cstdint>


namespace nPlotUtils
{
	void removeHeightOutliers_Histogram(cPlotPointCloud& plot, int min_bin_count);

	struct sHeightResults_t
	{
		double lowerHeight_mm = 0.0;
		double height_mm = 0.0;
		double upperHeight_mm = 0.0;
	};

	double computePlotHeights(const cPlotPointCloud& plot, double plotHeight_pct);
	sHeightResults_t computePlotHeights(const cPlotPointCloud& plot, double plotHeight_pct, double lowerBound_pct, double upperBound_pct);

	double computePlotHeights(const cPlotPointCloud& plot, int groundHeight_mm, double plotHeight_pct);
	sHeightResults_t computePlotHeights(const cPlotPointCloud& plot, int groundHeight_mm, double plotHeight_pct, double lowerBound_pct, double upperBound_pct);

	double computeDigitalBiomass_oct_tree(const cPlotPointCloud& plot, double voxel_size_mm, int min_bin_count);
	double computeDigitalBiomass_pcl(const cPlotPointCloud& plot, double voxel_size_mm);
//	double computeDigitalBiomass_open3d(const cPlotPointCloud& plot, double voxel_size_mm);
}

