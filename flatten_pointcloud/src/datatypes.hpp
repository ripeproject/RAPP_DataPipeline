
#pragma once

#include <cstdint>


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

	struct rappPoint_t
	{
		std::int32_t x_mm = 0;
		std::int32_t y_mm = 0;
		std::int32_t z_mm = 0;
	};

	struct rappPoint2_t
	{
		std::int32_t x_mm = 0;
		std::int32_t y_mm = 0;
		std::int32_t z_mm = 0;
		std::int32_t h_mm = INVALID_HEIGHT;
	};

	struct sCentroid_t
	{
		double x_mm = 0.0;
		double y_mm = 0.0;
		double z_mm = 0.0;
	};

}

