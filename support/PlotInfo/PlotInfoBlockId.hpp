
#pragma once

#include <cbdf/BlockId.hpp>

// Forward Declares
namespace plot_info
{
    enum class DataID : uint16_t;
}

class cPlotInfoID : public cBlockID
{
public:
    explicit cPlotInfoID(uint8_t majorVer = 1, uint8_t minorVer = 0);

    void dataID(plot_info::DataID id) noexcept;

};


