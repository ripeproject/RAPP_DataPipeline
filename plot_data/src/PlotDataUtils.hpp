
#pragma once

#include <cbdf/PlotPointCloud.hpp>

#include <cstdint>


namespace nPlotUtils
{
	void removeHeightOutliers_Histogram(cPlotPointCloud& plot, int min_bin_count);
	void removeHeightOutliers_Grubbs(cPlotPointCloud& plot, double alpha);

	struct sHeightResults_t
	{
		double lowerHeight_mm = 0.0;
		double height_mm = 0.0;
		double upperHeight_mm = 0.0;
	};

	double computePlotHeights(const cPlotPointCloud& plot, double plotHeight_pct);
	sHeightResults_t computePlotHeights(const cPlotPointCloud& plot, double plotHeight_pct, double lowerBound_pct, double upperBound_pct);

	double computeDigitalBiomass_oct_tree(const cPlotPointCloud& plot, double voxel_size_mm, int min_bin_count);


	struct sVoxelResults_t
	{
		double digitalBiomass = 0.0;
		unsigned int num_voxels = 0;
	};
	sVoxelResults_t computeDigitalBiomass_voxel_grid(const cPlotPointCloud& plot, double voxel_size_mm, int min_points_per_voxel = 0);

	struct sConvexHullResults_t
	{
		double digitalBiomass = 0.0;
		unsigned int num_convex_hull = 0;
	};
	sConvexHullResults_t computeDigitalBiomass_convex_hull(const cPlotPointCloud& plot);
	//	double computeDigitalBiomass_open3d(const cPlotPointCloud& plot, double voxel_size_mm);

	double computeLAI_lpi(const cPlotPointCloud& plot, double k, int groundLevel_mm);
}

