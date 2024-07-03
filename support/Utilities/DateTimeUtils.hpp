#pragma once

#include <string>
#include <ctime>


namespace nDateUtils
{
    std::time_t to_time_t(const std::string& month, const std::string& day, const std::string& year);

	std::uint16_t to_day_of_year(std::time_t date);

	std::uint16_t to_day_of_year(const std::string& month, const std::string& day, const std::string& year);
	
}

