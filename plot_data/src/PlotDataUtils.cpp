
#define NOMINMAX
#define USE_PCL

#include "PlotDataUtils.hpp"

#include "Constants.hpp"

#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>
#include <algorithm>

#ifdef USE_PCL
    #include <pcl/point_types.h>
    #include <pcl/conversions.h>
    #include <pcl/filters/voxel_grid.h>
#else
    #include <open3d/Open3D.h>
#endif


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

#ifdef USE_PCL
    double nPlotUtils::computeDigitalBiomass(const cPlotPointCloud& plot, int groundHeight_mm, double voxel_size)
    {
        pcl::PointCloud<pcl::PointXYZ>::Ptr points(new pcl::PointCloud<pcl::PointXYZ>);

        for (const auto& point : plot)
        {
            if (point.z_mm > groundHeight_mm)
            {
                points->emplace_back(pcl::PointXYZ(point.x_mm, point.y_mm, point.z_mm));
            }
        }

        pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2());

        pcl::toPCLPointCloud2(*points, *cloud);

        pcl::PCLPointCloud2::Ptr voxels(new pcl::PCLPointCloud2());

        // Create the filtering object
        pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
        sor.setInputCloud(cloud);
        sor.setLeafSize(voxel_size, voxel_size, voxel_size);
        sor.filter(*voxels);

        double single_voxel_volume = voxel_size * voxel_size * voxel_size;

        auto n = voxels->height * voxels->width;

        double volume = single_voxel_volume * n;

        double dx = plot.maxX_mm() - plot.minX_mm();
        double dy = plot.maxY_mm() - plot.minY_mm();

        double area = dx * dy;

        double biomass = 0;

        if (area > 0.0)
            biomass = volume / area;

        return biomass;
    }
#else
    double nPlotUtils::computeDigitalBiomass(const cPlotPointCloud& plot, int groundHeight_mm, double voxel_size)
    {
        using namespace open3d::geometry;

        double maxX = plot.maxX_mm();
        double minX = plot.minX_mm();

        int nX = static_cast<int>((maxX- minX)/ voxel_size);
        int nY = static_cast<int>((plot.maxY_mm() - plot.minY_mm()) / voxel_size);
        int nZ = static_cast<int>((plot.maxZ_mm() - plot.minZ_mm()) / voxel_size);

        std::vector<Eigen::Vector3d> points;

        for (const auto& point : plot)
        {
            if (point.z_mm > groundHeight_mm)
            {
                points.emplace_back(Eigen::Vector3d(point.x_mm, point.y_mm, point.z_mm));
            }
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
#endif
