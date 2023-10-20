
#include "PvtBlockId.hpp"

#include "ClassIdentifiers.hpp"


v1::cPvtID::cPvtID(uint8_t majorVer, uint8_t minorVer)
:
    cBlockID(ClassIDs::PVT, majorVer, minorVer)
{}

void v1::cPvtID::dataID(pvt::DataID id) noexcept
{
    cBlockID::dataID(static_cast<uint16_t>(id));
}


