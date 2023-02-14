
#pragma once

namespace pointcloud
{
	/**********************************************************
	   WARNING: DO NOT CHANGE THE ORDER OF THESE ENUMS!!!!
	   
	   Only add new one to the end of the list.
	**********************************************************/
    enum class DataID : uint16_t
    {
		COORDINATE_SYSTEM,
        IMU_DATA,
        REDUCED_POINT_CLOUD_DATA_BY_FRAME,
        SENSOR_POINT_CLOUD_DATA_BY_FRAME,
        POINT_CLOUD_DATA,
    };
}


