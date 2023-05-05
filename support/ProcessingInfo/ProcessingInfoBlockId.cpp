
#include "ProcessingInfoBlockId.hpp"

#include "ProcessingClassIdentifiers.hpp"

#include <cstdint>

cProcessingInfoID::cProcessingInfoID(uint8_t majorVer, uint8_t minorVer)
:
    cBlockID(static_cast<BLOCK_CLASS_ID_t>(ProcessingClassIDs::PROCESSING_INFO), majorVer, minorVer)
{}

void cProcessingInfoID::dataID(processing_info::DataID id) noexcept
{
    cBlockID::dataID(static_cast<uint16_t>(id));
}


