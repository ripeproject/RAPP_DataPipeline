
#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace pointcloud
{
    struct sPoint3D_t;

    struct sPoint2D_t
    {
        double X_m = 0.0;
        double Y_m = 0.0;

        sPoint2D_t() {}
        explicit sPoint2D_t(const sPoint3D_t& p);
        const sPoint2D_t& operator=(const sPoint3D_t& p);
    };

    struct sPoint3D_t
    {
        double X_m = 0.0;
        double Y_m = 0.0;
        double Z_m = 0.0;
    };

    enum class eCOORDINATE_SYSTEM : uint8_t
    {
        UNKNOWN,
        SENSOR,
        SENSOR_ENU,
        SENSOR_SEU,
    };

    enum class eKINEMATIC_MODEL : uint8_t
    {
        UNKNOWN,
        CONSTANT,
        DOLLY,
        GPS,
        GPS_SPEEDS,
        CONSTANT_SENSOR_ROTATION,
        DOLLY_SENSOR_ROTATION,
        GPS_SENSOR_ROTATION,
        GPS_SPEEDS_SENSOR_ROTATION,
    };

    struct imu_data_t
    {
        uint64_t	accelerometer_read_time_ns;
        uint64_t	gyroscope_read_time_ns;

        float acceleration_X_g;
        float acceleration_Y_g;
        float acceleration_Z_g;

        float angular_velocity_Xaxis_deg_per_sec;
        float angular_velocity_Yaxis_deg_per_sec;
        float angular_velocity_Zaxis_deg_per_sec;

        imu_data_t() : accelerometer_read_time_ns(0), gyroscope_read_time_ns(0),
            acceleration_X_g(0), acceleration_Y_g(0), acceleration_Z_g(0),
            angular_velocity_Xaxis_deg_per_sec(0), angular_velocity_Yaxis_deg_per_sec(0),
            angular_velocity_Zaxis_deg_per_sec(0)
        {}
        ~imu_data_t() = default;
        imu_data_t(const imu_data_t&) = default;
        imu_data_t(imu_data_t&&) = default;
        imu_data_t& operator=(const imu_data_t&) = default;
    };

    //
    // The basic struct of point cloud data.
    //
    struct sCloudPoint_t : public sPoint3D_t
    {
        uint32_t range_mm = 0;
        uint16_t signal = 0;
        uint16_t reflectivity = 0;
        uint16_t nir = 0;

        sCloudPoint_t() = default;
        sCloudPoint_t(const sCloudPoint_t&) = default;
    };

    //
    // A point cloud data struct with the addition of frame id.
    //
    struct sCloudPoint_FrameID_t : public sCloudPoint_t
    {
        uint16_t frameID = 0;

        sCloudPoint_FrameID_t() = default;
        sCloudPoint_FrameID_t(const sCloudPoint_FrameID_t&) = default;

        explicit sCloudPoint_FrameID_t(const sCloudPoint_t& p);
        const sCloudPoint_FrameID_t& operator=(const sCloudPoint_t& p);
    };

    //
    // A point cloud data struct with the addition sensor information.
    //
    struct sCloudPoint_SensorInfo_t : public sCloudPoint_FrameID_t
    {
        uint16_t chnNum = 0;
        uint16_t pixelNum = 0;

        sCloudPoint_SensorInfo_t() = default;
        sCloudPoint_SensorInfo_t(const sCloudPoint_SensorInfo_t&) = default;

        explicit sCloudPoint_SensorInfo_t(const sCloudPoint_t& p);
        explicit sCloudPoint_SensorInfo_t(const sCloudPoint_FrameID_t& p);

        const sCloudPoint_SensorInfo_t& operator=(const sCloudPoint_t& p);
        const sCloudPoint_SensorInfo_t& operator=(const sCloudPoint_FrameID_t& p);
    };

    //
    // A bounding box data struct used in various utilities.
    //
    struct sBoundingBox_t
    {
        std::array<sPoint2D_t, 4> points;
    };


    /*
     * The meaning of X, Y, and Z are depended on the value of the
     * eCOORDINATE_SYSTEM enumeration.
     * 
     * If eCOORDINATE_SYSTEM == SENSOR_SEU (south/east/up), then
     * x axis is positive heading south
     * y axis is positive heading east
     * z axis is positive going up
     */
    struct sSensorKinematicInfo_t
    {
        double timestamp_us = 0.0;

        double X_m = 0.0;
        double Y_m = 0.0;
        double Z_m = 0.0;

        double Vx_mps = 0.0;
        double Vy_mps = 0.0;
        double Vz_mps = 0.0;

        double pitch_deg = 0.0;
        double roll_deg = 0.0;
        double yaw_deg = 0.0;

        double pitchRate_dps = 0.0;
        double rollRate_dps = 0.0;
        double yawRate_dps = 0.0;
    };
}



///////////////////////////////////////////////////////////////////////////////
// Implementation Details
///////////////////////////////////////////////////////////////////////////////

inline pointcloud::sPoint2D_t::sPoint2D_t(const pointcloud::sPoint3D_t& p) : X_m(p.X_m), Y_m(p.Y_m)
{}

inline const pointcloud::sPoint2D_t& pointcloud::sPoint2D_t::operator=(const pointcloud::sPoint3D_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    return *this;
}


inline pointcloud::sCloudPoint_FrameID_t::sCloudPoint_FrameID_t(const pointcloud::sCloudPoint_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    Z_m = p.Z_m;
    range_mm = p.range_mm;
    signal = p.signal;
    reflectivity = p.reflectivity;
    nir = p.nir;
    frameID = 0;
}

inline const pointcloud::sCloudPoint_FrameID_t& pointcloud::sCloudPoint_FrameID_t::operator=(const pointcloud::sCloudPoint_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    Z_m = p.Z_m;
    range_mm = p.range_mm;
    signal = p.signal;
    reflectivity = p.reflectivity;
    nir = p.nir;
    frameID = 0;
    return *this;
}

inline pointcloud::sCloudPoint_SensorInfo_t::sCloudPoint_SensorInfo_t(const pointcloud::sCloudPoint_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    Z_m = p.Z_m;
    range_mm = p.range_mm;
    signal = p.signal;
    reflectivity = p.reflectivity;
    nir = p.nir;
    frameID = 0;
    chnNum = 0;
    pixelNum = 0;
}

inline pointcloud::sCloudPoint_SensorInfo_t::sCloudPoint_SensorInfo_t(const pointcloud::sCloudPoint_FrameID_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    Z_m = p.Z_m;
    range_mm = p.range_mm;
    signal = p.signal;
    reflectivity = p.reflectivity;
    nir = p.nir;
    frameID = p.frameID;
    chnNum = 0;
    pixelNum = 0;
}

inline const pointcloud::sCloudPoint_SensorInfo_t& pointcloud::sCloudPoint_SensorInfo_t::operator=(const pointcloud::sCloudPoint_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    Z_m = p.Z_m;
    range_mm = p.range_mm;
    signal = p.signal;
    reflectivity = p.reflectivity;
    nir = p.nir;
    frameID = 0;
    chnNum = 0;
    pixelNum = 0;
    return *this;
}

inline const pointcloud::sCloudPoint_SensorInfo_t& pointcloud::sCloudPoint_SensorInfo_t::operator=(const pointcloud::sCloudPoint_FrameID_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    Z_m = p.Z_m;
    range_mm = p.range_mm;
    signal = p.signal;
    reflectivity = p.reflectivity;
    nir = p.nir;
    frameID = p.frameID;
    chnNum = 0;
    pixelNum = 0;
    return *this;
}

