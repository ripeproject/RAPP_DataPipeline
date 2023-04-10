
#pragma once

#include <cstdint>
#include <utility>

/**
	Storage of the data from the Ceres system is based on a block file format.  

	In this file format, each block contains a block type identifier.  The block type is used
	as a signal on how the data in the block should be decoded.

    Block Type Layout
    Each block type consists of four fields:

    +------------------------------------------------------------+
    |                          Block Type                        |
    +------------+-----------------+-----------------+-----------+
    |  Class ID  |  Major Version  |  Minor Version  |  Data ID  |
    +------------+-----------------+-----------------+-----------+

    Class ID	   : A two-byte unsigned integer representing the class of the data block.
                     Class ID can be based on sensor manufacturer or other criteria.
                     A class ID of zero or 65535 are reserved.

    Major Version  : A single byte unsigned integer defining the data's class major version.

    Major Version  : A single byte unsigned integer defining the data's class minor version.

    Data ID        : A two-byte unsigned integer used to identify the data block.

    For Example:
    The OUSTER LiDAR has a firmware that can be upgraded.  When we upgrade the firmware,
    it is possible that certain data sent by the LiDAR changes format.  We can use
    the major/minor versions to track these changes.

	The class id is two-byte unsigned integer.  The following regions 

    0               : Reserved   
    1     -  1,024  : Range for Main program / Experiment Header data
    1,025 -  2,048  : Range for Experiment Controller data
    2,049 - 65,534  : Range for Sensor data
    65,535          : Reserved
*/

namespace v1
{
    static const uint16_t SYSTEM_BASE_ID = 1;
    static const uint16_t SYSTEM_MAX_ID = 0x0400;

    static const uint16_t EXP_CONTROL_BASE_ID = 0x0401;
    static const uint16_t EXP_CONTROL_MAX_ID = 0x0800;

    static const uint16_t SENSOR_BASE_ID = 0x0801;
    static const uint16_t SENSOR_MAX_ID = 0xFFFE;


    enum class ClassIDs : uint16_t
    {
        /***********************************************************
         * 1 - 1, 024  : Range for Main program / Experiment Header data
         ***********************************************************/
        EXPERIMENT_INFO = SYSTEM_BASE_ID,

        /***********************************************************
         * 1,025 -  2,048  : Range for Experiment Controller data
         ***********************************************************/

         /*
          * Experiment Controller based on position/velocity
          */
        PVT = EXP_CONTROL_BASE_ID,

        /*
         * Experiment Controller from Spidercam
         */
        SPIDERCAM = EXP_CONTROL_BASE_ID + 0x0010,

        /***********************************************************
         * 2,049 - 65,534  : Range for Sensor data
         ***********************************************************/

         /*
          * Sensors from OUSTER
          */
        OUSTER = SENSOR_BASE_ID,
        OUSTER_LIDAR,

        /*
         * Sensors from Septentrio
         */
        SEPTENTRIO = OUSTER + 0x0080,
        SSNX,

        /*
         * Sensors from Hyspex
         */
        HYSPEX = SSNX + 0x0080,
        HYSPEX_SWIR_384,
        HYSPEX_VNIR_3000N,

        /*
         * Sensors from Axis Communications
         */
        AXIS_COMMUNICATIONS = HYSPEX + 0x0080,
        AXIS_COMMUNICATIONS_CAMERA,

        // We will put the weather data at the end of our range
        WEATHER = SENSOR_MAX_ID - 0x0080,
    };
}

