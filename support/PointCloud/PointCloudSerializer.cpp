
#include "PointCloudSerializer.hpp"
#include "PointCloudBlockId.hpp"
#include "PointCloudDataIdentifiers.hpp"
#include "PointCloudTypes.hpp"
#include "PointCloud.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <cassert>
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

void cPointCloudSerializer::write(const pointcloud::eCOORDINATE_SYSTEM in)
{
    assert(mpDataFile);

    mBlockID->setVersion(1, 0);
	mBlockID->dataID(DataID::COORDINATE_SYSTEM);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(in);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing eCOORDINATE_SYSTEM data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void cPointCloudSerializer::write(const pointcloud::eKINEMATIC_MODEL in)
{
    assert(mpDataFile);

    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::KINEMATICS_MODEL);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(in);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing eKINEMATIC_MODEL data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void cPointCloudSerializer::write(const imu_data_t& in)
{
    assert(mpDataFile);

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

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing imu_data_t data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void cPointCloudSerializer::write(const cReducedPointCloudByFrame& in)
{
    assert(mpDataFile);

    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::REDUCED_POINT_CLOUD_DATA_BY_FRAME);

    mDataBuffer.clear();

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    auto& data = in.data();

    uint32_t num_points = data.size();

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

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cReducedPointCloudByFrame data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void cPointCloudSerializer::write(const cSensorPointCloudByFrame& in)
{
    assert(mpDataFile);

    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::SENSOR_POINT_CLOUD_DATA_BY_FRAME);

    mDataBuffer.clear();

    mDataBuffer.put<uint16_t>(in.frameID());
    mDataBuffer.put<uint64_t>(in.timestamp_ns());

    mDataBuffer.put<uint32_t>(in.channelsPerColumn());
    mDataBuffer.put<uint32_t>(in.columnsPerFrame());

    auto& data = in.data();
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

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing cPointCloud data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}


void cPointCloudSerializer::write(const cPointCloud& in)
{
    assert(mpDataFile);

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

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing point_cloud_t data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());

    mDataBuffer.capacity(old_size);
}

void cPointCloudSerializer::writeSensorAngles(double pitch_deg, double roll_deg, double yaw_deg)
{
    assert(mpDataFile);

    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::SENSOR_ROTATION_ANGLES);

    mDataBuffer.clear();
    mDataBuffer << pitch_deg;
    mDataBuffer << roll_deg;
    mDataBuffer << yaw_deg;

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sensor angles data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void cPointCloudSerializer::writeKinematicSpeed(double vx_mps, double vy_mps, double vz_mps)
{
    assert(mpDataFile);

    mBlockID->setVersion(1, 0);
    mBlockID->dataID(DataID::KINEMATICS_SPEEDS);

    mDataBuffer.clear();
    mDataBuffer << vx_mps;
    mDataBuffer << vy_mps;
    mDataBuffer << vz_mps;

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing kinematic speed data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}
