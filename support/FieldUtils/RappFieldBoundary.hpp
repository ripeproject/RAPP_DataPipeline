
#pragma once

/**
 *	Tower 1: northing_ft = 1237093.164, easting_ft = 1019287.146, elevation_ft = 710.708
 *  Tower 2: northing_ft = 1236468.119, easting_ft = 1019286.889, elevation_ft = 710.774
 *  Tower 3: northing_ft = 1236468.156, easting_ft = 1019912.027, elevation_ft = 710.853
 *  Tower 4: northing_ft = 1237093.252, easting_ft = 1019911.889, elevation_ft = 710.752
 * 
 *  Tower 1: lat = 40.0635686°, lng = -088.2081615°
 *  Tower 2: lat = 40.0635664°, lng = -088.2059295°
 *  Tower 3: lat = 40.0618528°, lng = -088.2081656°
 *  Tower 4: lat = 40.0618504°, lng = -088.2059322°
 * 
 *  reference point: northing_ft = 1236785.233, easting_ft = 1019358.757, elevation_ft = 708.045
 */

#include "datatypes.hpp"

#include <array>

// Forward Declarations

namespace rfb
{
	uint32_t minX_mm();
	uint32_t maxX_mm();

	uint32_t minY_mm();
	uint32_t maxY_mm();

	bool withinBoundary(const rfm::planePoint_t& point);
	bool withinBoundary(const rfm::rappPoint_t& point);

	rfm::rappPoint_t toRappCoordinates(const rfm::planePoint_t& point);
	rfm::rappPoint_t fromStatePlane( double northing_ft, const double easting_ft, const double elevation_ft);
	rfm::rappPoint_t fromGPS(const double lat_rad, const double lng_rad, const double height_m);
};

