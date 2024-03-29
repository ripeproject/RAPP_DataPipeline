
#include "OusterParser.hpp"
#include "OusterDataIdentifiers.hpp"
#include "BlockDataFile.hpp"

#include <ouster/ouster_utils.h>
#include <ouster/OusterLidarData.h>

#include <stdexcept>

using namespace v1::ouster;

namespace
{
    std::string to_string(v1::cDataBuffer& buffer)
    {
        std::string str;
        auto p = buffer.data();
        for (int i = 0; i < buffer.read_size(); ++i)
        {
            str += static_cast<char>(*p);
            ++p;
        }

        return str;
    }

    ::ouster::version_t to_version(v1::cDataBuffer& buffer, uint16_t len)
    {
        std::string s;
        buffer.read(s, len);
        return ::to_version(s);
    }

    ::ouster::version_t to_version(v1::cDataBuffer& buffer)
    {
        std::string s;
        buffer >> s;
        return ::to_version(s);
    }

    ::ouster::eOPERATING_MODE to_operating_mode(v1::cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eOPERATING_MODE>(mode); 
    };

    ::ouster::eSENSOR_STATUS to_sensor_status(v1::cDataBuffer& buffer)
    {
        uint8_t status = 0;
        buffer >> status;
        return static_cast<ouster::eSENSOR_STATUS>(status);
    };

    ::ouster::eLIDAR_MODE to_lidar_mode(v1::cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eLIDAR_MODE>(mode);
    };

    ::ouster::eTIMESTAMP_MODE to_timestamp_mode(v1::cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eTIMESTAMP_MODE>(mode);
    };

    ::ouster::eNMEA_BAUD_RATE to_nmea_baud_rate(v1::cDataBuffer& buffer)
    {
        uint8_t rate = 0;
        buffer >> rate;
        return static_cast<ouster::eNMEA_BAUD_RATE>(rate);
    };

    ::ouster::ePOLARITY to_polarity(v1::cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::ePOLARITY>(mode);
    };

    ::ouster::eIO_PIN_MODE to_pin_mode(v1::cDataBuffer& buffer)
    {
        uint8_t mode = 0;
        buffer >> mode;
        return static_cast<ouster::eIO_PIN_MODE>(mode);
    };

    ::ouster::azimuth_status to_azimuth_status(v1::cDataBuffer& buffer)
    {
        int32_t status = 0;
        buffer >> status;
        return static_cast<ouster::azimuth_status>(status);
    };
}

v1::cOusterParser::cOusterParser()
:
    cBlockParser()
{}

v1::cBlockID& v1::cOusterParser::blockID()
{
    return mBlockID;
}


