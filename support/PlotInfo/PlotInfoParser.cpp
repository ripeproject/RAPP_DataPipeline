
#include "PlotInfoParser.hpp"
#include "PlotInfoBlockId.hpp"
#include "PlotInfoDataIdentifiers.hpp"
#include <cbdf/BlockDataFile.hpp>

#include <stdexcept>

using namespace plot_info;


cPlotInfoParser::cPlotInfoParser()
:
    cBlockParser(), mBlockID(new cPlotInfoID())
{}

cPlotInfoParser::~cPlotInfoParser() {}

cBlockID& cPlotInfoParser::blockID()
{
    return *mBlockID;
}

void cPlotInfoParser::processData(BLOCK_MAJOR_VERSION_t major_version,
                                BLOCK_MINOR_VERSION_t minor_version,
                                BLOCK_DATA_ID_t data_id,
                                cDataBuffer& buffer)
{
    mBlockID->setVersion(major_version, minor_version);
    mBlockID->dataID(static_cast<plot_info::DataID>(data_id));

    switch (static_cast<plot_info::DataID>(data_id))
    {
    case DataID::PLOT_ID:
        processPlotID(buffer);
        break;
    case DataID::PLOT_NAME:
        processPlotName(buffer);
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
    case DataID::POINT_CLOUD_DIMENSIONS:
        processDimensions(buffer);
        break;
    case DataID::EVENT:
        processEvent(buffer);
        break;
    case DataID::EVENT_DESCRIPTION:
        processEventDescription(buffer);
        break;
    }
}

void cPlotInfoParser::processPlotID(cDataBuffer& buffer)
{
    int id = buffer.get<int32_t>();

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPlotID.");

    onPlotID(id);
}

void cPlotInfoParser::processPlotName(cDataBuffer& buffer)
{
    std::string name;
    buffer >> name;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPlotName.");

    onPlotName(name);
}

void cPlotInfoParser::processDimensions(cDataBuffer& buffer)
{
    double x_min_m = buffer.get<double>();
    double x_max_m = buffer.get<double>();
    double y_min_m = buffer.get<double>();
    double y_max_m = buffer.get<double>();
    double z_min_m = buffer.get<double>();
    double z_max_m = buffer.get<double>();

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processDimensions.");

    onDimensions(x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m);
}

void cPlotInfoParser::processPointCloudData_1_0(cDataBuffer& buffer)
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

void cPlotInfoParser::processPointCloudData_1_1(cDataBuffer& buffer)
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

void cPlotInfoParser::processEvent(cDataBuffer& buffer)
{
    std::string event_num;
    buffer >> event_num;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processEvent.");

    onEvent(event_num);
}

void cPlotInfoParser::processEventDescription(cDataBuffer& buffer)
{
    std::string event_description;
    buffer >> event_description;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processEventDescription.");

    onEventDescription(event_description);
}


