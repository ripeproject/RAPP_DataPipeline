
#pragma once

#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>
#include <array>

namespace pointcloud
{
    //
    // A bounding box data struct used in various utilities.
    //
    struct sBoundingBox_t
    {
        std::array<sPoint2D_t, 4> points;
    };
}