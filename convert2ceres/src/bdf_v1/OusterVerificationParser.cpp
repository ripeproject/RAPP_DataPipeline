
#include "OusterVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <ouster/ouster_defs.h>

#include <stdexcept>

using namespace v1::ouster;

void v1::cOusterVerificationParser::processConfigParam_2(cDataBuffer& buffer)
{
    cOusterParser::processConfigParam_2(buffer);

    if (mConfigParams.udp_ip.empty())
        mConfigParams.udp_ip = "fe80::7c35:e17b:e9aa:f24d";
    
    if (mConfigParams.udp_dest.empty())
        mConfigParams.udp_dest = "fe80::7c35:e17b:e9aa:f24d";

    if ((mConfigParams.lidar_port == 0) || (mConfigParams.imu_port == 0))
    {
        mConfigParams.lidar_port = 7502;
        mConfigParams.imu_port = 7503;
        mConfigParams.timestamp_mode = ::ouster::eTIMESTAMP_MODE::TIME_FROM_INTERNAL_OSC;
        mConfigParams.sync_pulse_in_polarity = ::ouster::ePOLARITY::ACTIVE_HIGH;
        mConfigParams.nmea_in_polarity = ::ouster::ePOLARITY::ACTIVE_HIGH;
        mConfigParams.nmea_ignore_valid_char = false;
        mConfigParams.nmea_baud_rate = ::ouster::eNMEA_BAUD_RATE::BAUD_9600;
        mConfigParams.nmea_leap_seconds = 0;
        mConfigParams.multipurpose_io_mode = ::ouster::eIO_PIN_MODE::OFF;
        mConfigParams.sync_pulse_out_polarity = ::ouster::ePOLARITY::ACTIVE_HIGH;
        mConfigParams.sync_pulse_out_frequency_hz = 1;
        mConfigParams.sync_pulse_out_angle = 360;
        mConfigParams.sync_pulse_out_pulse_width = 10;
        mConfigParams.auto_start_flag = true;
        mConfigParams.operating_mode = ::ouster::eOPERATING_MODE::NORMAL;
        mConfigParams.lidar_mode = ::ouster::eLIDAR_MODE::MODE_1024x10;
        mConfigParams.azimuth_window.min_deg = 0.0;
        mConfigParams.azimuth_window.max_deg = 360.0;
        mConfigParams.phase_lock_enable = false;
        mConfigParams.phase_lock_offset_deg = 0;
    }

    onConfigParam(mConfigParams);
}

void v1::cOusterVerificationParser::processSensorInfo_2(cDataBuffer& buffer)
{
    cOusterParser::processSensorInfo_2(buffer);

    if (mSensorInfo.product_line.empty())
        mSensorInfo.product_line = "OS-0-128";

    if (mSensorInfo.product_part_number.empty())
        mSensorInfo.product_part_number = "840-102144-C";

    if (mSensorInfo.product_serial_number.empty())
        mSensorInfo.product_serial_number = "992037000167";

    if (mSensorInfo.image_rev.empty())
        mSensorInfo.image_rev = "ousteros-image-prod-aries-v2.3.0+20220415163956";

    if (mSensorInfo.build_revision.major == 0)
    {
        mSensorInfo.build_revision.major = 2;
        mSensorInfo.build_revision.minor = 3;
        mSensorInfo.build_revision.patch = 0;
    }

    if (mSensorInfo.build_date.empty())
        mSensorInfo.build_date = "2022-04-14T21:11:47Z";

    if ((mSensorInfo.status == ::ouster::eSENSOR_STATUS::UNKNOWN) ||
        (static_cast<int>(mSensorInfo.status) >= static_cast<int>(::ouster::eSENSOR_STATUS::UNCONFIGURED)))
        mSensorInfo.status = ::ouster::eSENSOR_STATUS::RUNNING;

    onSensorInfo(mSensorInfo);
}

void v1::cOusterVerificationParser::processTimestamp_2(cDataBuffer& buffer)
{
    cOusterParser::processTimeInfo_2(buffer);

    onTimestamp(mTimestamp);
}

void v1::cOusterVerificationParser::processSyncPulseIn_2(cDataBuffer& buffer)
{
    cOusterParser::processSyncPulseIn_2(buffer);

    onSyncPulseIn(mSyncPulseIn);
}

void v1::cOusterVerificationParser::processSyncPulseOut_2(cDataBuffer& buffer)
{
    cOusterParser::processSyncPulseOut_2(buffer);
    onSyncPulseOut(mSyncPulseOut);
}

