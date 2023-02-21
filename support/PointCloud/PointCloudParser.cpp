
#include "PointCloudParser.hpp"
#include "PointCloudBlockId.hpp"
#include "PointCloudDataIdentifiers.hpp"
#include <cbdf/BlockDataFile.hpp>

#include <stdexcept>

using namespace pointcloud;

namespace
{
    eCOORDINATE_SYSTEM to_coordinate_system(cDataBuffer& buffer)
    {
        uint8_t system = 0;
        buffer >> system;
        return static_cast<eCOORDINATE_SYSTEM>(system);
    };
}

cPointCloudParser::cPointCloudParser()
:
    cBlockParser(), mBlockID(new cPointCloudID())
{}

cPointCloudParser::~cPointCloudParser() {}

cBlockID& cPointCloudParser::blockID()
{
    return *mBlockID;
}


void cPointCloudParser::processData(BLOCK_MAJOR_VERSION_t major_version,
                                BLOCK_MINOR_VERSION_t minor_version,
                                BLOCK_DATA_ID_t data_id,
                                cDataBuffer& buffer)
{
    mBlockID->setVersion(major_version, minor_version);
    mBlockID->dataID(static_cast<pointcloud::DataID>(data_id));

    switch (static_cast<pointcloud::DataID>(data_id))
    {
    case DataID::COORDINATE_SYSTEM:
        processCoordinateSystem(buffer);
        break;
    case DataID::IMU_DATA:
        processImuData(buffer);
        break;
    case DataID::REDUCED_POINT_CLOUD_DATA_BY_FRAME:
        processReducedPointCloudByFrame(buffer);
        break;
    case DataID::SENSOR_POINT_CLOUD_DATA_BY_FRAME:
        processSensorPointCloudByFrame(buffer);
        break;
    case DataID::POINT_CLOUD_DATA:
        processPointCloudData(buffer);
        break;
    }
}

void cPointCloudParser::processCoordinateSystem(cDataBuffer& buffer)
{
    eCOORDINATE_SYSTEM coordinate_sys = to_coordinate_system(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processCoordinateSystem.");

    onCoordinateSystem(coordinate_sys);
}

void cPointCloudParser::processImuData(cDataBuffer& buffer)
{
    imu_data_t data;

    buffer >> data.accelerometer_read_time_ns;
    buffer >> data.gyroscope_read_time_ns;

    buffer >> data.acceleration_X_g;
    buffer >> data.acceleration_Y_g;
    buffer >> data.acceleration_Z_g;

    buffer >> data.angular_velocity_Xaxis_deg_per_sec;
    buffer >> data.angular_velocity_Yaxis_deg_per_sec;
    buffer >> data.angular_velocity_Zaxis_deg_per_sec;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processImuData.");

    onImuData(data);
}

void cPointCloudParser::processReducedPointCloudByFrame(cDataBuffer& buffer)
{
    uint16_t frameID = 0;
    uint64_t timestamp_ns = 0;

    buffer >> frameID;
    buffer >> timestamp_ns;

    cReducedPointCloudByFrame pointCloud;
    pointCloud.frameID(frameID);
    pointCloud.timestamp_ns(timestamp_ns);

    uint32_t num_points = 0;

    buffer >> num_points;
    pointCloud.resize(num_points);

    for (uint32_t n = 0; n < num_points; ++n)
    {
        pointcloud::sCloudPoint_t point;
        buffer >> point.X_m;
        buffer >> point.Y_m;
        buffer >> point.Z_m;
        buffer >> point.range_mm;
        buffer >> point.signal;
        buffer >> point.reflectivity;
        buffer >> point.nir;

        pointCloud.set(n, point);
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processReducedPointCloudByFrame.");

    onReducedPointCloudByFrame(frameID, timestamp_ns, pointCloud);
}

void cPointCloudParser::processSensorPointCloudByFrame(cDataBuffer& buffer)
{
    uint16_t frameID = 0;
    uint64_t timestamp_ns = 0;
    std::vector<pointcloud::sCloudPoint_t> pointCloud;

    buffer >> frameID;
    buffer >> timestamp_ns;

    uint32_t num_points = 0;

    buffer >> num_points;

    for (uint32_t n = 0; n < num_points; ++n)
    {
        pointcloud::sCloudPoint_t point;
        buffer >> point.X_m;
        buffer >> point.Y_m;
        buffer >> point.Z_m;
        buffer >> point.range_mm;
        buffer >> point.signal;
        buffer >> point.reflectivity;
        buffer >> point.nir;

        pointCloud.push_back(point);
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSensorPointCloudByFrame.");

    onSensorPointCloudByFrame(frameID, timestamp_ns, pointCloud);
}

void cPointCloudParser::processPointCloudData(cDataBuffer& buffer)
{
    uint32_t num_points = 0;

    buffer >> num_points;

    cPointCloud pointCloud;
    pointCloud.resize(num_points);

    for (uint32_t n = 0; n < num_points; ++n)
    {
        pointcloud::sCloudPoint_t point;
        buffer >> point.X_m;
        buffer >> point.Y_m;
        buffer >> point.Z_m;
        buffer >> point.range_mm;
        buffer >> point.signal;
        buffer >> point.reflectivity;
        buffer >> point.nir;

        pointCloud.set(n, point);
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPointCloudData.");

    onPointCloudData(pointCloud);
}

/*
void cPointCloudParser::processLidarData(cDataBuffer& buffer)
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
*/

/*
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
*/


