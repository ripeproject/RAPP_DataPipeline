
#pragma once

/**
 *	Tower 1: northing_ft = 1237093.164, easting_ft = 1019287.146, elevation_ft = 710.708
 *  Tower 2: northing_ft = 1236468.119, easting_ft = 1019286.889, elevation_ft = 710.774
 *  tower 3: northing_ft = 1236468.156, easting_ft = 1019912.027, elevation_ft = 710.853
 *  tower 4: northing_ft = 1237093.252, easting_ft = 1019911.889, elevation_ft = 710.752
 * 
 *  reference point: northing_ft = 1236785.233, easting_ft = 1019358.757, elevation_ft = 708.045
 */

#include "datatypes.hpp"

#include <array>

// Forward Declarations

class cRappFieldBoundary
{
public:
	cRappFieldBoundary();

	bool withinBoundary(const rfm::planePoint_t& point) const;
	bool withinBoundary(const rfm::rappPoint_t& point) const;

	rfm::rappPoint_t toRappCoordinates(const rfm::planePoint_t& point) const;
	rfm::rappPoint_t toRappCoordinates(const double northing_ft, const double easting_ft, const double elevation_ft) const;

private:
	std::array<rfm::planePoint_t, 4> mUIUC_TowerLocations;
	const double mUIUC_ReferenceElevation_ft;

	std::array<rfm::rappPoint_t, 4> mRappTowerLocations;
};

