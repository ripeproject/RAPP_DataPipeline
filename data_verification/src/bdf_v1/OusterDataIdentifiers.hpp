
#pragma once

namespace v1
{
    namespace ouster
    {
        /**********************************************************
           WARNING: DO NOT CHANGE THE ORDER OF THESE ENUMS!!!!

           Only add new one to the end of the list.
        **********************************************************/
        enum class DataID : uint16_t
        {
            CONFIGURATION_INFO,
            SENSOR_INFO,
            TIMESTAMP,
            SYNC_PULSE_IN,
            SYNC_PULSE_OUT,
            MULTIPURPOSE_IO,
            NMEA,
            TIME_INFO,
            BEAM_INTRINSICS,
            IMU_INTRINSICS,
            LIDAR_INTRINSICS,
            LIDAR_DATA_FORMAT,
            ALERT_INFO,
            ALERTS,
            LIDAR_DATA,
            IMU_DATA,
            LIDAR_DATA_FRAME_TIMESTAMP,
        };
    }
}

