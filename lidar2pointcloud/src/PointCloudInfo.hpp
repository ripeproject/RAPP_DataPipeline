
#pragma once


#include <string>
#include <optional>
#include <vector>

// Forward Declarations



struct sPointCloudInfo_t
{
    /** Sensor Mounting Orientation **/
    double mount_pitch_deg = 0.0;
    double mount_roll_deg = 0.0;
    double mount_yaw_deg = 0.0;

    /** Sensor Options **/
    double min_dist_m = 0.0;
    double max_dist_m = 0.0;

    double min_azimuth_deg = 0.0;
    double max_azimuth_deg = 0.0;

    double min_altitude_deg = 0.0;
    double max_altitude_deg = 0.0;

    /** Point Cloud Options **/
    bool translateToGround = false;
    double translateThreshold_pct = 0.0;

    bool rotateToGround = false;
    double rotateThreshold_pct = 0.0;

};

