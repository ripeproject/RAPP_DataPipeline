
#pragma once

#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>
#include <array>

namespace pointcloud
{
	struct sPointCloudTranslationInterpPoint_t
	{
		double displacement_m = 0.0;
		double height_m = 0.0;

		bool operator==(const sPointCloudTranslationInterpPoint_t& rhs) const
		{
			return (displacement_m == rhs.displacement_m) && (height_m == rhs.height_m);
		}

		bool operator!=(const sPointCloudTranslationInterpPoint_t& rhs) const
		{
			return !operator==(rhs);
		}
	};

	struct sPointCloudRotationInterpPoint_t
	{
		double displacement_m = 0.0;
		double pitch_deg = 0.0;
		double roll_deg = 0.0;

		bool operator==(const sPointCloudRotationInterpPoint_t& rhs) const
		{
			return (displacement_m == rhs.displacement_m) && (pitch_deg == rhs.pitch_deg)
				&& (roll_deg == rhs.roll_deg);
		}

		bool operator!=(const sPointCloudRotationInterpPoint_t& rhs) const
		{
			return !operator==(rhs);
		}
	};
	
	//
    // A bounding box data struct used in various utilities.
    //
    struct sBoundingBox_t
    {
        std::array<sPoint2D_t, 4> corners;
    };

	//
	// A bounding circle data struct used in various utilities.
	//
	struct sBoundingCircle_t
	{
		sPoint2D_t center;
		double radius_m = 0.0;
	};
}