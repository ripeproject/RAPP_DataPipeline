#pragma once

#include <cstdint>


namespace nExpTypes
{
	struct sDate_t
	{
		std::uint16_t year = 0;
		std::uint8_t month = 0;
		std::uint8_t day = 0;
	};

	struct sTime_t
	{
		std::uint8_t hour = 0;
		std::uint8_t minute = 0;
		std::uint8_t seconds = 0;
	};

	struct sDateDoy_t
	{
		std::uint16_t year = 0;
		std::uint8_t month = 0;
		std::uint8_t day = 0;
		std::uint16_t doy = 0;
	};

	struct sExperimentDateTime_t
	{
		std::uint16_t year = 0;
		std::uint8_t month = 0;
		std::uint8_t day = 0;
		std::uint8_t hour = 0;
		std::uint8_t minute = 0;
		std::uint8_t seconds = 0;
	};
}

