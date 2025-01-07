
#include "ExportUtils.hpp"

#include "datatypes.hpp"
#include "Constants.hpp"
#include "PointCloudUtils.hpp"

#include <tinyply.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#define USE_FLOATS

using namespace pointcloud;


extern void update_progress(const int id, const int progress_pct);

namespace
{
#ifdef USE_FLOATS
    typedef float range_t;
    struct returns3 { float s, r, a; };

    typedef float frameID_t;
    struct pixel_loc { float chn, pixel; };
#else
    typedef uint32_t range_t;
    struct returns3 { uint16_t s, r, a; };

    typedef uint16_t frameID_t;
    struct pixel_loc { uint16_t chn, pixel; };
#endif

    struct position3 { float x, y, z; };
    struct position4 { float x, y, z, s; };
    struct color3 { uint8_t r, g, b; };
}
// end of namespace


void exportPointcloud2Ply(int id, const std::string& filename, const cRappPointCloud& pc, bool useBinaryFormat,
    double max_angle, double threshold_pct)
{
    using namespace tinyply;

    auto cloud_data = pc.data();

    shiftPointCloudToAGL(id, cloud_data);

    sPitchAndRoll_t result = computePcToGroundMeshRotationUsingGrid_deg(cloud_data, 
        pc.minX_mm(), pc.maxX_mm(), pc.minY_mm(), pc.maxY_mm(), threshold_pct);

    if (result.valid && (std::abs(result.pitch_deg) < max_angle) && (std::abs(result.roll_deg) < max_angle))
    {
        rotate(cloud_data, 0.0, result.pitch_deg, result.roll_deg);
    }

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;
    std::vector<frameID_t> frameIDs;
    std::vector<pixel_loc> pixels;

    bool saveFrameIDs = pc.hasFrameIDs();
    bool savePixels   = pc.hasPixelInfo();

    int i = 0;
    auto n = 3 * cloud_data.size() / 2;

    for (const auto& point : cloud_data)
    {
        update_progress(id, static_cast<int>((100.0 * i++) / n));

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
            loc.chn   = point.chnNum;
            loc.pixel = point.pixelNum;

            pixels.push_back(loc);
        }
        else if (saveFrameIDs)
        {
            frameIDs.push_back(point.frameID);
        }
    }

    std::filebuf file_buffer;

    if (useBinaryFormat)
    {
        file_buffer.open(filename, std::ios::out | std::ios::binary);
    }
    else
    {
        file_buffer.open(filename, std::ios::out);
    }

    if (!file_buffer.is_open())
        throw std::runtime_error("failed to open " + filename);

    PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
        Type::FLOAT32, vertices.size(), reinterpret_cast<uint8_t*>(vertices.data()), Type::INVALID, 0);

#ifdef USE_FLOATS

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::FLOAT32, ranges.size(), reinterpret_cast<uint8_t*>(ranges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::FLOAT32, returns.size(), reinterpret_cast<uint8_t*>(returns.data()), Type::INVALID, 0);

    if (!frameIDs.empty())
    {
        ply_file.add_properties_to_element("vertex", { "frame_id" },
            Type::FLOAT32, frameIDs.size(), reinterpret_cast<uint8_t*>(frameIDs.data()), Type::INVALID, 0);
    }

    if (!pixels.empty())
    {
        ply_file.add_properties_to_element("vertex", { "chn_num", "pixel_num"},
            Type::FLOAT32, pixels.size(), reinterpret_cast<uint8_t*>(pixels.data()), Type::INVALID, 0);
    }

#else

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::UINT32, ranges.size(), reinterpret_cast<uint8_t*>(ranges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::UINT16, returns.size(), reinterpret_cast<uint8_t*>(returns.data()), Type::INVALID, 0);

    if (!frameIDs.empty())
    {
        ply_file.add_properties_to_element("vertex", { "frame_id" },
            Type::UINT16, frameIDs.size(), reinterpret_cast<uint8_t*>(frameIDs.data()), Type::INVALID, 0);
    }

    if (!pixels.empty())
    {
        ply_file.add_properties_to_element("vertex", { "chn_num", "pixel_num" },
            Type::UINT16, pixels.size(), reinterpret_cast<uint8_t*>(pixels.data()), Type::INVALID, 0);
    }

#endif

    if (useBinaryFormat)
    {
        std::ostream outstream_binary(&file_buffer);
        if (outstream_binary.fail())
            throw std::runtime_error("failed to open " + filename);

        // Write a binary file
        ply_file.write(outstream_binary, true);
    }
    else
    {
        std::ostream outstream_ascii(&file_buffer);
        if (outstream_ascii.fail())
            throw std::runtime_error("failed to open " + filename);

        // Write an ASCII file
        ply_file.write(outstream_ascii, false);
    }

    update_progress(id, 100);
}

