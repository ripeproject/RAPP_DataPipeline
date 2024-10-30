#pragma once

#include <string>
#include <ctime>


namespace nDateUtils
{
	/*
	* Converts month, day, and year strings to time_t
	*
	* Month is a string representing an integer in the range 1 to 12
	* Day is a string representing an integer in the range 1 to 31 (depends on month)
	* Year is a string representing an integer greater than 1900
	*/
	std::time_t to_time_t(const std::string& month, const std::string& day, const std::string& year);

	/*
	* Converts month, day, and year integers to time_t
	* 
	* Month is an integer in the range 1 to 12
	* Day is an integer in the range 1 to 31 (depends on month)
	* Year is an integer greater than 1900 
	*/
	std::time_t to_time_t(int month, int day, int year);

	std::uint16_t to_day_of_year(std::time_t date);

	std::uint16_t to_day_of_year(const std::string& month, const std::string& day, const std::string& year);
	std::uint16_t to_day_of_year(int month, int day, int year);

}

