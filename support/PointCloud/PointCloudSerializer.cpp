
#include "PointCloudSerializer.hpp"
#include "PointCloudBlockId.hpp"
#include "PointCloudDataIdentifiers.hpp"
#include "PointCloudTypes.hpp"
#include "PointCloud.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <stdexcept>


using namespace pointcloud;


cPointCloudSerializer::cPointCloudSerializer()
:
    cBlockSerializer(), mBlockID(new cPointCloudID())
{}

cPointCloudSerializer::cPointCloudSerializer(std::size_t n, cBlockDataFileWriter* pDataFile)
:
    cBlockSerializer(n, pDataFile), mBlockID(new cPointCloudID())
{
}

cPointCloudSerializer::~cPointCloudSerializer() {};

cBlockID& cPointCloudSerializer::blockID()
{
    return *mBlockID;
}

void cPointCloudSerializer::writeBeginPointCloudBlock()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::BEGIN_POINT_CLOUD_BLOCK);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing begin point cloud block.");

    writeBlock(*mBlockID);
}

void cPointCloudSerializer::writeEndPointCloudBlock()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::END_POINT_CLOUD_BLOCK);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing end point cloud block.");

    writeBlock(*mBlockID);
}

void cPointCloudSerializer::write(const pointcloud::eCOORDINATE_SYSTEM in)
{
    mBlockID->setVersion(1, 0);
	mBlockID->dataID(DataID::COORDINATE_SYSTEM);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(in);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing eCOORDINATE_SYSTEM data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const pointcloud::eKINEMATIC_MODEL in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::KINEMATICS_MODEL);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(in);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing eKINEMATIC_MODEL data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const imu_data_t& in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::IMU_DATA);

    mDataBuffer.clear();
    mDataBuffer << in.accelerometer_read_time_ns;
    mDataBuffer << in.gyroscope_read_time_ns;

    mDataBuffer << in.acceleration_X_g;
    mDataBuffer << in.acceleration_Y_g;
    mDataBuffer << in.acceleration_Z_g;

    mDataBuffer << in.angular_velocity_Xaxis_deg_per_sec;
    mDataBuffer << in.angular_velocity_Yaxis_deg_per_sec;
    mDataBuffer << in.angular_velocity_Zaxis_deg_per_sec;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing imu_data_t data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const cPointCloud& in)
{
    mBlockID->setVersion(1, 1);
    mBlockID->dataID(DataID::POINT_CLOUD_DATA);

    mDataBuffer.clear();

    auto& data = in.data();

    uint64_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points*sizeof(pointcloud::sCloudPoint_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer << num_points;

    for (uint64_t n = 0; n < num_points; ++n)
    {
        auto point = data[n];

        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing point_cloud_t data.");

    writeBlock(*mBlockID, mDataBuffer);

    mDataBuffer.capacity(old_size);
}

void cPointCloudSerializer::write(const cPointCloud_FrameId& in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::POINT_CLOUD_DATA_FRAME_ID);

    mDataBuffer.clear();

    auto& data = in.data();

    uint64_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_FrameID_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer << num_points;

    for (uint64_t n = 0; n < num_points; ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
        mDataBuffer << point.frameID;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cPointCloud_FrameId data.");

    writeBlock(*mBlockID, mDataBuffer);

    mDataBuffer.capacity(old_size);
}

void cPointCloudSerializer::write(const cPointCloud_SensorInfo& in)
{
    mBlockID->setVersion(2, 0);
    mBlockID->dataID(DataID::POINT_CLOUD_DATA_SENSOR_INFO);

    mDataBuffer.clear();

    auto& data = in.data();

    uint64_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_SensorInfo_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer << num_points;

    for (uint64_t n = 0; n < num_points; ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
        mDataBuffer << point.frameID;
        mDataBuffer << point.chnNum;
        mDataBuffer << point.pixelNum;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cPointCloud_SensorInfo data.");

    writeBlock(*mBlockID, mDataBuffer);

    mDataBuffer.capacity(old_size);
}

void cPointCloudSerializer::writeSensorAngles(double pitch_deg, double roll_deg, double yaw_deg)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::SENSOR_ROTATION_ANGLES);

    mDataBuffer.clear();
    mDataBuffer << pitch_deg;
    mDataBuffer << roll_deg;
    mDataBuffer << yaw_deg;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sensor angles data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeKinematicSpeed(double vx_mps, double vy_mps, double vz_mps)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::KINEMATICS_SPEEDS);

    mDataBuffer.clear();
    mDataBuffer << vx_mps;
    mDataBuffer << vy_mps;
    mDataBuffer << vz_mps;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing kinematic speed data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeSensorRotationalSpeeds(double pitch_dps, double roll_dps, double yaw_dps)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::SENSOR_ROTATIONAL_SPEEDS);

    mDataBuffer.clear();
    mDataBuffer << pitch_dps;
    mDataBuffer << roll_dps;
    mDataBuffer << yaw_dps;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sensor rotation speed data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::POINT_CLOUD_DIMENSIONS);

    mDataBuffer.clear();
    mDataBuffer << x_min_m;
    mDataBuffer << x_max_m;
    mDataBuffer << y_min_m;
    mDataBuffer << y_max_m;
    mDataBuffer << z_min_m;
    mDataBuffer << z_max_m;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing dimemsional data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeDistanceWindow(double min_dist_m, double max_dist_m)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::DISTANCE_WINDOW_M);

    mDataBuffer.clear();
    mDataBuffer << min_dist_m;
    mDataBuffer << max_dist_m;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing distance window data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::AZIMUTH_WINDOW_DEG);

    mDataBuffer.clear();
    mDataBuffer << min_azimuth_deg;
    mDataBuffer << max_azimuth_deg;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing azimuth window data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeAltitudeWindow(double min_altitude_deg, double max_altitude_deg)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::ALTITUDE_WINDOW_DEG);

    mDataBuffer.clear();
    mDataBuffer << min_altitude_deg;
    mDataBuffer << max_altitude_deg;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing altitude window data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeBeginPointCloudList()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::BEGIN_POINT_CLOUD_LIST);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing begin point cloud list.");

    writeBlock(*mBlockID);
}

