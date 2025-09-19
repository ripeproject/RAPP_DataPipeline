#pragma once

#include "datatypes.hpp"

#include "RappPlot.hpp"
#include "PlotSplitDataTypes.hpp"
#include "PointCloudUtilTypes.hpp"
#include "PlotConfigScan.hpp"
#include "PlotConfigExclusion.hpp"
#include "PlotConfigInclusion.hpp"

#include <vector>
#include <memory>


class cRappPointCloud;


namespace rfm
{
	struct sHeightPercentile_t
	{
		int32_t	height_mm = 0;
		int32_t count = 0;
	};
}


namespace rapp
{
	pointcloud::sBoundingBox_t to_bounding_box(const rfm::sPlotBoundingBox_t box);

	std::unique_ptr<cRappPlot> do_plot_split(const cRappPointCloud& pc, const cPlotConfigPlotInfo& plotInfo,
		const cRappPointCloud* fullPointCloud = nullptr);

	std::vector<std::unique_ptr<cRappPlot>> do_plot_split(const cRappPointCloud& pc, cPlotConfigScan::iterator first,
		const cPlotConfigScan::iterator last, const cRappPointCloud* fullPointCloud = nullptr);

	void remove_exclusions(cPlotPointCloud& pc, int32_t plot_origin_x_mm, int32_t plot_origin_y_mm,
		std::vector<cPlotConfigExclusion>::const_iterator first, const std::vector<cPlotConfigExclusion>::const_iterator last);
};

