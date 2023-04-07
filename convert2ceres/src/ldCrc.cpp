
#include "ldCrc.hpp"

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cerrno>
#include <cstring>


namespace
{
    uint32_t to_uint32(const lidar_data::cBlockID& blockID)
    {
        uint32_t id = static_cast<uint32_t>(blockID.classID()) << 16;
        id |= static_cast<uint16_t>(blockID.majorVersion()) << 8;
        id |= blockID.minorVersion();

        return id;
    }

    /* Table of CRCs of all 8-bit messages. */
    uint32_t crc_table[256];

    /* Flag: has the table been computed? Initially false. */
    bool crc_table_computed = false;

    /* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below). */

    uint32_t update_crc(uint32_t crc, const std::byte* buf, std::size_t len)
    {
        std::uint32_t c = crc;
        for (int n = 0; n < len; ++n)
        {
            c = crc_table[(c ^ static_cast<const unsigned char>(buf[n])) & 0xff] ^ (c >> 8);
        }

        return c;
    }
}

/* Make the table for a fast CRC. */
void lidar_data::make_crc_table()
{
    if (crc_table_computed) return;

    for (int n = 0; n < 256; ++n)
    {
        std::uint32_t c = n;
        for (int k = 0; k < 8; ++k)
        {
            if (c & 1)
                c = 0xEDB88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }

    crc_table_computed = true;
}

/* Return the CRC of the block id and block bytes buf[0..len-1]. */
uint32_t lidar_data::crc(const lidar_data::cBlockID& blockID, const std::byte* buf, std::size_t len)
{
    uint32_t id = to_uint32(blockID);
    auto c = update_crc(0xffffffffUL, reinterpret_cast<const std::byte*>(&id), sizeof(id));
    auto dataID = blockID.dataID();
    c = update_crc(c, reinterpret_cast<const std::byte*>(&dataID), sizeof(dataID)); // ^ 0xffffffffUL;
    return update_crc(c, buf, len) ^ 0xffffffffUL;
}

/* Return the CRC of the block id.  Used in zero length data blocks */
uint32_t lidar_data::crc(const lidar_data::cBlockID& blockID)
{
    uint32_t id = to_uint32(blockID);
    auto c = update_crc(0xffffffffUL, reinterpret_cast<const std::byte*>(&id), sizeof(id));
    auto dataID = blockID.dataID();
    return update_crc(c, reinterpret_cast<const std::byte*>(&dataID), sizeof(dataID)) ^ 0xffffffffUL;
}

