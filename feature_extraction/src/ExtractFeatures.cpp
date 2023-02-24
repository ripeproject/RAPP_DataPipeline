
#include "ExtractFeatures.hpp"
#include "PointCloudTypes.hpp"

#include "nanoflann.hpp"
#include "utils.h"

#include <tinyply.h>

#include <iostream>


//#define USE_BINARY

bool cExtractFeatures::mIndividualPlyFiles = false;


cExtractFeatures::cExtractFeatures() : cPointCloudParser()
{}

cExtractFeatures::~cExtractFeatures()
{
    if (!mVertices.empty())
    {
        std::filesystem::path filename = mOutputPath;

        std::string ext = std::to_string(mFrameCount);
        ext += ".ply";

        filename.replace_extension(ext);

        writePlyFile(filename);
    }
}

void cExtractFeatures::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cExtractFeatures::onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) {}
void cExtractFeatures::onImuData(pointcloud::imu_data_t data) {}

void cExtractFeatures::onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<float3>   vertices;
    std::vector<uint32_t> ranges;
    std::vector<uint3>    returns;
    std::vector<uint16_t> frameIDs;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        float3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        uint3 data;
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

        std::string ext;

        if (mIndividualPlyFiles)
        {
           ext = std::to_string(mFrameCount);
           ext += ".";
        }

        ext += "ply";

        filename.replace_extension(ext);

        writePlyFile(filename);
    }

    ++mFrameCount;
}

void cExtractFeatures::onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<float3>   vertices;
    std::vector<uint32_t> ranges;
    std::vector<uint3>    returns;
    std::vector<uint16_t> frameIDs;

    PointCloud<double> cloud;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        cloud.pts.push_back(PointCloud<double>::PtData(point.X_m, point.Y_m, point.Z_m));

        float3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        uint3 data;
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

    int k = 20;
    LineDetection3D detector;
    std::vector<PLANE> planes;
    std::vector<std::vector<cv::Point3d>> lines;
    detector.run(cloud, k, planes, lines);

    if (mIndividualPlyFiles)
    {
        // Write out the detected planes from the point clound
        {
            std::filesystem::path filename = mOutputPath;

            std::string ext = std::to_string(mFrameCount);
            ext += ".planes.ply";

            filename.replace_extension(ext);
            writePlaneFile(filename, planes, detector.mScale);
        }


        // Write out the detected lines from the point clound
        {
            std::filesystem::path filename = mOutputPath;

            std::string ext = std::to_string(mFrameCount);
            ext += ".lines.ply";

            filename.replace_extension(ext);
            writeLineFile(filename, lines, detector.mScale);
        }

        // Write out the main frame point clound
        {
            std::filesystem::path filename = mOutputPath;

            std::string ext = std::to_string(mFrameCount);
            ext += ".ply";

            filename.replace_extension(ext);

            writePlyFile(filename);
        }
    }

    ++mFrameCount;
}

void cExtractFeatures::onPointCloudData(cPointCloud pointCloud)
{
    auto cloud_data = pointCloud.data();

    std::vector<float3>   vertices;
    std::vector<uint32_t> ranges;
    std::vector<uint3>    returns;

    for (const auto& point : cloud_data)
    {
        if ((point.X_m == 0) && (point.Y_m == 0) && (point.Z_m == 0))
            continue;

        float3 xyz;
        xyz.x = point.X_m;
        xyz.y = point.Y_m;
        xyz.z = point.Z_m;

        vertices.push_back(xyz);

        ranges.push_back(point.range_mm);

        uint3 data;
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

        writePlyFile(filename);
    }

    ++mFrameCount;
}

