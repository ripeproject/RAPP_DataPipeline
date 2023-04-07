
#include "ldOusterBlockId.hpp"

#include "ldClassIdentifiers.hpp"


lidar_data::cOusterLidarID::cOusterLidarID(uint8_t majorVer, uint8_t minorVer)
:
    cBlockID(ClassIDs::OUSTER_LIDAR, majorVer, minorVer)
{}

void lidar_data::cOusterLidarID::dataID(ouster::DataID id) noexcept
{
    cBlockID::dataID(static_cast<uint16_t>(id));
}


