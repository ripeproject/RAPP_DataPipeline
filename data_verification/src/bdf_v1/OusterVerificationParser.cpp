
#include "OusterVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>

using namespace v1::ouster;

void v1::cOusterVerificationParser::processConfigParam_2(cDataBuffer& buffer)
{
    cOusterParser::processConfigParam_2(buffer);
}

void v1::cOusterVerificationParser::processSensorInfo_2(cDataBuffer& buffer)
{
    cOusterParser::processSensorInfo_2(buffer);
}

void v1::cOusterVerificationParser::processTimestamp_2(cDataBuffer& buffer)
{
    cOusterParser::processTimeInfo_2(buffer);
}

void v1::cOusterVerificationParser::processSyncPulseIn_2(cDataBuffer& buffer)
{
    cOusterParser::processSyncPulseIn_2(buffer);
}

void v1::cOusterVerificationParser::processSyncPulseOut_2(cDataBuffer& buffer)
{
    cOusterParser::processSyncPulseOut_2(buffer);
}

void v1::cOusterVerificationParser::processMultipurposeIO_2(cDataBuffer& buffer)
{
    cOusterParser::processMultipurposeIO_2(buffer);
}

void v1::cOusterVerificationParser::processNmea_2(cDataBuffer& buffer)
{
    cOusterParser::processNmea_2(buffer);
}

void v1::cOusterVerificationParser::processTimeInfo_2(cDataBuffer& buffer)
{
    cOusterParser::processTimeInfo_2(buffer);
}

void v1::cOusterVerificationParser::processBeamIntrinsics_2(cDataBuffer& buffer)
{
    cOusterParser::processBeamIntrinsics_2(buffer);

//    if (mBeamIntrinsics.altitude_angles_deg.empty() || mBeamIntrinsics.azimuth_angles_deg.empty())
//    {
//        throw bdf::invalid_data("Invalid beam intrinsics version 2!");
//    }
}

void v1::cOusterVerificationParser::processImuIntrinsics_2(cDataBuffer& buffer)
{
    cOusterParser::processImuIntrinsics_2(buffer);

//    if (mImuIntrinsics.imu_to_sensor_transform.empty())
//    {
//        throw v1::bdf::invalid_data("Invalid imu intrinsics version 2!");
//    }
}

void v1::cOusterVerificationParser::processLidarIntrinsics_2(cDataBuffer& buffer)
{
    cOusterParser::processLidarIntrinsics_2(buffer);

//    if (mLidarIntrinsics.lidar_to_sensor_transform.empty())
//    {
//        throw v1::bdf::invalid_data("Invalid lidar intrinsics version 2!");
//    }
}

void v1::cOusterVerificationParser::processLidarDataFormat_2(cDataBuffer& buffer)
{
    cOusterParser::processLidarDataFormat_2(buffer);

//    if (mLidarDataFormat.pixels_per_column < 32)
//    {
//        throw v1::bdf::invalid_data("Invalid lidar data format version 2!");
//    }
}

void v1::cOusterVerificationParser::processImuData(cDataBuffer& buffer)
{
    cOusterParser::processImuData(buffer);
}

void v1::cOusterVerificationParser::processLidarData(cDataBuffer& buffer)
{
    cOusterParser::processLidarData(buffer);
}

void v1::cOusterVerificationParser::processLidarDataFrameTimestamp(cDataBuffer& buffer)
{
    cOusterParser::processLidarDataFrameTimestamp(buffer);
}



