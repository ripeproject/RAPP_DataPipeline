
#pragma once

#include <cstdint>
#include <string>

namespace processing_info
{
    enum class ePROCESSING_TYPE : uint16_t
    {
        UNKNOWN,
        POINT_CLOUD,
    };

    struct sProcessInfo_1_t
    {
        std::string		 name;
        ePROCESSING_TYPE type = ePROCESSING_TYPE::UNKNOWN;
        std::uint16_t	 year = 0;
        std::uint8_t	 month = 0;
        std::uint8_t	 day = 0;
        std::uint8_t	 hour = 0;
        std::uint8_t	 minute = 0;
        std::uint8_t	 seconds = 0;
    };
}


