
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

void cPlotInfoSerializer::writeBeginPlotInfoList()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::BEGIN_PLOT_INFO_LIST);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeBeginPlotInfoList data.");

    writeBlock(*mBlockID);
}

void cPlotInfoSerializer::writeEndPlotInfoList()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::END_PLOT_INFO_LIST);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeEndPlotInfoList data.");

    writeBlock(*mBlockID);
}

void cPlotInfoSerializer::writeBeginPlotInfoBlock()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::BEGIN_PLOT_INFO_BLOCK);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeBeginPlotInfoBlock data.");

    writeBlock(*mBlockID);
}

void cPlotInfoSerializer::writeEndPlotInfoBlock()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::END_PLOT_INFO_BLOCK);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeEndPlotInfoBlock data.");

    writeBlock(*mBlockID);
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

void cPlotInfoSerializer::writeSpecies(const std::string& species)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::SPECIES);

    mDataBuffer.clear();
    mDataBuffer << species;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeSpecies data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPlotInfoSerializer::writeCultivar(const std::string& cultivar)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::CULTIVAR);

    mDataBuffer.clear();
    mDataBuffer << cultivar;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeCultivar data.");

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

void cPlotInfoSerializer::writeTreatment(const std::string& treatment)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::TREATMENT);

    mDataBuffer.clear();
    mDataBuffer << treatment;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeTreatment data.");

    writeBlock(*mBlockID, mDataBuffer);
}

void cPlotInfoSerializer::writeBeginTreatments()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::BEGIN_TREATMENT_LIST);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeTreatment data.");

    writeBlock(*mBlockID);

}

void cPlotInfoSerializer::writeEndOfTreatments()
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::END_OF_TREATMENT_LIST);

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeTreatment data.");

    writeBlock(*mBlockID);
}

void cPlotInfoSerializer::writeTreatments(const std::vector<std::string>& treatments)
{
    if (treatments.empty())
        return;

    writeBeginTreatments();

    for (auto& treatment : treatments)
    {
        writeTreatment(treatment);
    }

    writeEndOfTreatments();
}

void cPlotInfoSerializer::writeConstructName(const std::string& name)
{
    setVersion(1, 0);
    mBlockID->dataID(DataID::CONSTRUCT_NAME);

    mDataBuffer.clear();
    mDataBuffer << name;

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing writeConstructName data.");

    writeBlock(*mBlockID, mDataBuffer);
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

void cPlotInfoSerializer::write(const cPointCloud& in)
{
    mBlockID->setVersion(1, 0);
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
        throw std::runtime_error("ERROR, Buffer Overrun in writing sCloudPoint_t data.");

    writeBlock(*mBlockID, mDataBuffer);

    mDataBuffer.capacity(old_size);
}

void cPlotInfoSerializer::write(const cPointCloud_FrameId& in)
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
        throw std::runtime_error("ERROR, Buffer Overrun in writing sCloudPoint_FrameID_t data.");

    writeBlock(*mBlockID, mDataBuffer);

    mDataBuffer.capacity(old_size);
}

void cPlotInfoSerializer::write(const cPointCloud_SensorInfo& in)
{
    mBlockID->setVersion(1, 0);
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
        throw std::runtime_error("ERROR, Buffer Overrun in writing sCloudPoint_SensorInfo_t data.");

    writeBlock(*mBlockID, mDataBuffer);

    mDataBuffer.capacity(old_size);
}


