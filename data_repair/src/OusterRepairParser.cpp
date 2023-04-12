
#include "OusterRepairParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

using namespace ouster;

void cOusterRepairParser::onConfigParam(ouster::config_param_2_t config_param)
{}

void cOusterRepairParser::onSensorInfo(ouster::sensor_info_2_t sensor_info)
{}

void cOusterRepairParser::onTimestamp(ouster::timestamp_2_t timestamp)
{}

void cOusterRepairParser::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info)
{}

void cOusterRepairParser::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info)
{}

void cOusterRepairParser::onMultipurposeIo(ouster::multipurpose_io_2_t io)
{}

void cOusterRepairParser::onNmea(ouster::nmea_2_t nmea)
{}

void cOusterRepairParser::onTimeInfo(ouster::time_info_2_t time_info)
{}

void cOusterRepairParser::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics)
{
    if (intrinsics.altitude_angles_deg.empty() || intrinsics.azimuth_angles_deg.empty())
    {
        throw bdf::invalid_data("Invalid beam intrinsics version 2!");
    }
}

void cOusterRepairParser::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics)
{
    if (intrinsics.imu_to_sensor_transform.empty())
    {
        throw bdf::invalid_data("Invalid imu intrinsics version 2!");
    }
}

void cOusterRepairParser::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics)
{
    if (intrinsics.lidar_to_sensor_transform.empty())
    {
        throw bdf::invalid_data("Invalid lidar intrinsics version 2!");
    }
}

void cOusterRepairParser::onLidarDataFormat(ouster::lidar_data_format_2_t format)
{
    if (format.pixels_per_column < 32)
    {
        throw bdf::invalid_data("Invalid lidar data format version 2!");
    }
}

void cOusterRepairParser::onImuData(ouster::imu_data_t data)
{}

void cOusterRepairParser::onLidarData(cOusterLidarData data)
{}

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


