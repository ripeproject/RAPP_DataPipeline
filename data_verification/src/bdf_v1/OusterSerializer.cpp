
#include "OusterSerializer.hpp"
#include "OusterDataIdentifiers.hpp"
#include "BlockDataFile.hpp"

#include "ouster_defs.h"
#include "ouster_utils.h"
#include "OusterLidarData.h"

#include <cassert>
#include <stdexcept>

using namespace ouster;

v1::cOusterSerializer::cOusterSerializer()
:
    cBlockSerializer()
{}

v1::cOusterSerializer::cOusterSerializer(std::size_t n, cBlockDataFileWriter* pDataFile)
:
    cBlockSerializer(n, pDataFile)
{
}

v1::cBlockID& v1::cOusterSerializer::blockID()
{
    return mBlockID;
}

void v1::cOusterSerializer::write(const ouster::config_param_2_t& in)
{
    assert(mpDataFile);

	mBlockID.dataID(DataID::CONFIGURATION_INFO);

    mDataBuffer.clear();
    mDataBuffer <<in.udp_ip;
    mDataBuffer << in.udp_dest;
    mDataBuffer << in.lidar_port;
    mDataBuffer << in.imu_port;
    mDataBuffer << to_uint8(in.timestamp_mode);
    mDataBuffer << to_uint8(in.sync_pulse_in_polarity);
    mDataBuffer << to_uint8(in.nmea_in_polarity);
    mDataBuffer << in.nmea_ignore_valid_char;
    mDataBuffer << to_uint8(in.nmea_baud_rate);
    mDataBuffer << in.nmea_leap_seconds;
    mDataBuffer << to_uint8(in.multipurpose_io_mode);
    mDataBuffer << to_uint8(in.sync_pulse_out_polarity);
    mDataBuffer << in.sync_pulse_out_frequency_hz;
    mDataBuffer << in.sync_pulse_out_angle;
    mDataBuffer << in.sync_pulse_out_pulse_width;
    mDataBuffer << in.auto_start_flag;
    mDataBuffer << to_uint8(in.operating_mode);
    mDataBuffer << to_uint8(in.lidar_mode);
    mDataBuffer << in.azimuth_window.min_deg;
    mDataBuffer << in.azimuth_window.max_deg;
    mDataBuffer << in.phase_lock_enable;
    mDataBuffer << in.phase_lock_offset_deg;

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing config_param_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::sensor_info_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::SENSOR_INFO);

    mDataBuffer.clear();
    mDataBuffer << in.product_line;
    mDataBuffer << in.product_part_number;
    mDataBuffer << in.product_serial_number;
    mDataBuffer << in.base_part_number;
    mDataBuffer << in.base_serial_number;
    mDataBuffer << in.image_rev;
    mDataBuffer << to_string(in.build_revision);
    mDataBuffer << to_string(in.proto_revision);
    mDataBuffer << in.build_date;
    mDataBuffer << to_uint8(in.status);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sensor_info_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::timestamp_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::TIMESTAMP);

    mDataBuffer.clear();
    mDataBuffer << in.time;
    mDataBuffer << to_uint8(in.mode);
    mDataBuffer << in.sync_pulse_in;
    mDataBuffer << in.internal_osc;
    mDataBuffer << in.ptp_1588;

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing timestamp_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::sync_pulse_in_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::SYNC_PULSE_IN);

    mDataBuffer.clear();
    mDataBuffer << in.locked;
    mDataBuffer << in.last_period_nsec;
    mDataBuffer << in.count_unfiltered;
    mDataBuffer << in.count;
    mDataBuffer << to_uint8(in.polarity);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sync_pulse_in_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::sync_pulse_out_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::SYNC_PULSE_OUT);

    mDataBuffer.clear();
    mDataBuffer << in.pulse_width_ms;
    mDataBuffer << in.angle_deg;
    mDataBuffer << in.frequency_hz;
    mDataBuffer << to_uint8(in.polarity);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sync_pulse_out_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::multipurpose_io_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::MULTIPURPOSE_IO);

    mDataBuffer.clear();
    mDataBuffer << to_uint8(in.mode);
    mDataBuffer << in.pulse_width_ms;
    mDataBuffer << in.angle_deg;
    mDataBuffer << in.frequency_hz;
    mDataBuffer << to_uint8(in.polarity);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing multipurpose_io_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::nmea_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::NMEA);

    mDataBuffer.clear();
    mDataBuffer << in.locked;
    mDataBuffer << to_uint8(in.baud_rate);
    mDataBuffer << in.bit_count;
    mDataBuffer << in.bit_count_unfiltered;
    mDataBuffer << in.start_char_count;
    mDataBuffer << in.char_count;
    mDataBuffer << in.last_read_message;
    mDataBuffer << in.date_decoded_count;
    mDataBuffer << in.not_valid_count;
    mDataBuffer << in.utc_decoded_count;
    mDataBuffer << in.leap_seconds;
    mDataBuffer << in.ignore_valid_char;
    mDataBuffer << to_uint8(in.polarity);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing nmea_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::time_info_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::TIME_INFO);

    mDataBuffer.clear();
    /* Timestamp Info */
    mDataBuffer << in.timestamp_info.time;
    mDataBuffer << to_uint8(in.timestamp_info.mode);
    mDataBuffer << in.timestamp_info.sync_pulse_in;
    mDataBuffer << in.timestamp_info.internal_osc;
    mDataBuffer << in.timestamp_info.ptp_1588;

    /* Sync Pulse Info */
    mDataBuffer << in.sync_pulse_info.locked;
    mDataBuffer << in.sync_pulse_info.last_period_nsec;
    mDataBuffer << in.sync_pulse_info.count_unfiltered;
    mDataBuffer << in.sync_pulse_info.count;
    mDataBuffer << to_uint8(in.sync_pulse_info.polarity);

    /* Multipurpose IO Info */
    mDataBuffer << to_uint8(in.multipurpose_io_info.mode);
    mDataBuffer << in.multipurpose_io_info.pulse_width_ms;
    mDataBuffer << in.multipurpose_io_info.angle_deg;
    mDataBuffer << in.multipurpose_io_info.frequency_hz;
    mDataBuffer << to_uint8(in.multipurpose_io_info.polarity);

    /* NMEA Info */
    mDataBuffer << in.nmea_info.locked;
    mDataBuffer << to_uint8(in.nmea_info.baud_rate);
    mDataBuffer << in.nmea_info.bit_count;
    mDataBuffer << in.nmea_info.bit_count_unfiltered;
    mDataBuffer << in.nmea_info.start_char_count;
    mDataBuffer << in.nmea_info.char_count;
    mDataBuffer << in.nmea_info.last_read_message;
    mDataBuffer << in.nmea_info.date_decoded_count;
    mDataBuffer << in.nmea_info.not_valid_count;
    mDataBuffer << in.nmea_info.utc_decoded_count;
    mDataBuffer << in.nmea_info.leap_seconds;
    mDataBuffer << in.nmea_info.ignore_valid_char;
    mDataBuffer << to_uint8(in.nmea_info.polarity);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing time_info_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::beam_intrinsics_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::BEAM_INTRINSICS);

    mDataBuffer.clear();
    mDataBuffer << in.lidar_to_beam_origins_mm;

    uint32_t n = in.azimuth_angles_deg.size();
    mDataBuffer << n;
    for (std::size_t i = 0; i < n; ++i)
        mDataBuffer << in.azimuth_angles_deg[i];

    n = in.altitude_angles_deg.size();
    mDataBuffer << n;
    for (std::size_t i = 0; i < n; ++i)
        mDataBuffer << in.altitude_angles_deg[i];

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing beam_intrinsics_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::imu_intrinsics_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::IMU_INTRINSICS);

    mDataBuffer.clear();

    uint32_t n = in.imu_to_sensor_transform.size();
    mDataBuffer << n;
    for (std::size_t i = 0; i < n; ++i)
        mDataBuffer << in.imu_to_sensor_transform[i];

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing imu_intrinsics_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::lidar_intrinsics_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::LIDAR_INTRINSICS);

    mDataBuffer.clear();

    uint32_t n = in.lidar_to_sensor_transform.size();
    mDataBuffer << n;
    for (std::size_t i = 0; i < n; ++i)
        mDataBuffer << in.lidar_to_sensor_transform[i];

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing lidar_intrinsics_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::lidar_data_format_2_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::LIDAR_DATA_FORMAT);

    mDataBuffer.clear();
    mDataBuffer << in.columns_per_frame;
    mDataBuffer << in.columns_per_packet;

    uint32_t n = in.pixel_shift_by_row.size();
    mDataBuffer << n;
    for (std::size_t i = 0; i < n; ++i)
        mDataBuffer << in.pixel_shift_by_row[i];

    mDataBuffer << in.pixels_per_column;
    mDataBuffer << in.column_window_min;
    mDataBuffer << in.column_window_max;

    if (mBlockID.minorVersion() == 3)
    {
        mDataBuffer << in.udp_profile_lidar;
        mDataBuffer << in.udp_profile_imu;
    }

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing lidar_data_format_2_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(const ouster::imu_data_t& in)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::IMU_DATA);

    mDataBuffer.clear();
    mDataBuffer << in.diagnostic_time_ns;
    mDataBuffer << in.accelerometer_read_time_ns;
    mDataBuffer << in.gyroscope_read_time_ns;

    mDataBuffer << in.acceleration_Xaxis_g;
    mDataBuffer << in.acceleration_Yaxis_g;
    mDataBuffer << in.acceleration_Zaxis_g;

    mDataBuffer << in.angular_velocity_Xaxis_deg_per_sec;
    mDataBuffer << in.angular_velocity_Yaxis_deg_per_sec;
    mDataBuffer << in.angular_velocity_Zaxis_deg_per_sec;

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing imu_data_t data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cOusterSerializer::write(uint16_t frameID, const cOusterLidarData& lidar_data)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::LIDAR_DATA_FRAME_TIMESTAMP);

    mDataBuffer.clear();

    mDataBuffer << frameID;

    uint64_t timestamp = lidar_data.timestamp_ns();
    mDataBuffer << timestamp;

    auto& data = lidar_data.data();
    uint16_t pixels_per_column = data.num_rows();
    uint16_t columns_per_frame = data.num_columns();

    mDataBuffer << pixels_per_column;
    mDataBuffer << columns_per_frame;

    for (uint16_t col = 0; col < columns_per_frame; ++col)
    {
        auto pixels = data.column(col);
        for (uint16_t r = 0; r < pixels_per_column; ++r)
        {
            auto& pixel = pixels[r];
            mDataBuffer << pixel.range_mm;
            mDataBuffer << pixel.signal;
            mDataBuffer << pixel.reflectivity;
            mDataBuffer << pixel.nir;
        }
    }

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cOusterLidarData data.");

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}


