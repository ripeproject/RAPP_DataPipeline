
#include "OusterRepairParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

using namespace ouster;

cOusterRepairParser::cOusterRepairParser()
{
    mSerializer.setBufferCapacity(256 * 1024 * 1024);
}

void cOusterRepairParser::attach(cBlockDataFileWriter* pDataFile)
{
    mSerializer.attach(pDataFile);
}

cBlockDataFileWriter* cOusterRepairParser::detach()
{
    return mSerializer.detach();
}

void cOusterRepairParser::onConfigParam(uint8_t instance_id, ouster::config_param_2_t config_param)
{
    if (config_param.udp_ip.empty())
        config_param.udp_ip = "fe80::7c35:e17b:e9aa:f24d";

    if (config_param.udp_dest.empty())
        config_param.udp_dest = "fe80::7c35:e17b:e9aa:f24d";

    if ((config_param.lidar_port == 0) || (config_param.imu_port == 0))
    {
        config_param.lidar_port = 7502;
        config_param.imu_port = 7503;
        config_param.timestamp_mode = ::ouster::eTIMESTAMP_MODE::TIME_FROM_INTERNAL_OSC;
        config_param.sync_pulse_in_polarity = ::ouster::ePOLARITY::ACTIVE_HIGH;
        config_param.nmea_in_polarity = ::ouster::ePOLARITY::ACTIVE_HIGH;
        config_param.nmea_ignore_valid_char = false;
        config_param.nmea_baud_rate = ::ouster::eNMEA_BAUD_RATE::BAUD_9600;
        config_param.nmea_leap_seconds = 0;
        config_param.multipurpose_io_mode = ::ouster::eIO_PIN_MODE::OFF;
        config_param.sync_pulse_out_polarity = ::ouster::ePOLARITY::ACTIVE_HIGH;
        config_param.sync_pulse_out_frequency_hz = 1;
        config_param.sync_pulse_out_angle = 360;
        config_param.sync_pulse_out_pulse_width = 10;
        config_param.auto_start_flag = true;
        config_param.operating_mode = ::ouster::eOPERATING_MODE::NORMAL;
        config_param.lidar_mode = ::ouster::eLIDAR_MODE::MODE_1024x10;
        config_param.azimuth_window.min_deg = 0.0;
        config_param.azimuth_window.max_deg = 360.0;
        config_param.phase_lock_enable = false;
        config_param.phase_lock_offset_deg = 0;
    }

    mSerializer.write(instance_id, config_param);
}

void cOusterRepairParser::onSensorInfo(uint8_t instance_id, ouster::sensor_info_2_t sensor_info)
{
    if (sensor_info.product_line.empty())
        sensor_info.product_line = "OS-0-128";

    if (sensor_info.product_part_number.empty())
        sensor_info.product_part_number = "840-102144-C";

    if (sensor_info.product_serial_number.empty())
        sensor_info.product_serial_number = "992037000167";

    if (sensor_info.image_rev.empty())
        sensor_info.image_rev = "ousteros-image-prod-aries-v2.3.0+20220415163956";

    if (sensor_info.build_revision.major == 0)
    {
        sensor_info.build_revision.major = 2;
        sensor_info.build_revision.minor = 3;
        sensor_info.build_revision.patch = 0;
    }

    if (sensor_info.build_date.empty())
        sensor_info.build_date = "2022-04-14T21:11:47Z";

    if ((sensor_info.status == ::ouster::eSENSOR_STATUS::UNKNOWN) ||
        (static_cast<int>(sensor_info.status) >= static_cast<int>(::ouster::eSENSOR_STATUS::UNCONFIGURED)))
        sensor_info.status = ::ouster::eSENSOR_STATUS::RUNNING;

    mSerializer.write(instance_id, sensor_info);
}

void cOusterRepairParser::onTimestamp(uint8_t instance_id, ouster::timestamp_2_t timestamp)
{
    mSerializer.write(instance_id, timestamp);
}

void cOusterRepairParser::onSyncPulseIn(uint8_t instance_id, ouster::sync_pulse_in_2_t pulse_info)
{
    mSerializer.write(instance_id, pulse_info);
}

void cOusterRepairParser::onSyncPulseOut(uint8_t instance_id, ouster::sync_pulse_out_2_t pulse_info)
{
    mSerializer.write(instance_id, pulse_info);
}

void cOusterRepairParser::onMultipurposeIo(uint8_t instance_id, ouster::multipurpose_io_2_t io)
{
    mSerializer.write(instance_id, io);
}

void cOusterRepairParser::onNmea(uint8_t instance_id, ouster::nmea_2_t nmea)
{
    mSerializer.write(instance_id, nmea);
}

