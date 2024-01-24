
#include "RappFieldBoundary.hpp"

#include "Constants.hpp"


cRappFieldBoundary::cRappFieldBoundary() : mUIUC_ReferenceElevation_ft(708.045)
{
	mUIUC_TowerLocations[0] = { 1237093.164, 1019287.146, 710.708 };
	mUIUC_TowerLocations[1] = { 1236468.119, 1019286.889, 710.774 };
	mUIUC_TowerLocations[2] = { 1236468.156, 1019912.027, 710.853 };
	mUIUC_TowerLocations[3] = { 1237093.252, 1019911.889, 710.752 };

	mRappTowerLocations[0] = { 0, 0, 
		static_cast<std::int32_t>((mUIUC_TowerLocations[0].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	mRappTowerLocations[1] = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - mUIUC_TowerLocations[1].northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((mUIUC_TowerLocations[1].easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((mUIUC_TowerLocations[1].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	mRappTowerLocations[2] = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - mUIUC_TowerLocations[2].northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((mUIUC_TowerLocations[2].easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((mUIUC_TowerLocations[2].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	mRappTowerLocations[3] = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - mUIUC_TowerLocations[3].northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((mUIUC_TowerLocations[3].easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((mUIUC_TowerLocations[3].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };
}


bool cRappFieldBoundary::withinBoundary(const rfm::planePoint_t& point) const
{
	return true;
}

bool cRappFieldBoundary::withinBoundary(const rfm::rappPoint_t& point) const
{
	return true;
}

rfm::rappPoint_t cRappFieldBoundary::toRappCoordinates(const rfm::planePoint_t& point) const
{
	rfm::rappPoint_t result;

	result = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - point.northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((point.easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((point.elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	return result;
}

rfm::rappPoint_t cRappFieldBoundary::toRappCoordinates(const double northing_ft, const double easting_ft, const double elevation_ft) const
{
	rfm::rappPoint_t result;

	result = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	return result;
}
