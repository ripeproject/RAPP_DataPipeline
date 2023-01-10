
#include "PointCloudBlockId.hpp"

#include "ProcessingClassIdentifiers.hpp"

#include <cstdint>

cPointCloudID::cPointCloudID(uint8_t majorVer, uint8_t minorVer)
:
    cBlockID(static_cast<BLOCK_CLASS_ID_t>(ProcessingClassIDs::POINT_CLOUD), majorVer, minorVer)
{}

void cPointCloudID::dataID(pointcloud::DataID id) noexcept
{
    cBlockID::dataID(static_cast<uint16_t>(id));
}


