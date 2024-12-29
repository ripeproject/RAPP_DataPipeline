
#include "datatypes.hpp"

#include "Constants.hpp"

#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>


using namespace rfm;

#if 0
sPoint3D_t& sPoint3D_t::operator=(const pointcloud::sCloudPoint_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	h_mm = INVALID_HEIGHT;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = 0;
	chnNum = 0;
	pixelNum = 0;

	return *this;
}

sPoint3D_t& sPoint3D_t::operator=(const pointcloud::sCloudPoint_FrameID_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	h_mm = INVALID_HEIGHT;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = rhs.frameID;
	chnNum = 0;
	pixelNum = 0;

	return *this;
}

sPoint3D_t& sPoint3D_t::operator=(const pointcloud::sCloudPoint_SensorInfo_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	h_mm = INVALID_HEIGHT;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = rhs.frameID;
	chnNum = rhs.chnNum;
	pixelNum = rhs.pixelNum;

	return *this;
}

#endif

