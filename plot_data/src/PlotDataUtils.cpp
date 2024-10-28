
#include "PlotDataUtils.hpp"

#include "PointCloudTypes.hpp"
#include "Constants.hpp"

#include <cstdint>
#include <algorithm>


nPlotUtils::sHeightResults_t nPlotUtils::computePlotHeights(const cPlotPointCloud& plot, int groundHeight_mm, double plotHeight_pct, double lowerBound_pct, double upperBound_pct)
{
    sHeightResults_t result;

    std::vector<int> heights;
    for (const auto& point : plot)
    {
        if (point.z_mm >= groundHeight_mm)
            heights.push_back(point.z_mm);
    }

    std::sort(heights.begin(), heights.end());

    auto n = heights.size();

    int i = static_cast<int>(n * (plotHeight_pct / 100.0));
    int l = static_cast<int>(n * (lowerBound_pct / 100.0));
    int u = static_cast<int>(n * (upperBound_pct / 100.0));

    result.height_mm = heights[i];
    result.lowerHeight_mm = heights[l];
    result.upperHeight_mm = heights[u];

	return result;
}

