
#include "DateTimeUtils.hpp"


using namespace std;



std::time_t nDateUtils::to_time_t(const std::string& month, const std::string& day, const std::string& year)
{
	if (month.empty() || day.empty() || year.empty())
		return { 0 };

	int mm = std::stoi(month, nullptr, 10);
	int dd = std::stoi(day, nullptr, 10);
	int yy = std::stoi(year, nullptr, 10);

	if (yy < 100) yy += 2000;

	struct tm date = { 0 };
	date.tm_year = yy - 1900;
	date.tm_mon = mm - 1;
	date.tm_mday = dd;
	date.tm_isdst = -1;

	return mktime(&date);
}

std::time_t nDateUtils::to_time_t(int month, int day, int year)
{
	if ((month < 1) || (month > 12))
		return { 0 };
	if ((day < 1) || (day > 31))
		return { 0 };
	if (year < 1900)
		return { 0 };

	struct tm date = { 0 };
	date.tm_year = year - 1900;
	date.tm_mon = month - 1;
	date.tm_mday = day;
	date.tm_isdst = -1;

	return mktime(&date);
}

std::uint16_t nDateUtils::to_day_of_year(std::time_t date)
{
    auto tm = localtime(&date);
	return tm->tm_yday;
}

std::uint16_t nDateUtils::to_day_of_year(const std::string& month, const std::string& day, const std::string& year)
{
	std::time_t date = to_time_t(month, day, year);
	return to_day_of_year(date);
}

std::uint16_t nDateUtils::to_day_of_year(int month, int day, int year)
{
	std::time_t date = to_time_t(month, day, year);
	return to_day_of_year(date);
}
