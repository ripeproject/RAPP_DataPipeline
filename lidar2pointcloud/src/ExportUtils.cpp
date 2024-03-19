
#include "ExportUtils.hpp"

#include "datatypes.hpp"
#include "Constants.hpp"
#include "FieldUtils.hpp"

#include <tinyply.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

//#define USE_FLOATS

namespace
{
#ifdef USE_FLOATS
    typedef float range_t;
    struct returns3 { float s, r, a; };
    struct beam_loc { float theta, phi; };
    struct pixel_loc { float chn, pixel; };
#else
    typedef uint32_t range_t;
    struct returns3 { uint16_t s, r, a; };
    struct beam_loc { double theta, phi; };
    struct pixel_loc { uint16_t chn, pixel; };
#endif

    struct position3 { float x, y, z; };
    struct position4 { float x, y, z, s; };
    struct color3 { uint8_t r, g, b; };
}
// end of namespace


void exportPointcloud2Ply(const std::string& filename, const cRappPointCloud& pc, bool useBinaryFormat)
{
    using namespace tinyply;

    auto cloud_data = pc.data();

    shiftPointCloudToAGL(cloud_data);

    std::vector<position3> vertices;
    std::vector<range_t>   ranges;
    std::vector<returns3>  returns;

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

#else

    ply_file.add_properties_to_element("vertex", { "range_mm" },
        Type::UINT32, ranges.size(), reinterpret_cast<uint8_t*>(ranges.data()), Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "intensity", "reflectivity", "ambient_noise" },
        Type::UINT16, returns.size(), reinterpret_cast<uint8_t*>(returns.data()), Type::INVALID, 0);

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
}

