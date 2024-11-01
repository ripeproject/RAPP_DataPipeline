
#pragma once

#include "PlotPointCloud.hpp"

#include <cstdint>


namespace nPlotUtils
{
	struct sHeightResults_t
	{
		double lowerHeight_mm = 0.0;
		double height_mm = 0.0;
		double upperHeight_mm = 0.0;
	};

	sHeightResults_t computePlotHeights(const cPlotPointCloud& plot, int groundHeight_mm, double plotHeight_pct, double lowerBound_pct, double upperBound_pct);

	double computeDigitalBiomass(const cPlotPointCloud& plot, int groundHeight_mm, double voxel_size);
}

