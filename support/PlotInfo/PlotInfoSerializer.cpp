
#include "PlotInfoSerializer.hpp"
#include "PlotInfoBlockId.hpp"
#include "PlotInfoDataIdentifiers.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <stdexcept>

using namespace plot_info;

cPlotInfoSerializer::cPlotInfoSerializer()
:
    cBlockSerializer(), mBlockID(new cPlotInfoID())
{}

cPlotInfoSerializer::cPlotInfoSerializer(std::size_t n, cBlockDataFileWriter* pDataFile)
:
    cBlockSerializer(n, pDataFile), mBlockID(new cPlotInfoID())
{
}

cPlotInfoSerializer::~cPlotInfoSerializer() {};

cBlockID& cPlotInfoSerializer::blockID()
{
    return *mBlockID;
}

void cPlotInfoSerializer::writeID(int id)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::PLOT_ID);

    mDataBuffer.clear();
    mDataBuffer << static_cast<int32_t>(id);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeID data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPlotInfoSerializer::writeName(const std::string& name)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::PLOT_NAME);

    mDataBuffer.clear();
    mDataBuffer << name;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeName data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPlotInfoSerializer::write(const cPointCloud& in)
{
    mBlockID->setVersion(1, 1);
    mBlockID->dataID(DataID::POINT_CLOUD_DATA);

    mDataBuffer.clear();

    auto& data = in.data();

    uint64_t num_points = data.size();

    auto old_size = mDataBuffer.capacity();
    cDataBuffer::size_type needed_size = 128 + num_points * sizeof(pointcloud::sCloudPoint_t);

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

void cPlotInfoSerializer::writeDimensions(double x_min_m, double x_max_m,
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

void cPlotInfoSerializer::writeEvent(const std::string& event)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::EVENT);

    mDataBuffer.clear();
    mDataBuffer << event;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeEvent data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPlotInfoSerializer::writeDescription(const std::string& description)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::DESCRIPTION);

    mDataBuffer.clear();
    mDataBuffer << description;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeDescription data.");

    writeBlock(*mBlockID, mDataBuffer);
}


