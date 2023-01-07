
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
        POINT_CLOUD_DATA,
    };
}


