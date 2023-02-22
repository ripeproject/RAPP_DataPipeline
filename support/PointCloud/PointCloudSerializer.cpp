
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

void cPointCloudSerializer::write(const pointcloud::eCOORDINATE_SYSTEM& in)
{
    assert(mpDataFile);

	mBlockID->dataID(DataID::COORDINATE_SYSTEM);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(in);

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing eCOORDINATE_SYSTEM data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void cPointCloudSerializer::write(const imu_data_t& in)
{
    assert(mpDataFile);

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

    mBlockID->dataID(DataID::POINT_CLOUD_DATA);

    mDataBuffer.clear();

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
        throw std::runtime_error("ERROR, Buffer Overrun in writing point_cloud_t data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

