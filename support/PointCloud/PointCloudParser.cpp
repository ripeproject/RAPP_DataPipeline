
#include "OusterParser.hpp"
#include "OusterBlockId.hpp"
#include "OusterDataIdentifiers.hpp"
#include "../BlockDataFile.hpp"

#include <ouster/ouster_utils.h>
#include <ouster/OusterLidarData.h>

#include <stdexcept>

using namespace ouster;

namespace
{
    ouster::version_t to_version(cDataBuffer& buffer)
    {
        std::string s;
        buffer >> s;
        return ::to_version(s);
    }

    ouster::eOPERATING_MODE to_operating_mode(cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eOPERATING_MODE>(mode); 
    };

    ouster::eSENSOR_STATUS to_sensor_status(cDataBuffer& buffer)
    {
        uint8_t status = 0;
        buffer >> status;
        return static_cast<ouster::eSENSOR_STATUS>(status);
    };

    ouster::eLIDAR_MODE to_lidar_mode(cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eLIDAR_MODE>(mode);
    };

    ouster::eTIMESTAMP_MODE to_timestamp_mode(cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eTIMESTAMP_MODE>(mode);
    };

    ouster::eNMEA_BAUD_RATE to_nmea_baud_rate(cDataBuffer& buffer)
    {
        uint8_t rate = 0;
        buffer >> rate;
        return static_cast<ouster::eNMEA_BAUD_RATE>(rate);
    };

    ouster::ePOLARITY to_polarity(cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::ePOLARITY>(mode);
    };

    ouster::eIO_PIN_MODE to_pin_mode(cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eIO_PIN_MODE>(mode);
    };

    ouster::azimuth_status to_azimuth_status(cDataBuffer& buffer)
    {
        int32_t status = 0;
        buffer >> status;
        return static_cast<ouster::azimuth_status>(status);
    };
}

cOusterParser::cOusterParser()
:
    cBlockParser(), mBlockID(new cOusterLidarID())
{}

cOusterParser::~cOusterParser() {}

cBlockID& cOusterParser::blockID()
{
    return *mBlockID;
}


void cOusterParser::processData(BLOCK_MAJOR_VERSION_t major_version,
                                BLOCK_MINOR_VERSION_t minor_version,
                                BLOCK_DATA_ID_t data_id,
                                cDataBuffer& buffer)
{
    mBlockID->setVersion(major_version, minor_version);
    mBlockID->dataID(static_cast<ouster::DataID>(data_id));

    switch (static_cast<ouster::DataID>(data_id))
    {
    case DataID::CONFIGURATION_INFO:
        processConfigParam_2(buffer);
        break;
    case DataID::SENSOR_INFO:
        processSensorInfo_2(buffer);
        break;
    case DataID::TIMESTAMP:
        processTimestamp_2(buffer);
        break;
    case DataID::SYNC_PULSE_IN:
        processSyncPulseIn_2(buffer);
        break;
    case DataID::SYNC_PULSE_OUT:
        processSyncPulseOut_2(buffer);
        break;
    case DataID::MULTIPURPOSE_IO:
        processMultipurposeIO_2(buffer);
        break;
    case DataID::NMEA:
        processNmea_2(buffer);
        break;
    case DataID::TIME_INFO:
        processTimeInfo_2(buffer);
        break;
    case DataID::BEAM_INTRINSICS:
        processBeamIntrinsics_2(buffer);
        break;
    case DataID::IMU_INTRINSICS:
        processImuIntrinsics_2(buffer);
        break;
    case DataID::LIDAR_INTRINSICS:
        processLidarIntrinsics_2(buffer);
        break;
    case DataID::LIDAR_DATA_FORMAT:
        processLidarDataFormat_2(buffer);
        break;
    case DataID::IMU_DATA:
        processImuData(buffer);
        break;
    case DataID::LIDAR_DATA:
        processLidarData(buffer);
        break;
    case DataID::LIDAR_DATA_FRAME_TIMESTAMP:
        processLidarDataFrameTimestamp(buffer);
        break;
    case DataID::ALERT_INFO:
    case DataID::ALERTS:
        break;
    }
}

void cOusterParser::processConfigParam_2(cDataBuffer& buffer)
{
    ouster::config_param_2_t config_param;

    buffer >> config_param.udp_ip;
    buffer >> config_param.udp_dest;
    buffer >> config_param.lidar_port;
    buffer >> config_param.imu_port;

    config_param.timestamp_mode = to_timestamp_mode(buffer);
    config_param.sync_pulse_in_polarity = to_polarity(buffer);
    config_param.nmea_in_polarity = to_polarity(buffer);

    buffer >> config_param.nmea_ignore_valid_char;

    config_param.nmea_baud_rate = to_nmea_baud_rate(buffer);

    buffer >> config_param.nmea_leap_seconds;

    config_param.multipurpose_io_mode = to_pin_mode(buffer);
    config_param.sync_pulse_out_polarity = to_polarity(buffer);

    buffer >> config_param.sync_pulse_out_frequency_hz;
    buffer >> config_param.sync_pulse_out_angle;
    buffer >> config_param.sync_pulse_out_pulse_width;
    buffer >> config_param.auto_start_flag;

    config_param.operating_mode = to_operating_mode(buffer);
    config_param.lidar_mode = to_lidar_mode(buffer);

    buffer >> config_param.azimuth_window.min_deg;
    buffer >> config_param.azimuth_window.max_deg;
    buffer >> config_param.phase_lock_enable;
    buffer >> config_param.phase_lock_offset_deg;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processConfigParam_2.");

    onConfigParam(config_param);
}

