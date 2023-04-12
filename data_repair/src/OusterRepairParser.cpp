
#include "OusterVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

using namespace ouster;

void cOusterVerificationParser::onConfigParam(ouster::config_param_2_t config_param)
{}

void cOusterVerificationParser::onSensorInfo(ouster::sensor_info_2_t sensor_info)
{}

void cOusterVerificationParser::onTimestamp(ouster::timestamp_2_t timestamp)
{}

void cOusterVerificationParser::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info)
{}

void cOusterVerificationParser::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info)
{}

void cOusterVerificationParser::onMultipurposeIo(ouster::multipurpose_io_2_t io)
{}

void cOusterVerificationParser::onNmea(ouster::nmea_2_t nmea)
{}

void cOusterVerificationParser::onTimeInfo(ouster::time_info_2_t time_info)
{}

void cOusterVerificationParser::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics)
{
    if (intrinsics.altitude_angles_deg.empty() || intrinsics.azimuth_angles_deg.empty())
    {
        throw bdf::invalid_data("Invalid beam intrinsics version 2!");
    }
}

void cOusterVerificationParser::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics)
{
    if (intrinsics.imu_to_sensor_transform.empty())
    {
        throw bdf::invalid_data("Invalid imu intrinsics version 2!");
    }
}

void cOusterVerificationParser::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics)
{
    if (intrinsics.lidar_to_sensor_transform.empty())
    {
        throw bdf::invalid_data("Invalid lidar intrinsics version 2!");
    }
}

void cOusterVerificationParser::onLidarDataFormat(ouster::lidar_data_format_2_t format)
{
    if (format.pixels_per_column < 32)
    {
        throw bdf::invalid_data("Invalid lidar data format version 2!");
    }
}

void cOusterVerificationParser::onImuData(ouster::imu_data_t data)
{}

void cOusterVerificationParser::onLidarData(cOusterLidarData data)
{}