void cOusterRepairParser::onTimeInfo(uint8_t instance_id, ouster::time_info_2_t time_info)
{
    mSerializer.write(instance_id, time_info);
}

void cOusterRepairParser::onBeamIntrinsics(uint8_t instance_id, ouster::beam_intrinsics_2_t intrinsics)
{
    if (intrinsics.altitude_angles_deg.empty() || intrinsics.azimuth_angles_deg.empty())
    {
        intrinsics.altitude_angles_deg =
            { 46.25, 45.24, 44.54, 44.12, 43.24, 42.27, 41.58, 41.16, 40.28, 39.31, 38.64,
            38.21, 37.33, 36.4, 35.71, 35.27, 34.38, 33.47, 32.81, 32.36, 31.47, 30.59,
            29.93, 29.46, 28.56, 27.71, 27.06, 26.58, 25.68, 24.84, 24.19, 23.71, 22.81,
            21.99, 21.35, 20.85, 19.94, 19.17, 18.52, 18, 17.09, 16.33, 15.71, 15.16, 14.26,
            13.53, 12.9, 12.34, 11.43, 10.73, 10.09, 9.52, 8.619999999999999, 7.93, 7.3, 6.72,
            5.81, 5.14, 4.51, 3.9, 3, 2.35, 1.72, 1.09, 0.19, -0.43, -1.07, -1.72, -2.63, -3.24,
            -3.86, -4.54, -5.44, -6.04, -6.66, -7.36, -8.27, -8.84, -9.48, -10.19, -11.1, -11.65,
            -12.29, -13.02, -13.93, -14.47, -15.11, -15.88, -16.78, -17.3, -17.95, -18.73, -19.64,
            -20.15, -20.8, -21.61, -22.51, -23, -23.66, -24.5, -25.39, -25.88, -26.54, -27.4,
            -28.29, -28.77, -29.43, -30.31, -31.2, -31.67, -32.34, -33.25, -34.13, -34.58, -35.27,
            -36.21, -37.09, -37.53, -38.22, -39.19, -40.08, -40.51, -41.21, -42.21, -43.1, -43.53,
            -44.24, -45.26 };

        intrinsics.azimuth_angles_deg =
            { 11.59, 4.26, -2.95, -10.05, 11.09, 4.06, -2.88, -9.73, 10.69, 3.89, -2.82,
            -9.470000000000001, 10.33, 3.73, -2.78, -9.23, 10.01, 3.61, -2.74, -9.029999999999999,
            9.74, 3.49, -2.7, -8.859999999999999, 9.5, 3.39, -2.68, -8.720000000000001,
            9.300000000000001, 3.29, -2.67, -8.6, 9.109999999999999, 3.2, -2.66, -8.51, 8.949999999999999,
            3.14, -2.66, -8.44, 8.82, 3.06, -2.66, -8.369999999999999, 8.69, 3.01, -2.65, -8.33,
            8.609999999999999, 2.95, -2.68, -8.32, 8.52, 2.9, -2.7, -8.31, 8.460000000000001, 2.87, -2.72,
            -8.32, 8.41, 2.82, -2.75, -8.35, 8.380000000000001, 2.79, -2.78, -8.380000000000001, 8.35, 2.76,
            -2.81, -8.44, 8.34, 2.75, -2.87, -8.5, 8.35, 2.72, -2.9, -8.58, 8.369999999999999, 2.7, -2.97,
            -8.68, 8.4, 2.71, -3.03, -8.81, 8.44, 2.69, -3.11, -8.94, 8.51, 2.68, -3.18, -9.1, 8.59, 2.67,
            -3.27, -9.279999999999999, 8.69, 2.68, -3.34, -9.49, 8.800000000000001, 2.69, -3.46,
            -9.720000000000001, 8.949999999999999, 2.71, -3.59, -10, 9.119999999999999, 2.74, -3.73, -10.31,
            9.34, 2.76, -3.89, -10.68, 9.58, 2.79, -4.07, -11.1, 9.880000000000001, 2.85, -4.28, -11.61 };

        intrinsics.lidar_to_beam_origins_mm = 27.67;
    }

    mSerializer.write(instance_id, intrinsics);
}

void cOusterRepairParser::onImuIntrinsics(uint8_t instance_id, ouster::imu_intrinsics_2_t intrinsics)
{
    if (intrinsics.imu_to_sensor_transform.empty())
    {
        intrinsics.imu_to_sensor_transform =
            { 1, 0, 0, 6.253, 0, 1, 0, -11.775, 0, 0, 1, 7.645, 0, 0, 0, 1 };
    }

    mSerializer.write(instance_id, intrinsics);
}

