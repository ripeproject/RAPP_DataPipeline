
#include "PlotInfoBlockId.hpp"

#include "ProcessingClassIdentifiers.hpp"

#include <cstdint>

cPlotInfoID::cPlotInfoID(uint8_t majorVer, uint8_t minorVer)
:
    cBlockID(static_cast<BLOCK_CLASS_ID_t>(ProcessingClassIDs::PLOT_INFO), majorVer, minorVer)
{}

void cPlotInfoID::dataID(plot_info::DataID id) noexcept
{
    cBlockID::dataID(static_cast<uint16_t>(id));
}


