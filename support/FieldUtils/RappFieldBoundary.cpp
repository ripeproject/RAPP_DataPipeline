
#include "RappFieldBoundary.hpp"

#include "Constants.hpp"


#include <proj/coordinateoperation.hpp>
#include <proj/crs.hpp>
#include <proj/io.hpp>
#include <proj/util.hpp> // for nn_dynamic_pointer_cast

using namespace NS_PROJ::crs;
using namespace NS_PROJ::io;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;

#include <array>
#include <stdexcept>

namespace
{
//	constexpr double Re_lat_m = 6361721.0810512137;
//	constexpr double Re_lat_ft = Re_lat_m * nConstants::M_TO_FT;

//	constexpr double Re_lng_m = 4888165.4030188089;
//	constexpr double Re_lng_ft = Re_lng_m * nConstants::M_TO_FT;

	const std::array<rfm::planePoint_t, 4> mUIUC_TowerLocations = {
		rfm::planePoint_t(1237093.164, 1019287.146, 710.708),
		rfm::planePoint_t(1236468.119, 1019286.889, 710.774),
		rfm::planePoint_t(1236468.156, 1019912.027, 710.853),
		rfm::planePoint_t(1237093.252, 1019911.889, 710.752) };

	constexpr double mUIUC_ReferenceElevation_ft = 708.045;
	constexpr double mUIUC_ReferenceElevation_m = mUIUC_ReferenceElevation_ft * nConstants::FT_TO_M;

