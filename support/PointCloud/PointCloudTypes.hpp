
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
        SLAM,
        CONSTANT_SENSOR_ROTATION,
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

        sCloudPoint_t() {}
    };

    //
    // A point cloud data struct with the addition of frame id.
    //
    struct sCloudPoint_FrameID_t : public sCloudPoint_t
    {
        uint16_t frameID = 0;

        sCloudPoint_FrameID_t() {}
    };

    //
    // A point cloud data struct with the addition sensor information.
    //
    struct sCloudPoint_SensorInfo_t : public sCloudPoint_FrameID_t
    {
        uint16_t chnNum = 0;
        uint16_t pixelNum = 0;
        double   theta_rad = 0.0;
        double   phi_rad = 0.0;

        sCloudPoint_SensorInfo_t() {}
    };

    //
    // A bounding box data struct used in various utilities.
    //
    struct sBoundingBox_t
    {
        std::array<sPoint2D_t, 4> points;
    };
}


inline pointcloud::sPoint2D_t::sPoint2D_t(const pointcloud::sPoint3D_t& p) : X_m(p.X_m), Y_m(p.Y_m)
{}

inline const pointcloud::sPoint2D_t& pointcloud::sPoint2D_t::operator=(const pointcloud::sPoint3D_t& p)
{
    X_m = p.X_m;
    Y_m = p.Y_m;
    return *this;
}
