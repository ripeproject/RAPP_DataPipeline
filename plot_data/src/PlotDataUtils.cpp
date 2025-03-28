
#define NOMINMAX


#include "PlotDataUtils.hpp"
#include "PlotSplitUtils.hpp"
#include "OctTree.hpp"

#include "Constants.hpp"

#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>
#include <algorithm>

#include <pcl/point_types.h>
#include <pcl/conversions.h>
#include <pcl/filters/voxel_grid.h>

//#include <open3d/Open3D.h>


namespace rfm
{
    struct sHeightPercentile_t
    {
        int32_t	height_mm = 0;
        int32_t count = 0;
    };
}


void nPlotUtils::removeHeightOutliers_Histogram(cPlotPointCloud& plot, int min_bin_count)
{
    std::vector<int> heights;

    for (const auto& point : plot)
    {
        heights.push_back(point.z_mm);
    }

    std::sort(heights.begin(), heights.end());

    auto min_height = heights.front();
    auto max_height = heights.back();

    float delta_height = (max_height - min_height) / 100.0;

    std::array<rfm::sHeightPercentile_t, 100> counts;

    for (int i = 1; i <= 100; ++i)
    {
        auto first = min_height + (i - 1) * delta_height;
        auto last = min_height + i * delta_height;

        int height_mm = static_cast<int>((first + last) / 2.0);

        std::vector<int>::iterator lb = std::lower_bound(heights.begin(), heights.end(), first);
        std::vector<int>::iterator ub = std::upper_bound(heights.begin(), heights.end(), last);

        auto d = std::distance(lb, ub);
        int count = static_cast<int>(d);
        counts[i - 1] = { height_mm, count };
    }

    int lowerBound_mm = plot.minZ_mm();
    int upperBound_mm = plot.maxZ_mm();

    int n = 99;
    for (int i = 0; i < n; ++i)
    {
        auto count = counts[i].count;

        if (count < min_bin_count)
            continue;

        lowerBound_mm = counts[i].height_mm;
        n = i;
        break;
    }

    for (int i = 99; i > n; --i)
    {
        auto count = counts[i].count;

        if (count < min_bin_count)
            continue;

        upperBound_mm = counts[i].height_mm;
        break;
    }

    plot = plot::trim_above(plot::trim_below(plot, lowerBound_mm), upperBound_mm);
}

double nPlotUtils::computePlotHeights(const cPlotPointCloud& plot, double plotHeight_pct)
{
    return computePlotHeights(plot, plotHeight_pct, plotHeight_pct - 1.0, plotHeight_pct + 1.0).height_mm;
}

