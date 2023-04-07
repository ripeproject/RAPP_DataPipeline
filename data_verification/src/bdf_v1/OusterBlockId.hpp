
#pragma once

#include "BlockId.hpp"

namespace v1
{
    // Forward Declares
    namespace ouster
    {
        enum class DataID : uint16_t;
    }

    class cOusterLidarID : public cBlockID
    {
    public:
        explicit cOusterLidarID(uint8_t majorVer = 0, uint8_t minorVer = 0);

        void dataID(ouster::DataID id) noexcept;
    };

}

