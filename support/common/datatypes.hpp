
#pragma once

#include <cstdint>

// Forward Declarations
namespace pointcloud
{
	struct sCloudPoint_t;
	struct sCloudPoint_FrameID_t;
	struct sCloudPoint_SensorInfo_t;
}


namespace rfm
{
	constexpr std::int32_t INVALID_HEIGHT = -999999;

	struct planePoint_t
	{
		double northing_ft = 0.0;
		double easting_ft = 0.0;
		double elevation_ft = 0.0;

		planePoint_t() = default;
		planePoint_t(double north_ft, double east_ft, double ele_ft )
			: northing_ft(north_ft), easting_ft(east_ft), elevation_ft(ele_ft)
		{}
	};

	struct rappPoint2D_t
	{
		std::int32_t x_mm = 0;
		std::int32_t y_mm = 0;

		bool operator==(const rappPoint2D_t& rhs) const
		{
			return (x_mm == rhs.x_mm) && (y_mm == rhs.y_mm);
		}
	};

	struct rappPoint_t
	{
		std::int32_t x_mm = 0;
		std::int32_t y_mm = 0;
		std::int32_t z_mm = 0;

		bool operator==(const rappPoint_t& rhs) const
		{
			return (x_mm == rhs.x_mm) && (y_mm == rhs.y_mm)
				&& (z_mm == rhs.z_mm);
		}

		bool operator!=(const rappPoint_t& rhs) const
		{
			return !operator==(rhs);
		}
	};

	struct rappPoint2_t : public rappPoint_t
	{
		std::int32_t h_mm = INVALID_HEIGHT;

		rappPoint2_t operator=(const rappPoint_t& rhs)
		{
			x_mm = rhs.x_mm;
			y_mm = rhs.y_mm;
			z_mm = rhs.z_mm;

			h_mm = INVALID_HEIGHT;

			return *this;
		}
	};

	struct rappSpeeds_t
	{
		double vx_mmps = 0.0;
		double vy_mmps = 0.0;
		double vz_mmps = 0.0;
	};

	struct sCentroid_t
	{
		double x_mm = 0.0;
		double y_mm = 0.0;
		double z_mm = 0.0;
	};

	struct sPoint3D_t
	{
		int32_t  x_mm = 0;
		int32_t  y_mm = 0;
		int32_t  z_mm = 0;
		int32_t  h_mm = INVALID_HEIGHT;
		uint32_t range_mm = 0;
		uint16_t signal = 0;
		uint16_t reflectivity = 0;
		uint16_t nir = 0;
		uint16_t frameID = 0;
		uint16_t chnNum = 0;
		uint16_t pixelNum = 0;

		sPoint3D_t& operator=(const pointcloud::sCloudPoint_t& rhs);
		sPoint3D_t& operator=(const pointcloud::sCloudPoint_FrameID_t& rhs);
		sPoint3D_t& operator=(const pointcloud::sCloudPoint_SensorInfo_t& rhs);
	};

}