void cPointCloudSerializer::writeEndPointCloudList()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::END_POINT_CLOUD_LIST);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing end point cloud list.");

    writeBlock(*mBlockID);
}


void cPointCloudSerializer::write(const cReducedPointCloudByFrame& in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::REDUCED_POINT_CLOUD_DATA_BY_FRAME);

    mDataBuffer.clear();

    auto& data = in.data();

    uint32_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer << num_points;

    for (uint32_t n = 0; n < num_points; ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cReducedPointCloudByFrame data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const cReducedPointCloudByFrame_FrameId& in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::REDUCED_POINT_CLOUD_DATA_BY_FRAME_FRAME_ID);

    mDataBuffer.clear();

    auto& data = in.data();

    uint32_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_FrameID_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer << num_points;

    for (uint32_t n = 0; n < num_points; ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
        mDataBuffer << point.frameID;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cReducedPointCloudByFrame_FrameId data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const cReducedPointCloudByFrame_SensorInfo& in)
{
    mBlockID->setVersion(2, 0);
    mBlockID->dataID(DataID::REDUCED_POINT_CLOUD_DATA_BY_FRAME_SENSOR_INFO);

    mDataBuffer.clear();

    auto& data = in.data();

    uint32_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_SensorInfo_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer << num_points;

    for (uint32_t n = 0; n < num_points; ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
        mDataBuffer << point.frameID;
        mDataBuffer << point.chnNum;
        mDataBuffer << point.pixelNum;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cReducedPointCloudByFrame_SensorInfo data.");

    writeBlock(*mBlockID, mDataBuffer);
}


void cPointCloudSerializer::write(const cSensorPointCloudByFrame& in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::SENSOR_POINT_CLOUD_DATA_BY_FRAME);

    mDataBuffer.clear();

    auto& data = in.data();

    uint32_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer.put<uint32_t>(in.channelsPerColumn());
    mDataBuffer.put<uint32_t>(in.columnsPerFrame());

    for (uint32_t n = 0; n < data.size(); ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cSensorPointCloudByFrame data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const cSensorPointCloudByFrame_FrameId& in)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::SENSOR_POINT_CLOUD_DATA_BY_FRAME_FRAME_ID);

    mDataBuffer.clear();

    auto& data = in.data();

    uint32_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_FrameID_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer.put<uint32_t>(in.channelsPerColumn());
    mDataBuffer.put<uint32_t>(in.columnsPerFrame());

    for (uint32_t n = 0; n < data.size(); ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
        mDataBuffer << point.frameID;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cSensorPointCloudByFrame_FrameId data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::write(const cSensorPointCloudByFrame_SensorInfo& in)
{
    mBlockID->setVersion(2, 0);
    mBlockID->dataID(DataID::SENSOR_POINT_CLOUD_DATA_BY_FRAME_SENSOR_INFO);

    mDataBuffer.clear();

    auto& data = in.data();

    uint32_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_SensorInfo_t);

    mDataBuffer.capacity(needed_size);

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer.put<uint32_t>(in.channelsPerColumn());
    mDataBuffer.put<uint32_t>(in.columnsPerFrame());

    for (uint32_t n = 0; n < data.size(); ++n)
    {
        auto point = data[n];
        mDataBuffer << point.X_m;
        mDataBuffer << point.Y_m;
        mDataBuffer << point.Z_m;
        mDataBuffer << point.range_mm;
        mDataBuffer << point.signal;
        mDataBuffer << point.reflectivity;
        mDataBuffer << point.nir;
        mDataBuffer << point.frameID;
        mDataBuffer << point.chnNum;
        mDataBuffer << point.pixelNum;
    }

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cSensorPointCloudByFrame_SensorInfo data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPointCloudSerializer::writeBeginSensorKinematics()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::BEGIN_SENSOR_KINEMATICS);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing begin sensor kinematics data.");

    writeBlock(*mBlockID);
}

void cPointCloudSerializer::writeEndSensorKinematics()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::END_SENSOR_KINEMATICS);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing end sensor kinematics data.");

    writeBlock(*mBlockID);
}

void cPointCloudSerializer::writeSensorKinematicInfo(const pointcloud::sSensorKinematicInfo_t& point)
{
    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::POINT_CLOUD_DIMENSIONS);

    mDataBuffer.clear();
    mDataBuffer.put<double>(point.timestamp_us);
    mDataBuffer << point.X_m;
    mDataBuffer << point.Y_m;
    mDataBuffer << point.Z_m;
    mDataBuffer << point.Vx_mps;
    mDataBuffer << point.Vy_mps;
    mDataBuffer << point.Vz_mps;

    mDataBuffer << point.pitch_deg;
    mDataBuffer << point.roll_deg;
    mDataBuffer << point.yaw_deg;
    mDataBuffer << point.pitchRate_dps;
    mDataBuffer << point.rollRate_dps;
    mDataBuffer << point.yawRate_dps;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sensor kinemation data.");

    writeBlock(*mBlockID, mDataBuffer);
}

