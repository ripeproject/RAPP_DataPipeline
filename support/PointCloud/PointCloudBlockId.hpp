
#pragma once

#include <cbdf/BlockId.hpp>

// Forward Declares
namespace pointcloud
{
    enum class DataID : uint16_t;
}

class cPointCloudID : public cBlockID
{
public:
    explicit cPointCloudID(uint8_t majorVer = 0, uint8_t minorVer = 0);

    void dataID(pointcloud::DataID id) noexcept;

};