void cOusterRepairParser::onLidarIntrinsics(uint8_t instance_id, ouster::lidar_intrinsics_2_t intrinsics)
{
    if (intrinsics.lidar_to_sensor_transform.empty())
    {
        intrinsics.lidar_to_sensor_transform =
            { -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 36.18, 0, 0, 0, 1 };
    }

    mSerializer.write(instance_id, intrinsics);
}

void cOusterRepairParser::onLidarDataFormat(uint8_t instance_id, ouster::lidar_data_format_2_t format)
{
    if ((format.pixels_per_column < 32)
        || (format.pixels_per_column > 128))
    {
        format.pixels_per_column = 128;
        format.columns_per_packet = 16;
        format.columns_per_frame = 1024;
        format.pixel_shift_by_row =
            { 66, 45, 25, 4, 65, 45, 25, 5, 63, 44, 25, 6, 62, 44, 25, 7, 61,
             43, 25, 7, 61, 43, 25, 8, 60, 43, 25, 8, 59, 42, 25, 9, 59, 42,
             25, 9, 58, 42, 25, 9, 58, 42, 25, 9, 58, 42, 25, 9, 57, 41, 25,
              9, 57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 9,
             57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 8, 57,
             41, 24, 8, 57, 41, 24, 8, 57, 41, 24, 7, 57, 41, 24, 7, 58, 41,
             23, 6, 58, 41, 23, 5, 58, 41, 23, 5, 59, 41, 22, 4, 60, 41, 22,
              3, 60, 41, 21, 1, 61, 41, 21, 0 };
        format.column_window_min = 0;
        format.column_window_max = 1023;
    }

    if (format.udp_profile_lidar.empty())
        format.udp_profile_lidar = "LEGACY";

    if (format.udp_profile_imu.empty())
        format.udp_profile_imu = "LEGACY";

    mSerializer.write(instance_id, format);
}

void cOusterRepairParser::onImuData(uint8_t instance_id, ouster::imu_data_t data)
{
    if ((data.acceleration_Xaxis_g == 0.0) &&
        (data.acceleration_Yaxis_g == 0.0) &&
        (data.acceleration_Zaxis_g == 0.0))
    {
        throw bdf::invalid_data("Bad lidar data");
    }

    mSerializer.write(instance_id, data);
}

void cOusterRepairParser::onLidarData(uint8_t instance_id, cOusterLidarData data)
{
    if (data.empty())
    {
        throw bdf::invalid_data("Bad lidar data");
    }

    const auto& matrix = data.data();

    bool data_valid = false;

    for (const auto& pixels : matrix.data())
    {
        if (pixels.range_mm > 0)
        {
            mNumBadFrames = 0;
            data_valid = true;
            break;
        }
    }

    if (data_valid)
    {
        mSerializer.write(instance_id, data.frame_id(), data);
        return;
    }

    ++mNumBadFrames;

    if (mNumBadFrames > mMaxNumBadFrames)
    {
        throw bdf::invalid_data("Missing lidar data!");
    }

    mSerializer.write(instance_id, data.frame_id(), data);
}

void cOusterRepairParser::processConfigParam_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processConfigParam_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processConfigParam_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processSensorInfo_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processSensorInfo_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processSensorInfo_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processTimestamp_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processTimestamp_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processTimestamp_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processSyncPulseIn_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processSyncPulseIn_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processSyncPulseIn_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processSyncPulseOut_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processSyncPulseOut_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processSyncPulseOut_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processMultipurposeIO_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processMultipurposeIO_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processMultipurposeIO_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processNmea_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processNmea_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processNmea_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processTimeInfo_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processTimeInfo_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processTimeInfo_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processBeamIntrinsics_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processBeamIntrinsics_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processBeamIntrinsics_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processImuIntrinsics_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processImuIntrinsics_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processImuIntrinsics_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processLidarIntrinsics_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processLidarIntrinsics_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processLidarIntrinsics_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processLidarDataFormat_2(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processLidarDataFormat_2(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processLidarDataFormat_2: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processImuData(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processImuData(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processImuData: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processLidarData(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processLidarData(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processLidarData: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}

void cOusterRepairParser::processLidarDataFrameTimestamp(cDataBuffer& buffer)
{
    try
    {
        cOusterParser::processLidarDataFrameTimestamp(buffer);
    }
    catch (const std::exception& e)
    {
        std::string msg = "processLidarDataFrameTimestamp: ";
        msg += e.what();
        throw bdf::invalid_data(msg);
    }
}