void v1::cOusterParser::processData(BLOCK_MAJOR_VERSION_t major_version,
                                BLOCK_MINOR_VERSION_t minor_version,
                                BLOCK_DATA_ID_t data_id,
                                cDataBuffer& buffer)
{
    mBlockID.setVersion(major_version, minor_version);
    mBlockID.dataID(static_cast<ouster::DataID>(data_id));

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

void v1::cOusterParser::processConfigParam_2(v1::cDataBuffer& buffer)
{
    std::string s = to_string(buffer);

    // Early versions did not record the length of a string
    // 67 is the minimum size of the packet
    auto buf_len = buffer.read_size();
    if (buf_len > 67)
    {
        if (buf_len == 91)
        {
            buffer.read(mConfigParams.udp_ip, 12);
            buffer.read(mConfigParams.udp_dest, 12);
        }
        else if (buf_len == 117)
        {
            buffer.read(mConfigParams.udp_ip, 25);
            buffer.read(mConfigParams.udp_dest, 25);
        }
        else
        {
            buffer >> mConfigParams.udp_ip;
            buffer >> mConfigParams.udp_dest;
        }
    }
    buffer >> mConfigParams.lidar_port;
    buffer >> mConfigParams.imu_port;

    mConfigParams.timestamp_mode = to_timestamp_mode(buffer);
    mConfigParams.sync_pulse_in_polarity = to_polarity(buffer);
    mConfigParams.nmea_in_polarity = to_polarity(buffer);

    buffer >> mConfigParams.nmea_ignore_valid_char;

    mConfigParams.nmea_baud_rate = to_nmea_baud_rate(buffer);

    buffer >> mConfigParams.nmea_leap_seconds;

    mConfigParams.multipurpose_io_mode = to_pin_mode(buffer);
    mConfigParams.sync_pulse_out_polarity = to_polarity(buffer);

    buffer >> mConfigParams.sync_pulse_out_frequency_hz;
    buffer >> mConfigParams.sync_pulse_out_angle;
    buffer >> mConfigParams.sync_pulse_out_pulse_width;
    buffer >> mConfigParams.auto_start_flag;

    mConfigParams.operating_mode = to_operating_mode(buffer);
    mConfigParams.lidar_mode = to_lidar_mode(buffer);

    buffer >> mConfigParams.azimuth_window.min_deg;
    buffer >> mConfigParams.azimuth_window.max_deg;
    buffer >> mConfigParams.phase_lock_enable;
    buffer >> mConfigParams.phase_lock_offset_deg;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processConfigParam_2.");
}

void v1::cOusterParser::processSensorInfo_2(v1::cDataBuffer& buffer)
{
    std::string s = to_string(buffer);

    // Early versions did not record the length of a string
    // 5 is the minimum size of the packet
    auto buf_len = buffer.read_size();
    if (buf_len > 13)
    {
        if (buf_len == 112)
        {
            buffer.read(mSensorInfo.product_line, 8);
            buffer.read(mSensorInfo.product_part_number, 12);
            buffer.read(mSensorInfo.product_serial_number, 12);
            buffer.read(mSensorInfo.image_rev, 47);
        }
        else
        {
            buffer >> mSensorInfo.product_line;
            buffer >> mSensorInfo.product_part_number;
            buffer >> mSensorInfo.product_serial_number;
            buffer >> mSensorInfo.base_part_number;
            buffer >> mSensorInfo.base_serial_number;
            buffer >> mSensorInfo.image_rev;
        }
    }

    if (buf_len == 31)
    {
        mSensorInfo.build_revision = to_version(buffer);
        mSensorInfo.proto_revision = to_version(buffer);
    }
    else
    {
        mSensorInfo.build_revision = to_version(buffer, 6);
        mSensorInfo.proto_revision = to_version(buffer, 6);
    }

    if (buf_len > 13)
    {
        if (buf_len == 112)
        {
            buffer.read(mSensorInfo.build_date, 20);
        }
        else
        {
            buffer >> mSensorInfo.build_date;
        }
    }

    mSensorInfo.status = to_sensor_status(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSensorInfo_2.");
}

void v1::cOusterParser::processTimestamp_2(v1::cDataBuffer& buffer)
{
    buffer >> mTimestamp.time;

    mTimestamp.mode = to_timestamp_mode(buffer);

    buffer >> mTimestamp.sync_pulse_in;
    buffer >> mTimestamp.internal_osc;
    buffer >> mTimestamp.ptp_1588;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processTimestamp_2.");
}

void v1::cOusterParser::processSyncPulseIn_2(v1::cDataBuffer& buffer)
{
    buffer >> mSyncPulseIn.locked;
    buffer >> mSyncPulseIn.last_period_nsec;
    buffer >> mSyncPulseIn.count_unfiltered;
    buffer >> mSyncPulseIn.count;

    mSyncPulseIn.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSyncPulseIn_2.");
}

void v1::cOusterParser::processSyncPulseOut_2(v1::cDataBuffer& buffer)
{
    buffer >> mSyncPulseOut.pulse_width_ms;
    buffer >> mSyncPulseOut.angle_deg;
    buffer >> mSyncPulseOut.frequency_hz;

    mSyncPulseOut.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSyncPulseOut_2.");
}

void v1::cOusterParser::processMultipurposeIO_2(v1::cDataBuffer& buffer)
{
    mMultipurposeIo.mode = to_pin_mode(buffer);

    buffer >> mMultipurposeIo.pulse_width_ms;
    buffer >> mMultipurposeIo.angle_deg;
    buffer >> mMultipurposeIo.frequency_hz;

    mMultipurposeIo.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processMultipurposeIO_2.");
}

void v1::cOusterParser::processNmea_2(v1::cDataBuffer& buffer)
{
    buffer >> mNmea.locked;
    mNmea.baud_rate = to_nmea_baud_rate(buffer);
    buffer >> mNmea.bit_count;
    buffer >> mNmea.bit_count_unfiltered;
    buffer >> mNmea.start_char_count;
    buffer >> mNmea.char_count;
    buffer >> mNmea.last_read_message;
    buffer >> mNmea.date_decoded_count;
    buffer >> mNmea.not_valid_count;
    buffer >> mNmea.utc_decoded_count;
    buffer >> mNmea.leap_seconds;
    buffer >> mNmea.ignore_valid_char;
    mNmea.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processNmea_2.");
}

void v1::cOusterParser::processTimeInfo_2(v1::cDataBuffer& buffer)
{
    /* Timestamp Info */
    buffer >> mTimeInfo.timestamp_info.time;
    mTimeInfo.timestamp_info.mode = to_timestamp_mode(buffer);
    buffer >> mTimeInfo.timestamp_info.sync_pulse_in;
    buffer >> mTimeInfo.timestamp_info.internal_osc;
    buffer >> mTimeInfo.timestamp_info.ptp_1588;

    /* Sync Pulse Info */
    buffer >> mTimeInfo.sync_pulse_info.locked;
    buffer >> mTimeInfo.sync_pulse_info.last_period_nsec;
    buffer >> mTimeInfo.sync_pulse_info.count_unfiltered;
    buffer >> mTimeInfo.sync_pulse_info.count;
    mTimeInfo.sync_pulse_info.polarity = to_polarity(buffer);

    /* Multipurpose IO Info */
    mTimeInfo.multipurpose_io_info.mode = to_pin_mode(buffer);
    buffer >> mTimeInfo.multipurpose_io_info.pulse_width_ms;
    buffer >> mTimeInfo.multipurpose_io_info.angle_deg;
    buffer >> mTimeInfo.multipurpose_io_info.frequency_hz;
    mTimeInfo.multipurpose_io_info.polarity = to_polarity(buffer);

    /* NMEA Info */
    buffer >> mTimeInfo.nmea_info.locked;
    mTimeInfo.nmea_info.baud_rate = to_nmea_baud_rate(buffer);
    buffer >> mTimeInfo.nmea_info.bit_count;
    buffer >> mTimeInfo.nmea_info.bit_count_unfiltered;
    buffer >> mTimeInfo.nmea_info.start_char_count;
    buffer >> mTimeInfo.nmea_info.char_count;
    buffer >> mTimeInfo.nmea_info.last_read_message;
    buffer >> mTimeInfo.nmea_info.date_decoded_count;
    buffer >> mTimeInfo.nmea_info.not_valid_count;
    buffer >> mTimeInfo.nmea_info.utc_decoded_count;
    buffer >> mTimeInfo.nmea_info.leap_seconds;
    buffer >> mTimeInfo.nmea_info.ignore_valid_char;
    mTimeInfo.nmea_info.polarity = to_polarity(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processTimeInfo_2.");
}

void v1::cOusterParser::processBeamIntrinsics_2(v1::cDataBuffer& buffer)
{
    buffer >> mBeamIntrinsics.lidar_to_beam_origins_mm;

    uint32_t n = 0;
    buffer >> n;
    mBeamIntrinsics.azimuth_angles_deg.resize(n);

    for (std::size_t i = 0; i < n; ++i)
        buffer >> mBeamIntrinsics.azimuth_angles_deg[i];

    n = 0;
    buffer >> n;
    mBeamIntrinsics.altitude_angles_deg.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> mBeamIntrinsics.altitude_angles_deg[i];

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processBeamIntrinsics_2.");
}

void v1::cOusterParser::processImuIntrinsics_2(v1::cDataBuffer& buffer)
{
    uint32_t n = 0;
    buffer >> n;
    mImuIntrinsics.imu_to_sensor_transform.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> mImuIntrinsics.imu_to_sensor_transform[i];

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processImuIntrinsics_2.");
}

void v1::cOusterParser::processLidarIntrinsics_2(cDataBuffer& buffer)
{
    uint32_t n = 0;
    buffer >> n;
    mLidarIntrinsics.lidar_to_sensor_transform.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> mLidarIntrinsics.lidar_to_sensor_transform[i];

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processLidarIntrinsics_2.");
}

void v1::cOusterParser::processLidarDataFormat_2(v1::cDataBuffer& buffer)
{
    buffer >> mLidarDataFormat.columns_per_frame;
    buffer >> mLidarDataFormat.columns_per_packet;

    uint32_t n = 0;
    buffer >> n;
    mLidarDataFormat.pixel_shift_by_row.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        buffer >> mLidarDataFormat.pixel_shift_by_row[i];

    buffer >> mLidarDataFormat.pixels_per_column;
    buffer >> mLidarDataFormat.column_window_min;
    buffer >> mLidarDataFormat.column_window_max;

    if (mBlockID.minorVersion() == 3)
    {
        buffer >> mLidarDataFormat.udp_profile_lidar;
        buffer >> mLidarDataFormat.udp_profile_imu;
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processLidarDataFormat_2.");
}

void v1::cOusterParser::processImuData(v1::cDataBuffer& buffer)
{
    buffer >> mImuData.diagnostic_time_ns;
    buffer >> mImuData.accelerometer_read_time_ns;
    buffer >> mImuData.gyroscope_read_time_ns;

    buffer >> mImuData.acceleration_Xaxis_g;
    buffer >> mImuData.acceleration_Yaxis_g;
    buffer >> mImuData.acceleration_Zaxis_g;

    buffer >> mImuData.angular_velocity_Xaxis_deg_per_sec;
    buffer >> mImuData.angular_velocity_Yaxis_deg_per_sec;
    buffer >> mImuData.angular_velocity_Zaxis_deg_per_sec;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processImuData.");
}

void v1::cOusterParser::processLidarData(v1::cDataBuffer& buffer)
{
    uint16_t pixels_per_column = 0;
    uint16_t columns_per_frame = 0;

    buffer >> pixels_per_column;
    buffer >> columns_per_frame;

    mLidarData.resize(pixels_per_column, columns_per_frame);

    ::ouster::lidar_data_block_t pixel;

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
}

void v1::cOusterParser::processLidarDataFrameTimestamp(v1::cDataBuffer& buffer)
{
    uint16_t frame_id = 0;
    uint64_t timestamp_ns = 0;

    buffer >> frame_id;
    buffer >> timestamp_ns;

    mLidarData.frame_id(frame_id);
    mLidarData.timestamp_ns(timestamp_ns);

    processLidarData(buffer);
}


