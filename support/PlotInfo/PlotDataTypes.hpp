
#pragma once

#include <cstdint>


// Forward Declarations
namespace pointcloud
{
	struct sCloudPoint_t;
	struct sCloudPoint_FrameID_t;
	struct sCloudPoint_SensorInfo_t;
}


namespace plot
{
	constexpr std::int32_t INVALID_HEIGHT = -999999;

	struct rappPoint_t
	{
		int32_t  x_mm = 0;
		int32_t  y_mm = 0;
		int32_t  z_mm = 0;
	};

	struct sPoint3D_t
	{
		int32_t  x_mm = 0;
		int32_t  y_mm = 0;
		int32_t  z_mm = 0;
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

	struct sCentroid_t
	{
		double x_mm = 0.0;
		double y_mm = 0.0;
		double z_mm = 0.0;
	};
}
