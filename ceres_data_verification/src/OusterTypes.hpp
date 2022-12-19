/**
 * @file
 * @brief OUSTER Standard Definitions
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#ifdef ERROR
    #undef ERROR
#endif

#ifdef BAUD_9600
    #undef BAUD_9600
#endif

#ifdef BAUD_115200
    #undef BAUD_115200
#endif

namespace ouster
{

    enum class eOPERATING_MODE : uint8_t
    {
        NORMAL,
        STANDBY
    };

    enum class eSENSOR_STATUS : uint8_t
    {
        UNKNOWN,
        INITIALIZING,
        WARMUP,
        UPDATING,
        RUNNING,
        STANDBY,
        ERROR,
        UNCONFIGURED
    };

    enum class eLIDAR_MODE : uint8_t
    {
        MODE_512x10,
        MODE_512x20,
        MODE_1024x10,
        MODE_1024x20,
        MODE_2048x10
    };

    enum class eTIMESTAMP_MODE : uint8_t
    {
        TIME_FROM_INTERNAL_OSC,
        TIME_FROM_SYNC_PULSE_IN,
        TIME_FROM_PTP_1588
    };

    enum class eNMEA_BAUD_RATE : uint8_t
    {
        BAUD_9600,
        BAUD_115200
    };

    enum class ePOLARITY : uint8_t
    {
        ACTIVE_HIGH,
        ACTIVE_LOW
    };

    enum class eIO_PIN_MODE : uint8_t
    {
        OFF,
        INPUT_NMEA_UART,
        OUTPUT_FROM_INTERNAL_OSC,
        OUTPUT_FROM_SYNC_PULSE_IN,
        OUTPUT_FROM_PTP_1588,
        OUTPUT_FROM_ENCODER_ANGLE
    };

    enum azimuth_status : int32_t
    {
        BAD = 0,
        GOOD = -1
    };

    struct version_t 
    {
        int16_t major = 0;
        int16_t minor = 0;
        int16_t patch = 0;
        std::string extra;
    };

    const version_t invalid_version = { 0, 0, 0 };

    struct azimuth_range_t
    {
        double min_deg = 0;
        double max_deg = 0;
    };

    struct azimuth_window_t
    {
        uint32_t min_mdeg = 0;
        uint32_t max_mdeg = 0;
    };

    struct column_window_t
    {
        uint16_t min = 0;
        uint16_t max = 0;
    };

    struct config_param_2_t 
    {
        std::string udp_ip;
        std::string udp_dest;
        uint16_t lidar_port = 0;
        uint16_t imu_port = 0;
        eTIMESTAMP_MODE timestamp_mode = eTIMESTAMP_MODE::TIME_FROM_INTERNAL_OSC;
        ePOLARITY sync_pulse_in_polarity = ePOLARITY::ACTIVE_HIGH;
        ePOLARITY nmea_in_polarity = ePOLARITY::ACTIVE_HIGH;
        bool nmea_ignore_valid_char = false;
        eNMEA_BAUD_RATE nmea_baud_rate = eNMEA_BAUD_RATE::BAUD_9600;
        int nmea_leap_seconds = 0;
        eIO_PIN_MODE multipurpose_io_mode = eIO_PIN_MODE::OFF;
        ePOLARITY sync_pulse_out_polarity = ePOLARITY::ACTIVE_HIGH;
        double sync_pulse_out_frequency_hz = 0;
        double sync_pulse_out_angle = 0;
        double sync_pulse_out_pulse_width = 0;
        bool auto_start_flag = false;
        eOPERATING_MODE operating_mode = eOPERATING_MODE::NORMAL;
        eLIDAR_MODE lidar_mode = eLIDAR_MODE::MODE_1024x10;
        azimuth_range_t azimuth_window;
        bool phase_lock_enable = false;
        double phase_lock_offset_deg = 0;
    };

    struct sensor_info_2_t 
    {
        std::string product_line;
        std::string product_part_number;
        std::string product_serial_number;
        std::string base_part_number;
        std::string base_serial_number;
        std::string image_rev;
        version_t build_revision = invalid_version;
        version_t proto_revision = invalid_version;
        std::string build_date;
        eSENSOR_STATUS status = eSENSOR_STATUS::UNKNOWN;
    };

    struct timestamp_2_t 
    {
        double time = 0;
        eTIMESTAMP_MODE mode = eTIMESTAMP_MODE::TIME_FROM_INTERNAL_OSC;
        uint32_t sync_pulse_in = 0;
        uint32_t internal_osc = 0;
        uint32_t ptp_1588 = 0;
    };

    struct sync_pulse_in_2_t
    {
        bool locked = false;
        uint32_t last_period_nsec = 0;
        uint32_t count_unfiltered = 0;
        uint32_t count = 0;
        ePOLARITY polarity = ePOLARITY::ACTIVE_HIGH;
    };

    struct sync_pulse_out_2_t
    {
        uint16_t pulse_width_ms = 0;
        double angle_deg = 0;
        uint32_t frequency_hz = 0;
        ePOLARITY polarity = ePOLARITY::ACTIVE_HIGH;
    };

    struct multipurpose_io_2_t
    {
        eIO_PIN_MODE mode = eIO_PIN_MODE::OFF;
        double pulse_width_ms = 0;
        double angle_deg = 0;
        double frequency_hz = 0;
        ePOLARITY polarity = ePOLARITY::ACTIVE_HIGH;
    };

    struct nmea_2_t
    {
        bool locked = false;
        eNMEA_BAUD_RATE baud_rate = eNMEA_BAUD_RATE::BAUD_9600;
        int bit_count = 0;
        int bit_count_unfiltered = 0;
        int start_char_count = 0;
        int char_count = 0;
        std::string last_read_message;
        int date_decoded_count = 0;
        int not_valid_count = 0;
        int utc_decoded_count = 0;
        int leap_seconds = 0;
        bool ignore_valid_char = false;
        ePOLARITY polarity = ePOLARITY::ACTIVE_HIGH;
    };

    struct time_info_2_t 
    {
        timestamp_2_t       timestamp_info;
        sync_pulse_in_2_t   sync_pulse_info;
        multipurpose_io_2_t multipurpose_io_info;
        nmea_2_t            nmea_info;
    };

    struct beam_intrinsics_2_t 
    {
        double lidar_to_beam_origins_mm = 0;
        std::vector<double> azimuth_angles_deg;
        std::vector<double> altitude_angles_deg;
    };

    struct imu_intrinsics_2_t 
    {
        std::vector<double> imu_to_sensor_transform;
    };

    struct lidar_intrinsics_2_t 
    {
        std::vector<double> lidar_to_sensor_transform;
    };

    struct lidar_data_format_2_t
    {
        uint16_t columns_per_frame = 0;
        uint16_t columns_per_packet = 0;
        std::vector<int> pixel_shift_by_row;
        uint16_t pixels_per_column = 0;
        uint16_t column_window_min = 0;
        uint16_t column_window_max = 0;
    };

    struct lidar_data_format_2_3_t : public lidar_data_format_2_t
    {
        std::string udp_profile_lidar;
        std::string udp_profile_imu;
    };

    struct alert_info_t 
    {
        bool        active = false;
        uint32_t    id = 0;
        uint64_t    timestamp_ms = 0;
        int         cursor = 0;
        std::string category;
        std::string level;
        std::string message;
        std::string msg_verbose;
    };

    struct alerts_2_t 
    {
        int next_cursor = 0;
        std::vector<alert_info_t> active;
        std::vector<alert_info_t> log;
    };

    struct imu_data_t
    {
        uint64_t	diagnostic_time_ns = 0;
        uint64_t	accelerometer_read_time_ns = 0;
        uint64_t	gyroscope_read_time_ns = 0;

        float acceleration_Xaxis_g = 0;
        float acceleration_Yaxis_g = 0;
        float acceleration_Zaxis_g = 0;

        float angular_velocity_Xaxis_deg_per_sec = 0;
        float angular_velocity_Yaxis_deg_per_sec = 0;
        float angular_velocity_Zaxis_deg_per_sec = 0;
    };

    struct lidar_data_block_t
    {
        uint32_t range_mm = 0;
        uint16_t intensity = 0;
        uint16_t reflectivity = 0;
        uint16_t ambient_noise = 0;
    };

    struct lidar_channel_t
    {
        std::vector<lidar_data_block_t> pixels;
    };

    struct lidar_data_frame_t
    {
        uint16_t frame_id = 0;
        uint64_t timestamp_ns = 0;

        uint16_t pixels_per_column = 0;
        uint16_t columns_per_frame = 0;

        std::vector<lidar_channel_t> channels;
    };


} // End namespace ouster

