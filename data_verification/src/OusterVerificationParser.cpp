
#include "OusterVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

using namespace ouster;

void cOusterVerificationParser::onConfigParam(uint8_t instance_id, ouster::config_param_2_t config_param)
{}

void cOusterVerificationParser::onSensorInfo(uint8_t instance_id, ouster::sensor_info_2_t sensor_info)
{}

void cOusterVerificationParser::onTimestamp(uint8_t instance_id, ouster::timestamp_2_t timestamp)
{}

void cOusterVerificationParser::onSyncPulseIn(uint8_t instance_id, ouster::sync_pulse_in_2_t pulse_info)
{}

void cOusterVerificationParser::onSyncPulseOut(uint8_t instance_id, ouster::sync_pulse_out_2_t pulse_info)
{}

void cOusterVerificationParser::onMultipurposeIo(uint8_t instance_id, ouster::multipurpose_io_2_t io)
{}

void cOusterVerificationParser::onNmea(uint8_t instance_id, ouster::nmea_2_t nmea)
{}

void cOusterVerificationParser::onTimeInfo(uint8_t instance_id, ouster::time_info_2_t time_info)
{}

void cOusterVerificationParser::onBeamIntrinsics(uint8_t instance_id, ouster::beam_intrinsics_2_t intrinsics)
{
    if (intrinsics.altitude_angles_deg.empty() || intrinsics.azimuth_angles_deg.empty())
    {
        throw bdf::invalid_data("Invalid beam intrinsics version 2!");
    }
}

void cOusterVerificationParser::onImuIntrinsics(uint8_t instance_id, ouster::imu_intrinsics_2_t intrinsics)
{
    if (intrinsics.imu_to_sensor_transform.empty())
    {
        throw bdf::invalid_data("Invalid imu intrinsics version 2!");
    }
}

void cOusterVerificationParser::onLidarIntrinsics(uint8_t instance_id, ouster::lidar_intrinsics_2_t intrinsics)
{
    if (intrinsics.lidar_to_sensor_transform.empty())
    {
        throw bdf::invalid_data("Invalid lidar intrinsics version 2!");
    }
}

void cOusterVerificationParser::onLidarDataFormat(uint8_t instance_id, ouster::lidar_data_format_2_t format)
{
    if (format.pixels_per_column < 32)
    {
        throw bdf::invalid_data("Invalid lidar data format version 2!");
    }
}

void cOusterVerificationParser::onImuData(uint8_t instance_id, ouster::imu_data_t data)
{}

void cOusterVerificationParser::onLidarData(uint8_t instance_id, cOusterLidarData data)
{
    auto n = data.columnsPerFrame();

    for (std::size_t col = 0; col < n; ++col)
    {
        const auto& channel = data.channels(col);

        for (const auto& pixels : channel)
        {
            if (pixels.range_mm > 1000)
            {
                mNumBadFrames = 0;
                return;
            }
        }
    }

    ++mNumBadFrames;

    if (mNumBadFrames > mMaxNumBadFrames)
    {
        throw bdf::invalid_data("Missing lidar data!");
    }
}

void cOusterVerificationParser::processConfigParam_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processSensorInfo_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processTimestamp_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processSyncPulseIn_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processSyncPulseOut_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processMultipurposeIO_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processNmea_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processTimeInfo_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processBeamIntrinsics_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processImuIntrinsics_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processLidarIntrinsics_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processLidarDataFormat_2(cDataBuffer& buffer)
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

void cOusterVerificationParser::processImuData(cDataBuffer& buffer)
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

void cOusterVerificationParser::processLidarData(cDataBuffer& buffer)
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

void cOusterVerificationParser::processLidarDataFrameTimestamp(cDataBuffer& buffer)
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