	const std::array<rfm::rappPoint_t, 4> mRappTowerLocations = {
		rfm::rappPoint_t(0, 
			0, 
			static_cast<std::int32_t>((mUIUC_TowerLocations[0].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM)),

		rfm::rappPoint_t(static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - mUIUC_TowerLocations[1].northing_ft) * nConstants::FT_TO_MM),
			static_cast<std::int32_t>((mUIUC_TowerLocations[1].easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
			static_cast<std::int32_t>((mUIUC_TowerLocations[1].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM)),

		rfm::rappPoint_t(static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - mUIUC_TowerLocations[2].northing_ft) * nConstants::FT_TO_MM),
			static_cast<std::int32_t>((mUIUC_TowerLocations[2].easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
			static_cast<std::int32_t>((mUIUC_TowerLocations[2].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM)),

		rfm::rappPoint_t(static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - mUIUC_TowerLocations[3].northing_ft) * nConstants::FT_TO_MM),
			static_cast<std::int32_t>((mUIUC_TowerLocations[3].easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
			static_cast<std::int32_t>((mUIUC_TowerLocations[3].elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM)) };

	/*
	* 40.0635686°, -88.2081615°
	* 40.0618528°, -88.2081656°
	* 40.0618504°, -88.2059322°
	* 40.0635664°, -88.2059295°
	* 
	*/
	/*
	 * Tower 1 : lat = 40.0635686°, lng = -88.2081615°
	 * Tower 2 : lat = 40.0618528°, lng = -88.2081656°
	 * Tower 3 : lat = 40.0618504°, lng = -88.2059322°
	 * Tower 4 : lat = 40.0635664°, lng = -88.2059295°
	 */

	struct sLatLon_WGS84_t
	{
		double mLatitude_rad = 0.0;
		double mLongitude_rad = 0.0;

		sLatLon_WGS84_t(double lat, double lng) : mLatitude_rad(lat), mLongitude_rad(lng)
		{}
	};

	const std::array<sLatLon_WGS84_t, 4> mRappTowerLocations_WGS84 = {
		sLatLon_WGS84_t(40.0635686 * nConstants::DEG_TO_RAD, -88.2081615 * nConstants::DEG_TO_RAD),
		sLatLon_WGS84_t(40.0618528 * nConstants::DEG_TO_RAD, -88.2081656 * nConstants::DEG_TO_RAD),
		sLatLon_WGS84_t(40.0618504 * nConstants::DEG_TO_RAD, -88.2059322 * nConstants::DEG_TO_RAD),
		sLatLon_WGS84_t(40.0635664 * nConstants::DEG_TO_RAD, -88.2059295 * nConstants::DEG_TO_RAD)
	};

}

uint32_t rfb::minX_mm()
{
	return mRappTowerLocations[0].x_mm;
}

uint32_t rfb::minY_mm()
{
	return mRappTowerLocations[0].y_mm;
}

uint32_t rfb::minZ_mm()
{
	return 0;
}

uint32_t rfb::maxX_mm()
{
	return mRappTowerLocations[2].x_mm;
}

uint32_t rfb::maxY_mm()
{
	return mRappTowerLocations[2].y_mm;
}

uint32_t rfb::maxZ_mm()
{
	return 10000;
}

bool rfb::withinBoundary(const rfm::planePoint_t& point)
{
	rfm::rappPoint_t p = toRappCoordinates(point);
	return withinBoundary(p);
}

bool rfb::withinBoundary(const rfm::rappPoint_t& point)
{
	return withinBoundary(point.x_mm, point.y_mm);
}

bool rfb::withinBoundary(const std::int32_t x_mm, const std::int32_t y_mm)
{
	if ((x_mm < minX_mm()) || (x_mm > maxX_mm()))
		return false;

	if ((y_mm < minY_mm()) || (y_mm > maxY_mm()))
		return false;

	return true;
}

rfm::rappPoint_t rfb::toRappCoordinates(const rfm::planePoint_t& point)
{
	rfm::rappPoint_t result;

	result = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - point.northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((point.easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((point.elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	return result;
}

rfm::rappPoint_t rfb::fromStatePlane(const double northing_ft, const double easting_ft, const double elevation_ft)
{
	rfm::rappPoint_t result;

	result = { static_cast<std::int32_t>((mUIUC_TowerLocations[0].northing_ft - northing_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((easting_ft - mUIUC_TowerLocations[0].easting_ft) * nConstants::FT_TO_MM),
		static_cast<std::int32_t>((elevation_ft - mUIUC_ReferenceElevation_ft) * nConstants::FT_TO_MM) };

	return result;
}

rfm::rappPoint_t rfb::fromGPS(const double lat_rad, const double lng_rad, const double height_m)
{
	static double ref_north_m = (mUIUC_TowerLocations[0].northing_ft * nConstants::FT_TO_M);
	static double ref_east_m = (mUIUC_TowerLocations[0].easting_ft * nConstants::FT_TO_M);

	static auto dbContext = DatabaseContext::create();

	// Instantiate a generic authority factory, that is not tied to a particular
	// authority, to be able to get transformations registered by different
	// authorities. This can only be used for CoordinateOperationContext.
	static auto authFactory = AuthorityFactory::create(dbContext, std::string());

	// Create a coordinate operation context, that can be customized to ammend
	// the way coordinate operations are computed. Here we ask for default
	// settings.
	static auto coord_op_context = CoordinateOperationContext::create(authFactory, nullptr, 0.0);

	// Instantiate a authority factory for EPSG related objects.
	static auto authFactoryEPSG = AuthorityFactory::create(dbContext, "EPSG");

	// Instantiate source CRS from EPSG code: WGS84
	static auto wgs84_CRS = authFactoryEPSG->createCoordinateReferenceSystem("4326");

	// Instantiate target CRS from EPSG code: Illinois East NAD83(2011)
	static auto il_state_plane_east_CRS = authFactoryEPSG->createCoordinateReferenceSystem("6454");

	// List operations available to transform from EPSG:4326
	// (WGS 84 latitude/longitude) to EPSG:6454 (Illinois State Plane East).
	auto list = CoordinateOperationFactory::create()->createOperations(wgs84_CRS, il_state_plane_east_CRS, coord_op_context);

	// Check that we got a non-empty list of operations
	// The list is sorted from the most relevant to the less relevant one.
	// Cf
	// https://proj.org/operations/operations_computation.html#filtering-and-sorting-of-coordinate-operations
	// for more details on the sorting of those operations.
	// For a transformation between a projected CRS and its base CRS, like
	// we do here, there will be only one operation.
	assert(!list.empty());

	// Create an execution context (must only be used by one thread at a time)
	PJ_CONTEXT* ctx = proj_context_create();

	// Create a coordinate transformer from the first operation of the list
	auto transformer = list[0]->coordinateTransformer(ctx);

	double lat_deg = lat_rad * nConstants::RAD_TO_DEG;
	double lng_deg = lng_rad * nConstants::RAD_TO_DEG;

	// Perform the coordinate transformation.
	PJ_COORD c = { {
		lat_deg,    // latitude in degree
		lng_deg,     // longitude in degree
		0.0,     // z ordinate. unused
		HUGE_VAL // time ordinate. unused
	} };

	c = transformer->transform(c);

	double easting_m = c.v[0];
	double northing_m = c.v[1];

	// Destroy execution context
	proj_context_destroy(ctx);

	rfm::rappPoint_t result;

	double x_m = ref_north_m - northing_m;	// positive is to the south
	double y_m = easting_m - ref_east_m;	// positive is to the east

	result.x_mm = static_cast<std::int32_t>(x_m * nConstants::M_TO_MM);
	result.y_mm = static_cast<std::int32_t>(y_m * nConstants::M_TO_MM);
	result.z_mm = static_cast<std::int32_t>((height_m - mUIUC_ReferenceElevation_m) * nConstants::M_TO_MM);

	return result;
}