void cOusterParser::processSensorInfo_2(cDataBuffer& buffer)
{
    ouster::sensor_info_2_t sensor_info;

    buffer >> sensor_info.product_line;
    buffer >> sensor_info.product_part_number;
    buffer >> sensor_info.product_serial_number;
    buffer >> sensor_info.base_part_number;
    buffer >> sensor_info.base_serial_number;
    buffer >> sensor_info.image_rev;

    sensor_info.build_revision = to_version(buffer);
    sensor_info.proto_revision = to_version(buffer);

    buffer >> sensor_info.build_date;

    sensor_info.status = to_sensor_status(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSensorInfo_2.");

    onSensorInfo(sensor_info);
}

void cOusterParser::processTimestamp_2(cDataBuffer& buffer)
{
    ouster::timestamp_2_t timestamp;

    buffer >> timestamp.time;

    timestamp.mode = to_timestamp_mode(buffer);

    buffer >> timestamp.sync_pulse_in;
    buffer >> timestamp.internal_osc;
    buffer >> timestamp.ptp_1588;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processTimestamp_2.");

    onTimestamp(timestamp);
}

void cOusterParser::processSyncPulseIn_2(cDataBuffer& buffer)
{
    ouster::sync_pulse_in_2_t pulse_info;

    buffer >> pulse_info.locked;
    buffer >> pulse_info.last_period_nsec;
    buffer >> pulse_info.count_unfiltered;
    buffer >> pulse_info.count;

    pulse_info.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSyncPulseIn_2.");

    onSyncPulseIn(pulse_info);
}

void cOusterParser::processSyncPulseOut_2(cDataBuffer& buffer)
{
    ouster::sync_pulse_out_2_t pulse_info;

    buffer >> pulse_info.pulse_width_ms;
    buffer >> pulse_info.angle_deg;
    buffer >> pulse_info.frequency_hz;

    pulse_info.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSyncPulseOut_2.");

    onSyncPulseOut(pulse_info);
}

void cOusterParser::processMultipurposeIO_2(cDataBuffer& buffer)
{
    ouster::multipurpose_io_2_t io;

    io.mode = to_pin_mode(buffer);

    buffer >> io.pulse_width_ms;
    buffer >> io.angle_deg;
    buffer >> io.frequency_hz;

    io.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processMultipurposeIO_2.");

    onMultipurposeIo(io);
}

void cOusterParser::processNmea_2(cDataBuffer& buffer)
{
    ouster::nmea_2_t nmea;

    buffer >> nmea.locked;
    nmea.baud_rate = to_nmea_baud_rate(buffer);
    buffer >> nmea.bit_count;
    buffer >> nmea.bit_count_unfiltered;
    buffer >> nmea.start_char_count;
    buffer >> nmea.char_count;
    buffer >> nmea.last_read_message;
    buffer >> nmea.date_decoded_count;
    buffer >> nmea.not_valid_count;
    buffer >> nmea.utc_decoded_count;
    buffer >> nmea.leap_seconds;
    buffer >> nmea.ignore_valid_char;
    nmea.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processNmea_2.");

    onNmea(nmea);
}

void cOusterParser::processTimeInfo_2(cDataBuffer& buffer)
{
    ouster::time_info_2_t time_info;

    /* Timestamp Info */
    buffer >> time_info.timestamp_info.time;
    time_info.timestamp_info.mode = to_timestamp_mode(buffer);
    buffer >> time_info.timestamp_info.sync_pulse_in;
    buffer >> time_info.timestamp_info.internal_osc;
    buffer >> time_info.timestamp_info.ptp_1588;

    /* Sync Pulse Info */
    buffer >> time_info.sync_pulse_info.locked;
    buffer >> time_info.sync_pulse_info.last_period_nsec;
    buffer >> time_info.sync_pulse_info.count_unfiltered;
    buffer >> time_info.sync_pulse_info.count;
    time_info.sync_pulse_info.polarity = to_polarity(buffer);

    /* Multipurpose IO Info */
    time_info.multipurpose_io_info.mode = to_pin_mode(buffer);
    buffer >> time_info.multipurpose_io_info.pulse_width_ms;
    buffer >> time_info.multipurpose_io_info.angle_deg;
    buffer >> time_info.multipurpose_io_info.frequency_hz;
    time_info.multipurpose_io_info.polarity = to_polarity(buffer);

    /* NMEA Info */
    buffer >> time_info.nmea_info.locked;
    time_info.nmea_info.baud_rate = to_nmea_baud_rate(buffer);
    buffer >> time_info.nmea_info.bit_count;
    buffer >> time_info.nmea_info.bit_count_unfiltered;
    buffer >> time_info.nmea_info.start_char_count;
    buffer >> time_info.nmea_info.char_count;
    buffer >> time_info.nmea_info.last_read_message;
    buffer >> time_info.nmea_info.date_decoded_count;
    buffer >> time_info.nmea_info.not_valid_count;
    buffer >> time_info.nmea_info.utc_decoded_count;
    buffer >> time_info.nmea_info.leap_seconds;
    buffer >> time_info.nmea_info.ignore_valid_char;
    time_info.nmea_info.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processTimeInfo_2.");

    onTimeInfo(time_info);
}

void cOusterParser::processBeamIntrinsics_2(cDataBuffer& buffer)
{
    ouster::beam_intrinsics_2_t intrinsics;

    buffer >> intrinsics.lidar_to_beam_origins_mm;

    uint32_t n = 0;
    buffer >> n;
    intrinsics.azimuth_angles_deg.resize(n);

    for (std::size_t i = 0; i < n; ++i)
        buffer >> intrinsics.azimuth_angles_deg[i];

    n = 0;
    buffer >> n;
    intrinsics.altitude_angles_deg.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> intrinsics.altitude_angles_deg[i];

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processBeamIntrinsics_2.");

    onBeamIntrinsics(intrinsics);
}

void cOusterParser::processImuIntrinsics_2(cDataBuffer& buffer)
{
    ouster::imu_intrinsics_2_t intrinsics;

    uint32_t n = 0;
    buffer >> n;
    intrinsics.imu_to_sensor_transform.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> intrinsics.imu_to_sensor_transform[i];

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processImuIntrinsics_2.");

    onImuIntrinsics(intrinsics);
}

void cOusterParser::processLidarIntrinsics_2(cDataBuffer& buffer)
{
    ouster::lidar_intrinsics_2_t intrinsics;

    uint32_t n = 0;
    buffer >> n;
    intrinsics.lidar_to_sensor_transform.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> intrinsics.lidar_to_sensor_transform[i];

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processLidarIntrinsics_2.");

    onLidarIntrinsics(intrinsics);
}

void cOusterParser::processLidarDataFormat_2(cDataBuffer& buffer)
{
    ouster::lidar_data_format_2_t format;

    buffer >> format.columns_per_frame;
    buffer >> format.columns_per_packet;

    uint32_t n = 0;
    buffer >> n;
    format.pixel_shift_by_row.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> format.pixel_shift_by_row[i];

    buffer >> format.pixels_per_column;
    buffer >> format.column_window_min;
    buffer >> format.column_window_max;

    if (mBlockID->minorVersion() == 3)
    {
        buffer >> format.udp_profile_lidar;
        buffer >> format.udp_profile_imu;
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processLidarDataFormat_2.");

    onLidarDataFormat(format);
}

void cOusterParser::processImuData(cDataBuffer& buffer)
{
    ouster::imu_data_t data;

    buffer >> data.diagnostic_time_ns;
    buffer >> data.accelerometer_read_time_ns;
    buffer >> data.gyroscope_read_time_ns;

    buffer >> data.acceleration_Xaxis_g;
    buffer >> data.acceleration_Yaxis_g;
    buffer >> data.acceleration_Zaxis_g;

    buffer >> data.angular_velocity_Xaxis_deg_per_sec;
    buffer >> data.angular_velocity_Yaxis_deg_per_sec;
    buffer >> data.angular_velocity_Zaxis_deg_per_sec;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processImuData.");

    onImuData(data);
}

void cOusterParser::processLidarData(cDataBuffer& buffer)
{
    uint16_t pixels_per_column = 0;
    uint16_t columns_per_frame = 0;

    buffer >> pixels_per_column;
    buffer >> columns_per_frame;

    mLidarData.resize(pixels_per_column, columns_per_frame);

    lidar_data_block_t pixel;

    for (uint16_t col = 0; col < columns_per_frame; ++col)
    {
        for (uint16_t chn = 0; chn < pixels_per_column; ++chn)
        {
            buffer >> pixel.range_mm;
            buffer >> pixel.signal;
            buffer >> pixel.reflectivity;
            buffer >> pixel.nir;
            mLidarData.channel(col, chn, pixel);
        }
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processLidarData.");

    onLidarData(mLidarData);
}

void cOusterParser::processLidarDataFrameTimestamp(cDataBuffer& buffer)
{
    uint16_t frame_id = 0;
    uint64_t timestamp_ns = 0;

    buffer >> frame_id;
    buffer >> timestamp_ns;

    mLidarData.frame_id(frame_id);
    mLidarData.timestamp_ns(timestamp_ns);

    processLidarData(buffer);
}