nPlotUtils::sHeightResults_t nPlotUtils::computePlotHeights(const cPlotPointCloud& plot, double plotHeight_pct, double lowerBound_pct, double upperBound_pct)
{
    sHeightResults_t result;

    std::vector<int> heights;
    for (const auto& point : plot)
    {
        heights.push_back(point.z_mm);
    }

    if (heights.empty())
        return result;

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

double nPlotUtils::computePlotHeights(const cPlotPointCloud& plot, int groundHeight_mm, double plotHeight_pct)
{
    return computePlotHeights(plot, groundHeight_mm, plotHeight_pct, plotHeight_pct - 1.0, plotHeight_pct + 1.0).height_mm;
}

nPlotUtils::sHeightResults_t nPlotUtils::computePlotHeights(const cPlotPointCloud& plot, int groundHeight_mm, double plotHeight_pct, double lowerBound_pct, double upperBound_pct)
{
    sHeightResults_t result;

    if (plot.vegetationOnly())
        groundHeight_mm = 0;

    std::vector<int> heights;
    for (const auto& point : plot)
    {
        if (point.z_mm >= groundHeight_mm)
            heights.push_back(point.z_mm);
    }

    if (heights.empty())
        return result;

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

double nPlotUtils::computeDigitalBiomass_oct_tree(const cPlotPointCloud& plot, double voxel_size_mm, int min_bin_count)
{
    auto minX_mm = plot.minX_mm();
    auto maxX_mm = plot.maxX_mm();
    auto minY_mm = plot.minY_mm();
    auto maxY_mm = plot.maxY_mm();
    auto minZ_mm = plot.minZ_mm();
    auto maxZ_mm = plot.maxZ_mm();

    double mx = plot.maxX_mm() - plot.minX_mm();
    double my = plot.maxY_mm() - plot.minY_mm();
    double mz = plot.maxZ_mm() - plot.minZ_mm();

    double max_side = std::max(mx, my);
    max_side = std::max(max_side, mz);

    double half_size = max_side / 2.0;

    double half_size_voxel = (std::floor(half_size / voxel_size_mm) + 1) * voxel_size_mm;

    double cx = (plot.maxX_mm() + plot.minX_mm()) / 2.0;
    double cy = (plot.maxY_mm() + plot.minY_mm()) / 2.0;
    double cz = (plot.maxZ_mm() + plot.minZ_mm()) / 2.0;

    minX_mm = cx - half_size_voxel;
    maxX_mm = cx + half_size_voxel;
    minY_mm = cy - half_size_voxel;
    maxY_mm = cy + half_size_voxel;
    minZ_mm = cz - half_size_voxel;
    maxZ_mm = cz + half_size_voxel;

    std::unique_ptr<cOctTree> tree = std::make_unique<cOctTree>(minX_mm, maxX_mm, minY_mm, maxY_mm, minZ_mm, maxZ_mm, voxel_size_mm);

    for (const auto& point : plot)
    {
        tree->addPoint(point.x_mm, point.y_mm, point.z_mm);
    }

    double volume_mm3 = tree->volume_mm3(min_bin_count);

    double dx = plot.maxX_mm() - plot.minX_mm();
    double dy = plot.maxY_mm() - plot.minY_mm();

    double area_mm2 = dx * dy;

    double biomass = 0;

    if (area_mm2 > 0.0)
        biomass = volume_mm3 / area_mm2;

    return biomass;
}

double nPlotUtils::computeDigitalBiomass_pcl(const cPlotPointCloud& plot, double voxel_size_mm)
{
    auto num_points_in_cloud = plot.size();

    auto minX_mm = plot.minX_mm();
    auto maxX_mm = plot.maxX_mm();
    auto minY_mm = plot.minY_mm();
    auto maxY_mm = plot.maxY_mm();
    auto minZ_mm = plot.minZ_mm();
    auto maxZ_mm = plot.maxZ_mm();

    double mx = plot.maxX_mm() - plot.minX_mm();
    double my = plot.maxY_mm() - plot.minY_mm();
    double mz = plot.maxZ_mm() - plot.minZ_mm();

    double max_side = std::max(mx, my);
    max_side = std::max(max_side, mz);

    double half_size = max_side / 2.0;

    double half_size_voxel = (std::floor(half_size / voxel_size_mm) + 1) * voxel_size_mm;

    double cx = (plot.maxX_mm() + plot.minX_mm()) / 2.0;
    double cy = (plot.maxY_mm() + plot.minY_mm()) / 2.0;
    double cz = (plot.maxZ_mm() + plot.minZ_mm()) / 2.0;

    minX_mm = cx - half_size_voxel;
    maxX_mm = cx + half_size_voxel;
    minY_mm = cy - half_size_voxel;
    maxY_mm = cy + half_size_voxel;
    minZ_mm = cz - half_size_voxel;
    maxZ_mm = cz + half_size_voxel;

    std::unique_ptr<cOctTree> tree = std::make_unique<cOctTree>( minX_mm, maxX_mm, minY_mm, maxY_mm, minZ_mm, maxZ_mm, voxel_size_mm );

    pcl::PointCloud<pcl::PointXYZ>::Ptr points(new pcl::PointCloud<pcl::PointXYZ>);

    for (const auto& point : plot)
    {
        tree->addPoint(point.x_mm, point.y_mm, point.z_mm);
        points->emplace_back(pcl::PointXYZ(point.x_mm, point.y_mm, point.z_mm));
    }

    pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2());

    pcl::toPCLPointCloud2(*points, *cloud);

    pcl::PCLPointCloud2::Ptr voxels(new pcl::PCLPointCloud2());

    // Create the filtering object
    pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
    sor.setInputCloud(cloud);
    sor.setLeafSize(voxel_size_mm, voxel_size_mm, voxel_size_mm);
    sor.filter(*voxels);

    double single_voxel_volume_mm3 = voxel_size_mm * voxel_size_mm * voxel_size_mm;

    auto v = voxels->data;
    auto num_points_in_voxels = voxels->data.size();

    std::sort(v.begin(), v.end());

    auto last = std::unique(v.begin(), v.end());
    v.erase(last, v.end());

    auto m = v.size();

    auto n = voxels->height * voxels->width;

    double volume_mm3 = single_voxel_volume_mm3 * n;
    double test_volume = tree->volume_mm3(0);

    double dx = plot.maxX_mm() - plot.minX_mm();
    double dy = plot.maxY_mm() - plot.minY_mm();

    double area_mm2 = dx * dy;

    double biomass = 0;

    if (area_mm2 > 0.0)
        biomass = volume_mm3 / area_mm2;

    return biomass;
}

/*
double nPlotUtils::computeDigitalBiomass_open3d(const cPlotPointCloud& plot, double voxel_size)
{
    using namespace open3d::geometry;

    double maxX = plot.maxX_mm();
    double minX = plot.minX_mm();

    int nX = static_cast<int>((maxX - minX) / voxel_size);
    int nY = static_cast<int>((plot.maxY_mm() - plot.minY_mm()) / voxel_size);
    int nZ = static_cast<int>((plot.maxZ_mm() - plot.minZ_mm()) / voxel_size);

    std::vector<Eigen::Vector3d> points;

    for (const auto& point : plot)
    {
        points.emplace_back(Eigen::Vector3d(point.x_mm, point.y_mm, point.z_mm));
    }

    std::unique_ptr<PointCloud> pc = std::make_unique<PointCloud>(points);

    auto voxel_grid = VoxelGrid::CreateFromPointCloud(*pc, voxel_size);
    auto voxels = voxel_grid->GetVoxels();

    int mX = 0;
    int mY = 0;
    int mZ = 0;
    for (const auto& voxel : voxels)
    {
        if (voxel.grid_index_.x() > mX)
            mX = voxel.grid_index_.x();
        if (voxel.grid_index_.y() > mY)
            mY = voxel.grid_index_.y();
        if (voxel.grid_index_.z() > mZ)
            mZ = voxel.grid_index_.z();
    }

    auto min_bounds = voxel_grid->GetMinBound();
    auto max_bounds = voxel_grid->GetMaxBound();
    double single_voxel_volume = voxel_size * voxel_size * voxel_size;

    auto n = voxels.size();

    double volume = single_voxel_volume * voxels.size();

    double max_x = max_bounds.x();
    double min_x = min_bounds.x();
    double dx = max_x - min_x;

    double max_y = max_bounds.y();
    double min_y = min_bounds.y();
    double dy = max_y - min_y;

    double area = dx * dy;

    double biomass = 0;

    if (area > 0.0)
        biomass = volume / area;

    return biomass;
}
*/


