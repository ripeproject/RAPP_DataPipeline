
#pragma once

#include "BlockId.hpp"

namespace v1
{
	// Forward Declares
	namespace pvt
	{
		enum class DataID : uint16_t;

		enum class ePOSTION_UNITS : uint8_t
		{
			UNKNOWN,
			INCHES,
			FEET,
			MILLIMETERS,
			CENTIMETERS,
			METERS
		};

		enum class eVELOCITY_UNITS : uint8_t
		{
			UNKNOWN,
			INCHES_PER_SEC,
			FEET_PER_SEC,
			MILLIMETERS_PER_SEC,
			CENTIMETERS_PER_SEC,
			METERS_PER_SEC
		};

		enum class eTIME_UNITS : uint8_t
		{
			UNKNOWN,
			NANOSECONDS,
			MICROSECONDS,
			MILLISECONDS,
			SECONDS
		};
	}

	class cPvtID : public cBlockID
	{
	public:
		explicit cPvtID(uint8_t majorVer = 0, uint8_t minorVer = 0);

		void dataID(pvt::DataID id) noexcept;

	};
}

