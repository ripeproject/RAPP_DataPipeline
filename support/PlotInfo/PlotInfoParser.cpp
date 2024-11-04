
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
        processName(buffer);
        break;
    case DataID::POINT_CLOUD_DATA:
        processPointCloudData(buffer);
        break;
    case DataID::POINT_CLOUD_DATA_FRAME_ID:
        processPointCloudData_FrameId(buffer);
        break;
    case DataID::POINT_CLOUD_DATA_SENSOR_INFO:
        processPointCloudData_SensorInfo(buffer);
        break;
    case DataID::POINT_CLOUD_DIMENSIONS:
        processDimensions(buffer);
        break;
    case DataID::EVENT:
        processEvent(buffer);
        break;
    case DataID::DESCRIPTION:
        processDescription(buffer);
        break;
    case DataID::SUB_PLOT_ID:
        processSubPlotID(buffer);
        break;
    case DataID::BEGIN_PLOT_INFO_LIST:
        processBeginPlotInfoList(buffer);
        break;
    case DataID::END_PLOT_INFO_LIST:
        processEndPlotInfoList(buffer);
        break;
    case DataID::BEGIN_PLOT_INFO_BLOCK:
        processBeginPlotInfoBlock(buffer);
        break;
    case DataID::END_PLOT_INFO_BLOCK:
        processEndPlotInfoBlock(buffer);
        break;
    case DataID::SPECIES:
        processSpecies(buffer);
        break;
    case DataID::CULTIVAR:
        processCultivar(buffer);
        break;
    case DataID::CONSTRUCT_NAME:
        processConstructName(buffer);
        break;
    case DataID::COPY_NUMBER:
        processCopyNumber(buffer);
        break;
    case DataID::BEGIN_TREATMENT_LIST:
        break;
    case DataID::END_OF_TREATMENT_LIST:
        break;
    case DataID::POT_LABEL:
        processPotLabel(buffer);
        break;
    case DataID::SEED_GENERATION:
         processSeedGeneration(buffer);
       break;
    case DataID::TREATMENT:
        processTreatment(buffer);
        break;
    }
}

void cPlotInfoParser::processBeginPlotInfoList(cDataBuffer& buffer)
{
    onBeginPlotInfoList();
}

void cPlotInfoParser::processEndPlotInfoList(cDataBuffer& buffer)
{
    onEndPlotInfoList();
}

void cPlotInfoParser::processBeginPlotInfoBlock(cDataBuffer& buffer)
{
    onBeginPlotInfoBlock();
}

void cPlotInfoParser::processEndPlotInfoBlock(cDataBuffer& buffer)
{
    onEndPlotInfoBlock();
}

void cPlotInfoParser::processPlotID(cDataBuffer& buffer)
{
    int id = buffer.get<int32_t>();

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPlotID.");

    onPlotID(id);
}

void cPlotInfoParser::processSubPlotID(cDataBuffer& buffer)
{
    int id = buffer.get<int32_t>();

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSubPlotID.");

    onSubPlotID(id);
}

void cPlotInfoParser::processName(cDataBuffer& buffer)
{
    std::string name;
    buffer >> name;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processName.");

    onName(name);
}

void cPlotInfoParser::processDescription(cDataBuffer& buffer)
{
    std::string event_description;
    buffer >> event_description;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processDescription.");

    onDescription(event_description);
}

void cPlotInfoParser::processSpecies(cDataBuffer& buffer)
{
    std::string species;
    buffer >> species;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSpecies.");

    onSpecies(species);
}

void cPlotInfoParser::processCultivar(cDataBuffer& buffer)
{
    std::string cultivar;
    buffer >> cultivar;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processCultivar.");

    onCultivar(cultivar);
}

void cPlotInfoParser::processTreatment(cDataBuffer& buffer)
{

}

void cPlotInfoParser::processConstructName(cDataBuffer& buffer)
{
    std::string name;
    buffer >> name;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processConstructName.");

    onConstructName(name);
}

void cPlotInfoParser::processEvent(cDataBuffer& buffer)
{
    std::string event_num;
    buffer >> event_num;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processEvent.");

    onEvent(event_num);
}

void cPlotInfoParser::processPotLabel(cDataBuffer& buffer)
{
    std::string label;
    buffer >> label;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPotLabel.");

    onPotLabel(label);
}

void cPlotInfoParser::processSeedGeneration(cDataBuffer& buffer)
{
    std::string seed_gen;
    buffer >> seed_gen;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processSeedGeneration.");

    onSeedGeneration(seed_gen);
}

void cPlotInfoParser::processCopyNumber(cDataBuffer& buffer)
{
    std::string copy_num;
    buffer >> copy_num;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processCopyNumber.");

    onCopyNumber(copy_num);
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

    onPlotDimensions(x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m);
}

void cPlotInfoParser::processPointCloudData(cDataBuffer& buffer)
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

    onPlotPointCloudData(pointCloud);
}

void cPlotInfoParser::processPointCloudData_FrameId(cDataBuffer& buffer)
{
    uint64_t num_points = buffer.get<uint64_t>();

    cPointCloud_FrameId pointCloud;
    pointCloud.resize(num_points);

    for (uint32_t n = 0; n < num_points; ++n)
    {
        pointcloud::sCloudPoint_FrameID_t point;
        buffer >> point.X_m;
        buffer >> point.Y_m;
        buffer >> point.Z_m;
        buffer >> point.range_mm;
        buffer >> point.signal;
        buffer >> point.reflectivity;
        buffer >> point.nir;
        buffer >> point.frameID;

        pointCloud.set(n, point);
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPointCloudData_FrameId.");

    onPlotPointCloudData(pointCloud);
}

void cPlotInfoParser::processPointCloudData_SensorInfo(cDataBuffer& buffer)
{
    uint64_t num_points = buffer.get<uint64_t>();

    cPointCloud_SensorInfo pointCloud;
    pointCloud.resize(num_points);

    for (uint32_t n = 0; n < num_points; ++n)
    {
        pointcloud::sCloudPoint_SensorInfo_t point;
        buffer >> point.X_m;
        buffer >> point.Y_m;
        buffer >> point.Z_m;
        buffer >> point.range_mm;
        buffer >> point.signal;
        buffer >> point.reflectivity;
        buffer >> point.nir;
        buffer >> point.frameID;
        buffer >> point.chnNum;
        buffer >> point.pixelNum;

        pointCloud.set(n, point);
    }

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processPointCloudData_SensorInfo.");

    onPlotPointCloudData(pointCloud);
}


