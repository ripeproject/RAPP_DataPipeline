
#include "pointcloud2ply.hpp"
//#include "PointCloudTypes.hpp"

#include "Constants.hpp"


#include <tinyply.h>

#include <iostream>


//#define USE_BINARY

bool cPointCloud2Ply::mIndividualPlyFiles = false;
bool cPointCloud2Ply::mSaveDollyPositions = false;
bool cPointCloud2Ply::mUseBinaryFormat    = false;
bool cPointCloud2Ply::mResetOrigin        = false;

cPointCloud2Ply::cPointCloud2Ply() : cPointCloudParser()
{
    uint8_t R = static_cast<uint8_t>(rand() % 255);
    uint8_t G = static_cast<uint8_t>(rand() % 255);
    uint8_t B = static_cast<uint8_t>(rand() % 255);

    mColor = { R, G, B };
}

cPointCloud2Ply::~cPointCloud2Ply()
{
    if (!mVertices.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext;

        if (mIndividualPlyFiles)
            ext += std::to_string(mFrameCount);

        ext += ".ply";

        filename += ext;
//        filename.replace_extension(ext);

        writePointcloud(filename);
    }


    if (!mPositions.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = ".position.ply";

        filename.replace_extension(ext);

        writePosition(filename);
    }
}

void cPointCloud2Ply::close()
{
    if (!mVertices.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext;

        if (mIndividualPlyFiles)
            ext += std::to_string(mFrameCount);

        ext += ".ply";

        filename += ext;
        //        filename.replace_extension(ext);

        writePointcloud(filename);
    }


    if (!mPositions.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = ".position.ply";

        filename.replace_extension(ext);

        writePosition(filename);
    }
}

void cPointCloud2Ply::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cPointCloud2Ply::onBeginPointCloudBlock() {}
void cPointCloud2Ply::onEndPointCloudBlock() {}
void cPointCloud2Ply::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) {}
void cPointCloud2Ply::onKinematicModel(pointcloud::eKINEMATIC_MODEL model) {}
void cPointCloud2Ply::onDistanceWindow(double min_dist_m, double max_dist_m) {}
void cPointCloud2Ply::onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg) {}
void cPointCloud2Ply::onAltitudeWindow(double min_altitude_deg, double max_altitude_deg) {}

void cPointCloud2Ply::onReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm) {}

void cPointCloud2Ply::onVegetationOnly(const bool vegetation_only) {}

void cPointCloud2Ply::onGroundLevel(double ground_level_mm)
{
    mGroundLevel_mm = ground_level_mm;
}

void cPointCloud2Ply::onDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m) {}

void cPointCloud2Ply::onBeginSensorKinematics()
{}

void cPointCloud2Ply::onEndSensorKinematics() 
{}

void cPointCloud2Ply::onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point)
{}

