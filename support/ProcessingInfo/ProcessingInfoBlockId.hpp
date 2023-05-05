
#pragma once

#include <cbdf/BlockId.hpp>

// Forward Declares
namespace processing_info
{
    enum class DataID : uint16_t;
}

class cProcessingInfoID : public cBlockID
{
public:
    explicit cProcessingInfoID(uint8_t majorVer = 0, uint8_t minorVer = 0);

    void dataID(processing_info::DataID id) noexcept;

};