void v1::cOusterVerificationParser::processMultipurposeIO_2(cDataBuffer& buffer)
{
    cOusterParser::processMultipurposeIO_2(buffer);
    onMultipurposeIo(mMultipurposeIo);
}

void v1::cOusterVerificationParser::processNmea_2(cDataBuffer& buffer)
{
    cOusterParser::processNmea_2(buffer);
    onNmea(mNmea);
}

void v1::cOusterVerificationParser::processTimeInfo_2(cDataBuffer& buffer)
{
    cOusterParser::processTimeInfo_2(buffer);
    onTimeInfo(mTimeInfo);
}

void v1::cOusterVerificationParser::processBeamIntrinsics_2(cDataBuffer& buffer)
{
    cOusterParser::processBeamIntrinsics_2(buffer);

    if (mBeamIntrinsics.altitude_angles_deg.empty() || mBeamIntrinsics.azimuth_angles_deg.empty())
    {
        mBeamIntrinsics.altitude_angles_deg = 
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
        
        mBeamIntrinsics.azimuth_angles_deg = 
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

        mBeamIntrinsics.lidar_to_beam_origins_mm = 27.67;
    }

    onBeamIntrinsics(mBeamIntrinsics);
}

void v1::cOusterVerificationParser::processImuIntrinsics_2(cDataBuffer& buffer)
{
    cOusterParser::processImuIntrinsics_2(buffer);

    if (mImuIntrinsics.imu_to_sensor_transform.empty())
    {
        mImuIntrinsics.imu_to_sensor_transform =
            { 1, 0, 0, 6.253, 0, 1, 0, -11.775, 0, 0, 1, 7.645, 0, 0, 0, 1 };
    }

    onImuIntrinsics(mImuIntrinsics);
}

void v1::cOusterVerificationParser::processLidarIntrinsics_2(cDataBuffer& buffer)
{
    cOusterParser::processLidarIntrinsics_2(buffer);

    if (mLidarIntrinsics.lidar_to_sensor_transform.empty())
    {
        mLidarIntrinsics.lidar_to_sensor_transform =
            {-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 36.18, 0, 0, 0, 1};
    }

    onLidarIntrinsics(mLidarIntrinsics);
}

void v1::cOusterVerificationParser::processLidarDataFormat_2(cDataBuffer& buffer)
{
    cOusterParser::processLidarDataFormat_2(buffer);

    if ((mLidarDataFormat.pixels_per_column < 32)
        || (mLidarDataFormat.pixels_per_column > 128))
    {
        mLidarDataFormat.pixels_per_column  = 128;
        mLidarDataFormat.columns_per_packet = 16;
        mLidarDataFormat.columns_per_frame  = 1024;
        mLidarDataFormat.pixel_shift_by_row =
            { 66, 45, 25, 4, 65, 45, 25, 5, 63, 44, 25, 6, 62, 44, 25, 7, 61,
             43, 25, 7, 61, 43, 25, 8, 60, 43, 25, 8, 59, 42, 25, 9, 59, 42,
             25, 9, 58, 42, 25, 9, 58, 42, 25, 9, 58, 42, 25, 9, 57, 41, 25,
              9, 57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 9,
             57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 9, 57, 41, 25, 8, 57,
             41, 24, 8, 57, 41, 24, 8, 57, 41, 24, 7, 57, 41, 24, 7, 58, 41,
             23, 6, 58, 41, 23, 5, 58, 41, 23, 5, 59, 41, 22, 4, 60, 41, 22,
              3, 60, 41, 21, 1, 61, 41, 21, 0 };
        mLidarDataFormat.column_window_min = 0;
        mLidarDataFormat.column_window_max = 1023;
    }

    if (mLidarDataFormat.udp_profile_lidar.empty())
        mLidarDataFormat.udp_profile_lidar = "LEGACY";

    if (mLidarDataFormat.udp_profile_imu.empty())
        mLidarDataFormat.udp_profile_imu = "LEGACY";

    onLidarDataFormat(mLidarDataFormat);
}

void v1::cOusterVerificationParser::processImuData(cDataBuffer& buffer)
{
    cOusterParser::processImuData(buffer);
    onImuData(mImuData);
}

void v1::cOusterVerificationParser::processLidarData(cDataBuffer& buffer)
{
    cOusterParser::processLidarData(buffer);
    onLidarData(mLidarData);
}

void v1::cOusterVerificationParser::processLidarDataFrameTimestamp(cDataBuffer& buffer)
{
    cOusterParser::processLidarDataFrameTimestamp(buffer);
}