void cPointCloud2Ply::onPointCloudData(cPointCloud pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(cPointCloud_FrameId pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<uint16_t>  frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(point.frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(cPointCloud_SensorInfo pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<uint16_t>  frameIDs;
    std::vector<pixel_loc> pixel_locs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(point.frameID);

        pixel_loc ploc;
        ploc.chn = point.chnNum;
        ploc.pixel = point.pixelNum;
        pixel_locs.push_back(ploc);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());
    mPixelLocations.insert(mPixelLocations.end(), pixel_locs.begin(), pixel_locs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onBeginPointCloudList() {}
void cPointCloud2Ply::onEndPointCloudList() {}

void cPointCloud2Ply::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3>  vertices;
    std::vector<range_t>    ranges;
    std::vector<returns3>   returns;
    std::vector<uint16_t>   frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<uint16_t>  frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(point.frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<uint16_t>  frameIDs;
    std::vector<pixel_loc> pixel_locs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(point.frameID);

        pixel_loc ploc;
        ploc.chn = point.chnNum;
        ploc.pixel = point.pixelNum;
        pixel_locs.push_back(ploc);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());
    mPixelLocations.insert(mPixelLocations.end(), pixel_locs.begin(), pixel_locs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    if (mResyncTimestamp)
    {
        mStartTimestamp_ns = timestamp_ns;
        mResyncTimestamp = false;
    }

    double time_sec = static_cast<double>(timestamp_ns - mStartTimestamp_ns) / 1'000'000'000.0;

    auto cloud_data = pointCloud.data();

    std::vector<position3>  vertices;
    std::vector<range_t>    ranges;
    std::vector<returns3>   returns;
    std::vector<uint16_t>   frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<uint16_t>  frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(point.frameID);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

void cPointCloud2Ply::onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<uint16_t>  frameIDs;
    std::vector<pixel_loc> pixel_locs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        position3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        frameIDs.push_back(point.frameID);

        pixel_loc ploc;
        ploc.chn = point.chnNum;
        ploc.pixel = point.pixelNum;
        pixel_locs.push_back(ploc);
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());
    mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());
    mPixelLocations.insert(mPixelLocations.end(), pixel_locs.begin(), pixel_locs.end());

    if (mIndividualPlyFiles)
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePointcloud(filename);
    }

    ++mFrameCount;
}

/*
 * Plot Info
 */
void cPointCloud2Ply::onBeginPlotInfoList() {}
void cPointCloud2Ply::onEndPlotInfoList() {}

void cPointCloud2Ply::onBeginPlotInfoBlock() {}
void cPointCloud2Ply::onEndPlotInfoBlock() {}

void cPointCloud2Ply::onPlotID(int id)
{
    mPlotId = id;
}

void cPointCloud2Ply::onSubPlotID(int id) 
{
    mSubPlotId = id;
}

void cPointCloud2Ply::onName(const std::string& name) {}
void cPointCloud2Ply::onDescription(const std::string& description) {}

void cPointCloud2Ply::onSpecies(const std::string& species) {}
void cPointCloud2Ply::onCultivar(const std::string& cultivar) {}

void cPointCloud2Ply::onBeginTreatmentList() {}
void cPointCloud2Ply::onEndTreatmentList() {}
void cPointCloud2Ply::onTreatment(const std::string& treatment) {}

void cPointCloud2Ply::onConstructName(const std::string& name) {}
void cPointCloud2Ply::onEvent(const std::string& event) {}

void cPointCloud2Ply::onPotLabel(const std::string& pot_label) {}
void cPointCloud2Ply::onSeedGeneration(const std::string& seed_generation) {}
void cPointCloud2Ply::onCopyNumber(const std::string& copy_number) {}
void cPointCloud2Ply::onLeafType(const std::string& leaf_type) {}

void cPointCloud2Ply::onPlotDimensions(double x_min_m, double x_max_m,
    double y_min_m, double y_max_m, double z_min_m, double z_max_m) {}

void cPointCloud2Ply::onPlotPointCloudData(cPlotPointCloud pointCloud)
{
    auto cloud_data = pointCloud.data();

    if (mGroundLevel_mm.has_value())
    {
        auto ground_level_mm = mGroundLevel_mm.value();

        for (auto& point : cloud_data)
        {
            point.z_mm -= ground_level_mm;
        }
    }

    std::vector<position3>  vertices;
    std::vector<range_t>    ranges;
    std::vector<returns3>   returns;
    std::vector<frameID_t>  frameIDs;
    std::vector<pixel_loc>  pixels;

    bool saveFrameIDs = pointCloud.hasFrameIDs();
    bool savePixels   = pointCloud.hasPixelInfo();

    for (const auto& point : cloud_data)
    {
        if ((point.x_mm == 0) && (point.y_mm == 0) && (point.z_mm == 0))
            continue;

        position3 xyz;
        xyz.x = point.x_mm * nConstants::MM_TO_M;
        xyz.y = point.y_mm * nConstants::MM_TO_M;
        xyz.z = point.z_mm * nConstants::MM_TO_M;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        returns3 data;
        data.a = point.nir;
        data.s = point.signal;
        data.r = point.reflectivity;
        returns.push_back(data);

        if (savePixels)
        {
            frameIDs.push_back(point.frameID);

            pixel_loc loc;
            loc.chn = point.chnNum;
            loc.pixel = point.pixelNum;

            pixels.push_back(loc);
        }
        else if (saveFrameIDs)
        {
            frameIDs.push_back(point.frameID);
        }
    }

    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    mRanges.insert(mRanges.end(), ranges.begin(), ranges.end());
    mReturns.insert(mReturns.end(), returns.begin(), returns.end());

    if (pixels.empty())
        mPixelLocations.clear();
    else
        mPixelLocations.insert(mPixelLocations.end(), pixels.begin(), pixels.end());


    if (frameIDs.empty())
        mFrameIDs.clear();
    else
        mFrameIDs.insert(mFrameIDs.end(), frameIDs.begin(), frameIDs.end());

    std::string pathname = mOutputPath.string();
    pathname += "_Plot";
    pathname += std::to_string(mPlotId);

    if (mSubPlotId > 0)
    {
        pathname += ".";
        pathname += std::to_string(mSubPlotId);
    }

    std::filesystem::path filename = pathname;
    filename.replace_extension("ply");

    writePointcloud(filename);
}

/*
 * Spidercam Info
 */
void cPointCloud2Ply::onPosition(spidercam::sPosition_1_t pos)
{
    mResyncTimestamp = true;

    if (mSaveDollyPositions)
    {
        position4 xyz;
        xyz.x = pos.X_mm / 1000.0;
        xyz.y = pos.Y_mm / 1000.0;
        xyz.z = pos.Z_mm / 1000.0;
        xyz.s = pos.speed_mmps / 1000.0;

        mPositions.push_back(xyz);
        mColors.push_back(mColor);
    }
}

void cPointCloud2Ply::writePosition(std::filesystem::path filename)
{
    using namespace tinyply;

    std::filebuf file_buffer;

    if (mUseBinaryFormat)
    {
        file_buffer.open(filename, std::ios::out | std::ios::binary);
    }
    else
    {
        file_buffer.open(filename, std::ios::out);
    }

    if (!file_buffer.is_open())
        throw std::runtime_error("failed to open " + filename.string());

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z", "s" },
        Type::FLOAT32, mPositions.size(), reinterpret_cast<uint8_t*>(mPositions.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "red", "green", "blue" },
        Type::UINT8, mColors.size(), reinterpret_cast<uint8_t*>(mColors.data()), Type::INVALID, 0);

    if (mUseBinaryFormat)
    {
        std::ostream outstream_binary(&file_buffer);
        if (outstream_binary.fail())
            throw std::runtime_error("failed to open " + filename.string());

        // Write a binary file
        ply_file.write(outstream_binary, true);
    }
    else
    {
        std::ostream outstream_ascii(&file_buffer);
        if (outstream_ascii.fail())
            throw std::runtime_error("failed to open " + filename.string());

        // Write an ASCII file
        ply_file.write(outstream_ascii, false);
    }

    mPositions.clear();
}

