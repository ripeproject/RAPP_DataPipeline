
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

    eKINEMATIC_MODEL to_kinematics_model(cDataBuffer& buffer)
    {
        uint8_t system = 0;
        buffer >> system;
        return static_cast<eKINEMATIC_MODEL>(system);
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
        switch (major_version)
        {
        case 1:
        {
            switch (minor_version)
            {
            case 0:
                processPointCloudData_1_0(buffer);
                break;
            case 1:
                processPointCloudData_1_1(buffer);
                break;
            }
            break;
        }
        }
        break;
    case DataID::KINEMATICS_MODEL:
        processKinematicsModel(buffer);
        break;
    case DataID::SENSOR_ROTATION_ANGLES:
        processSensorAngles(buffer);
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

void cPointCloudParser::processKinematicsModel(cDataBuffer& buffer)
{
    eKINEMATIC_MODEL model = to_kinematics_model(buffer);

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processKinematicsModel.");

    onKinematicModel(model);
}

void cPointCloudParser::processSensorAngles(cDataBuffer& buffer)
{
    double pitch_deg = buffer.get<double>();
    double roll_deg = buffer.get<double>();
    double yaw_deg = buffer.get<double>();

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSensorAngles.");

    onSensorAngles(pitch_deg, roll_deg, yaw_deg);
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
    uint16_t frameID = buffer.get<uint16_t>();
    uint64_t timestamp_ns = buffer.get<uint64_t>();

    cReducedPointCloudByFrame pointCloud;
    pointCloud.frameID(frameID);
    pointCloud.timestamp_ns(timestamp_ns);

    uint32_t num_points = buffer.get<uint32_t>();

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
    uint16_t frameID = buffer.get<uint16_t>();
    uint64_t timestamp_ns = buffer.get<uint64_t>();

    cSensorPointCloudByFrame pointCloud;
    pointCloud.frameID(frameID);
    pointCloud.timestamp_ns(timestamp_ns);

    uint32_t channelsPerColumn = buffer.get<uint32_t>();
    uint32_t columnsPerFrame = buffer.get<uint32_t>();

    pointCloud.resize(channelsPerColumn, columnsPerFrame);

    for (int c = 0; c < columnsPerFrame; ++c)
    {
        for (int p = 0; p < channelsPerColumn; ++p)
        {
            pointcloud::sCloudPoint_t point;
            buffer >> point.X_m;
            buffer >> point.Y_m;
            buffer >> point.Z_m;
            buffer >> point.range_mm;
            buffer >> point.signal;
            buffer >> point.reflectivity;
            buffer >> point.nir;

            pointCloud.set(c, p, point);
        }
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSensorPointCloudByFrame.");

    onSensorPointCloudByFrame(frameID, timestamp_ns, pointCloud);
}

void cPointCloudParser::processPointCloudData_1_0(cDataBuffer& buffer)
{
    uint32_t num_points = buffer.get<uint32_t>();

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
        throw std::runtime_error("ERROR, Buffer under run in processPointCloudData_1_0.");

    onPointCloudData(pointCloud);
}

void cPointCloudParser::processPointCloudData_1_1(cDataBuffer& buffer)
{
    uint64_t num_points = buffer.get<uint64_t>();

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
        throw std::runtime_error("ERROR, Buffer under run in processPointCloudData_1_1.");

    onPointCloudData(pointCloud);
}

