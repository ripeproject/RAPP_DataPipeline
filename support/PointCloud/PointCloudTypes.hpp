
#pragma once

#include <cstdint>
#include <vector>

namespace pointcloud
{
	enum class eCOORDINATE_SYSTEM : uint8_t
	{
		UNKNOWN,
		SENSOR,
		SENSOR_ENU,
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


    struct sCloudPoint_t
    {
        double X_m;
        double Y_m;
        double Z_m;
        uint32_t range_mm;
        uint16_t signal;
        uint16_t reflectivity;
        uint16_t nir;

        sCloudPoint_t() : X_m(0), Y_m(0), Z_m(0),
            range_mm(0), signal(0), reflectivity(0), nir(0)
        {}
    };


    struct reduced_point_cloud_by_frame_t
    {
        uint16_t frameID;
        uint64_t timestamp_ns;
        std::vector<pointcloud::sCloudPoint_t> pointCloud;
    };


    struct sensor_point_cloud_by_frame_t
    {
        uint16_t frameID;
        uint64_t timestamp_ns;
        std::vector<pointcloud::sCloudPoint_t> pointCloud;
    };


    struct point_cloud_t
    {
        std::vector<pointcloud::sCloudPoint_t> pointCloud;
    };
}