void cExtractFeatures::writePlyFile(std::filesystem::path filename)
{
    using namespace tinyply;

#ifdef USE_BINARY
    std::filebuf binary_buffer;
    binary_buffer.open(filename, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&binary_buffer);
    if (outstream_binary.fail())
        throw std::runtime_error("failed to open " + filename.string());
#else
    std::filebuf fb_ascii;
    fb_ascii.open(filename, std::ios::out);
    std::ostream outstream_ascii(&fb_ascii);
    if (outstream_ascii.fail()) 
        throw std::runtime_error("failed to open " + filename.string());
#endif

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
        Type::FLOAT32, mVertices.size(), reinterpret_cast<uint8_t*>(mVertices.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::UINT32, mRanges.size(), reinterpret_cast<uint8_t*>(mRanges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::UINT16, mReturns.size(), reinterpret_cast<uint8_t*>(mReturns.data()), Type::INVALID, 0);

    if (!mFrameIDs.empty())
    {
        ply_file.add_properties_to_element("vertex", { "frame_id" },
            Type::UINT16, mFrameIDs.size(), reinterpret_cast<uint8_t*>(mFrameIDs.data()), Type::INVALID, 0);
    }

#ifdef USE_BINARY
    // Write a binary file
    ply_file.write(outstream_binary, true);
#else
    // Write an ASCII file
    ply_file.write(outstream_ascii, false);
#endif

    mVertices.clear();
    mRanges.clear();
    mReturns.clear();
    mFrameIDs.clear();
}

void cExtractFeatures::writePlaneFile(std::filesystem::path filename,
                                        const std::vector<PLANE>& planes, double scale)
{
    std::vector<float3> vertices;
    std::vector<color3> colors;
    std::vector<int>    indexs;

    for (int p = 0; p < planes.size(); ++p)
    {
        uint8_t R = static_cast<uint8_t>(rand() % 255);
        uint8_t G = static_cast<uint8_t>(rand() % 255);
        uint8_t B = static_cast<uint8_t>(rand() % 255);

        color3 color = { R, G, B };

        const auto& plane = planes[p];

        for (int i = 0; i < plane.lines3d.size(); ++i)
        {
            const auto& lines = plane.lines3d[i];

            for (int j = 0; j < lines.size(); ++j)
            {
                const auto& line = lines[j];

                cv::Point3d dev = line[1] - line[0];
                double L = sqrt(dev.x * dev.x + dev.y * dev.y + dev.z * dev.z);
                int k = L / (scale / 10);

                double x = line[0].x;
                double y = line[0].y;
                double z = line[0].z;

                double dx = dev.x / k;
                double dy = dev.y / k;
                double dz = dev.z / k;

                for (int j = 0; j < k; ++j)
                {
                    x += dx;
                    y += dy;
                    z += dz;

                    float3 xyz;
                    xyz.x = x;
                    xyz.y = y;
                    xyz.z = z;

                    vertices.push_back(xyz);
                    colors.push_back(color);
                    indexs.push_back(p);
                }
            }
        }
    }

    using namespace tinyply;

#ifdef USE_BINARY
    std::filebuf binary_buffer;
    binary_buffer.open(filename, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&binary_buffer);
    if (outstream_binary.fail())
        throw std::runtime_error("failed to open " + filename.string());
#else
    std::filebuf fb_ascii;
    fb_ascii.open(filename, std::ios::out);
    std::ostream outstream_ascii(&fb_ascii);
    if (outstream_ascii.fail())
        throw std::runtime_error("failed to open " + filename.string());
#endif

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
        Type::FLOAT32, vertices.size(), reinterpret_cast<uint8_t*>(vertices.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "red", "green", "blue" },
        Type::UINT8, colors.size(), reinterpret_cast<uint8_t*>(colors.data()), Type::INVALID, 0);

    if (!mFrameIDs.empty())
    {
        ply_file.add_properties_to_element("vertex", { "plane_id" },
            Type::INT32, indexs.size(), reinterpret_cast<uint8_t*>(indexs.data()), Type::INVALID, 0);
    }

#ifdef USE_BINARY
    // Write a binary file
    ply_file.write(outstream_binary, true);
#else
    // Write an ASCII file
    ply_file.write(outstream_ascii, false);
#endif

}

void cExtractFeatures::writeLineFile(std::filesystem::path filename,
    const std::vector<std::vector<cv::Point3d>>& lines, double scale)
{
    std::vector<float3> vertices;
    std::vector<color3> colors;
    std::vector<int>    indexes;

    for (int p = 0; p < lines.size(); ++p)
    {
        uint8_t R = static_cast<uint8_t>(rand() % 255);
        uint8_t G = static_cast<uint8_t>(rand() % 255);
        uint8_t B = static_cast<uint8_t>(rand() % 255);

        color3 color = { R, G, B };

        const auto& line = lines[p];

        cv::Point3d dev = line[1] - line[0];
        double L = sqrt(dev.x * dev.x + dev.y * dev.y + dev.z * dev.z);
        int k = static_cast<int>(L / (scale / 10.0));

        double x = line[0].x;
        double y = line[0].y;
        double z = line[0].z;

        double dx = dev.x / k;
        double dy = dev.y / k;
        double dz = dev.z / k;

        for (int j = 0; j < k; ++j)
        {
            x += dx;
            y += dy;
            z += dz;

            float3 xyz;
            xyz.x = x;
            xyz.y = y;
            xyz.z = z;

            vertices.push_back(xyz);
            colors.push_back(color);
            indexes.push_back(p);
        }
    }

    using namespace tinyply;

#ifdef USE_BINARY
    std::filebuf binary_buffer;
    binary_buffer.open(filename, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&binary_buffer);
    if (outstream_binary.fail())
        throw std::runtime_error("failed to open " + filename.string());
#else
    std::filebuf fb_ascii;
    fb_ascii.open(filename, std::ios::out);
    std::ostream outstream_ascii(&fb_ascii);
    if (outstream_ascii.fail())
        throw std::runtime_error("failed to open " + filename.string());
#endif

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
        Type::FLOAT32, vertices.size(), reinterpret_cast<uint8_t*>(vertices.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "red", "green", "blue" },
        Type::UINT8, colors.size(), reinterpret_cast<uint8_t*>(colors.data()), Type::INVALID, 0);

    if (!mFrameIDs.empty())
    {
        ply_file.add_properties_to_element("vertex", { "plane_id" },
            Type::INT32, indexes.size(), reinterpret_cast<uint8_t*>(indexes.data()), Type::INVALID, 0);
    }

#ifdef USE_BINARY
    // Write a binary file
    ply_file.write(outstream_binary, true);
#else
    // Write an ASCII file
    ply_file.write(outstream_ascii, false);
#endif

}

