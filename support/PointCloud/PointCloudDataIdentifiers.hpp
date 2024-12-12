
#pragma once

#include <cstdint>


namespace pointcloud
{
	/**********************************************************
	   WARNING: DO NOT CHANGE THE ORDER OF THESE ENUMS!!!!
	   
	   Only add new one to the end of the list.
	**********************************************************/
    enum class DataID : uint16_t
    {
		COORDINATE_SYSTEM,
        IMU_DATA,                   // <-- deprecated, but don't remove
        REDUCED_POINT_CLOUD_DATA_BY_FRAME,
        SENSOR_POINT_CLOUD_DATA_BY_FRAME,
        POINT_CLOUD_DATA,
        SENSOR_ROTATION_ANGLES,     // <-- deprecated, but don't remove
        KINEMATICS_MODEL,
        KINEMATICS_SPEEDS,          // <-- deprecated, but don't remove
        POINT_CLOUD_DIMENSIONS,
        POINT_CLOUD_DATA_FRAME_ID,
        POINT_CLOUD_DATA_SENSOR_INFO,
        REDUCED_POINT_CLOUD_DATA_BY_FRAME_FRAME_ID,
        REDUCED_POINT_CLOUD_DATA_BY_FRAME_SENSOR_INFO,
        SENSOR_POINT_CLOUD_DATA_BY_FRAME_FRAME_ID,
        SENSOR_POINT_CLOUD_DATA_BY_FRAME_SENSOR_INFO,
        SENSOR_ROTATIONAL_SPEEDS,   // <-- deprecated, but don't remove
        DISTANCE_WINDOW_M,
        AZIMUTH_WINDOW_DEG,
        ALTITUDE_WINDOW_DEG,
        BEGIN_SENSOR_KINEMATICS,
        END_SENSOR_KINEMATICS,
        SENSOR_KINEMATIC_POINT,
        BEGIN_POINT_CLOUD_BLOCK,
        END_POINT_CLOUD_BLOCK,
        BEGIN_POINT_CLOUD_LIST,
        END_POINT_CLOUD_LIST,
        REFERENCE_POINT,
        VEGETATION_ONLY,
    };
}


