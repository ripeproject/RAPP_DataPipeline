
#include "OusterBlockId.hpp"

#include "ClassIdentifiers.hpp"


v1::cOusterLidarID::cOusterLidarID(uint8_t majorVer, uint8_t minorVer)
:
    cBlockID(ClassIDs::OUSTER_LIDAR, majorVer, minorVer)
{}

void v1::cOusterLidarID::dataID(ouster::DataID id) noexcept
{
    cBlockID::dataID(static_cast<uint16_t>(id));
}