void cPointCloud2Ply::writePointcloud(std::filesystem::path filename)
{
    using namespace tinyply;

    if (mResetOrigin)
    {
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();

        for (const auto& vertex : mVertices)
        {
            if (vertex.x < minX)
                minX = vertex.x;

            if (vertex.y < minY)
                minY = vertex.y;
        }

        for (auto& vertex : mVertices)
        {
            vertex.x -= minX;
            vertex.y -= minY;
        }
    }

    std::filebuf file_buffer;

    if (mUseBinaryFormat)
    {
        file_buffer.open(filename, std::ios::out | std::ios::binary);
    }
    else
    {
        file_buffer.open(filename, std::ios::out);
    }

    if (!file_buffer.is_open())
        throw std::runtime_error("failed to open " + filename.string());

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
        Type::FLOAT32, mVertices.size(), reinterpret_cast<uint8_t*>(mVertices.data()), Type::INVALID, 0);

#ifdef USE_FLOATS

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::FLOAT32, mRanges.size(), reinterpret_cast<uint8_t*>(mRanges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::FLOAT32, mReturns.size(), reinterpret_cast<uint8_t*>(mReturns.data()), Type::INVALID, 0);

#else

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::UINT32, mRanges.size(), reinterpret_cast<uint8_t*>(mRanges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::UINT16, mReturns.size(), reinterpret_cast<uint8_t*>(mReturns.data()), Type::INVALID, 0);

#endif

    if (!mFrameIDs.empty())
    {
#ifdef USE_FLOATS
        ply_file.add_properties_to_element("vertex", { "frame_id" },
            Type::FLOAT32, mFrameIDs.size(), reinterpret_cast<uint8_t*>(mFrameIDs.data()), Type::INVALID, 0);
#else
        ply_file.add_properties_to_element("vertex", { "frame_id" },
            Type::UINT16, mFrameIDs.size(), reinterpret_cast<uint8_t*>(mFrameIDs.data()), Type::INVALID, 0);
#endif
    }

    if (!mPixelLocations.empty())
    {
#ifdef USE_FLOATS
        ply_file.add_properties_to_element("vertex", { "channel", "pixel" },
            Type::FLOAT32, mPixelLocations.size(), reinterpret_cast<uint8_t*>(mPixelLocations.data()), Type::INVALID, 0);
#else
        ply_file.add_properties_to_element("vertex", { "channel", "pixel" },
            Type::UINT16, mPixelLocations.size(), reinterpret_cast<uint8_t*>(mPixelLocations.data()), Type::INVALID, 0);
#endif
    }


    if (mUseBinaryFormat)
    {
        std::ostream outstream_binary(&file_buffer);
        if (outstream_binary.fail())
            throw std::runtime_error("failed to open " + filename.string());

        // Write a binary file
        ply_file.write(outstream_binary, true);
    }
    else
    {
        std::ostream outstream_ascii(&file_buffer);
        if (outstream_ascii.fail())
            throw std::runtime_error("failed to open " + filename.string());

        // Write an ASCII file
        ply_file.write(outstream_ascii, false);
    }

    mVertices.clear();
    mRanges.clear();
    mReturns.clear();
    mFrameIDs.clear();
    mPixelLocations.clear();
}

