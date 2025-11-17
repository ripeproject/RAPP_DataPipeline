
#include "StringUtils.hpp"

#include <ctime>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <locale>

// trim from start (in place)
void nStringUtils::ltrim(std::string& s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
void nStringUtils::rtrim(std::string& s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

bool nStringUtils::iequal(const std::string& lhs, const char* const rhs)
{
	if (lhs.size() == strlen(rhs))
	{
		auto n = lhs.size();
		for (size_t i = 0; i < n; ++i)
		{
			if (std::tolower(lhs[i]) != std::tolower(rhs[i]))
				return false;
		}

		return true;
	}

	return false;
}


bool nStringUtils::icontains(const std::string& in, const char* const sub_str)
{
	auto n = std::min(in.size(), strlen(sub_str));
	for (size_t i = 0; i < n; ++i)
	{
		if (std::tolower(in[i]) != std::tolower(sub_str[i]))
			return false;
	}

	return true;
}

std::vector<std::string> nStringUtils::get_parameters(const std::string& str)
{
	auto first = str.find_first_of('{');

	if (first == std::string::npos)
		return std::vector<std::string>();

	auto last = str.find_last_of('}');

	if (last == std::string::npos)
		return std::vector<std::string>();

	++first;
	std::string args = str.substr(first, last - first);

	std::vector<std::string> results;

	first = 0;

	for (size_t i = 0; i < args.size(); ++i)
	{
		if (args[i] == ',')
		{
			std::string parameter = args.substr(first, i - first);
			first = i + 1;

			results.push_back(parameter);
		}
	}

	std::string parameter = args.substr(first);
	results.push_back(parameter);

	return results;
}

std::string nStringUtils::addMeasurementTimestamp(std::string filename)
{
	return addMeasurementTimestamp(filename, std::time(nullptr));
}

std::string nStringUtils::addMeasurementTimestamp(std::string filename, std::time_t time)
{
	return addMeasurementTimestamp(filename, std::localtime(&time));
}

std::string nStringUtils::addMeasurementTimestamp(std::string filename, std::tm* time)
{
	if (!time)
		return filename;

	char timestamp[100] = { '\0' };

	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", time);

	filename += "_";
	filename += timestamp;

	return filename;
}

std::tm nStringUtils::extractMeasurementTimestamp(std::string filename)
{
	std::tm tm{};

	auto start_process_mark = filename.find("_p");
	if (start_process_mark != std::string::npos)
	{
		filename.erase(start_process_mark);
	}

	auto start_measure_mark = filename.find_last_of('_');
	if (start_measure_mark != std::string::npos)
	{
		++start_measure_mark;	// Skip the leading dash

		auto ext = filename.find_last_of('.');

		if (ext != std::string::npos)
			ext -= start_measure_mark;

		std::string time_mark = filename.substr(start_measure_mark, ext);

		std::string year = time_mark.substr(0, 4);
		std::string month = time_mark.substr(4, 2);
		std::string day = time_mark.substr(6, 2);

		std::string hour = time_mark.substr(8, 2);
		std::string minute = time_mark.substr(10, 2);
		std::string second = time_mark.substr(12, 2);

		tm.tm_year = std::stoi(year) - 1900;
		tm.tm_mon  = std::stoi(month) - 1;
		tm.tm_mday = std::stoi(day);

		tm.tm_hour = std::stoi(hour);
		tm.tm_min = std::stoi(minute);
		tm.tm_sec = std::stoi(second);
	}

	return tm;
}


void nStringUtils::extractMeasurementTimestamp(std::string filename, std::string& timestamp)
{
	timestamp.clear();

	auto start_process_mark = filename.find("_p");
	if (start_process_mark != std::string::npos)
	{
		filename.erase(start_process_mark);
	}

	auto start_measure_mark = filename.find_last_of('_');
	if (start_measure_mark != std::string::npos)
	{
		++start_measure_mark;	// Skip the leading dash

		auto ext = filename.find_last_of('.');

		if (ext != std::string::npos)
			ext -= start_measure_mark;

		timestamp = filename.substr(start_measure_mark, ext);
	}
}

std::string nStringUtils::addProcessedTimestamp(std::string filename)
{
	return addProcessedTimestamp(filename, std::time(nullptr));
}

std::string nStringUtils::addProcessedTimestamp(std::string filename, std::time_t time)
{
	return addProcessedTimestamp(filename, std::localtime(&time));
}

std::string nStringUtils::addProcessedTimestamp(std::string filename, std::tm* time)
{
	if (!time)
		return filename;

	char timestamp[100] = { '\0' };

	std::time_t t = std::time(nullptr);
	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M", time);

	filename += "_p";
	filename += timestamp;
	filename += "p";

	return filename;
}

nStringUtils::sFilenameAndExtension nStringUtils::removeMeasurementTimestamp(const std::string& filename)
{
	std::string base;
	std::string extension;

	auto dash = filename.find_last_of('_');
	if (dash != std::string::npos)
	{
		base = filename.substr(0, dash);
	}
	else
	{
		auto ext = filename.find_last_of('.');
		if (ext != std::string::npos)
		{
			base = filename.substr(0, ext);
			extension = filename.substr(ext + 1);
			return { base, extension };
		}

		base = filename;
		return { base, extension };
	}

	auto ext = filename.find_last_of('.');
	if (ext != std::string::npos)
	{
		extension = filename.substr(ext + 1);
	}

	return { base, extension };
}

nStringUtils::sFilenameAndExtension nStringUtils::removeProcessedTimestamp(const std::string& filename)
{
	std::string base;
	std::string extension;

	auto dash = filename.find("_p");
	if (dash != std::string::npos)
	{
		base = filename.substr(0, dash);
	}
	else
	{
		auto ext = filename.find_last_of('.');
		if (ext != std::string::npos)
		{
			base = filename.substr(0, ext);
			extension = filename.substr(ext + 1);
			return { base, extension };
		}

		base = filename;
		return { base, extension };
	}

	auto ext = filename.find("p.");
	if (ext != std::string::npos)
	{
		extension = filename.substr(ext + 1);
	}

	return { base, extension };
}

nStringUtils::sFilenameAndExtension nStringUtils::splitFilename(const std::string& filename)
{
	std::string base;
	std::string extension;

	auto pos = filename.rfind('.');

	if (pos == std::string::npos)
	{
		base = filename;
	}
	else
	{
		base = filename.substr(0, pos);
		extension = filename.substr(pos + 1);
	}

	return { base, extension };
}

nStringUtils::sPathAndFilename nStringUtils::splitPathname(const std::string& pathname)
{
	std::filesystem::path fp = pathname;

	std::string path = fp.parent_path().string();
	std::string filename = fp.filename().string();

	return { path, filename };
}


std::string nStringUtils::safeFilename(std::string filename)
{
	std::replace_if(filename.begin(), filename.end(),
		[](std::string::value_type c) 
		{
			switch (c)
			{
			case ':': return true;
			case '\\': return true;
			case '/': return true;
			case '<': return true;
			case '>': return true;
			case '|': return true;
			case '?': return true;
			case '*': return true;
			case '"': return true;
			}
			return std::isspace(c) != 0;
		}, '_');

	return filename;
}

std::string nStringUtils::compactFilename(const std::string& filename, std::size_t max_len)
{
	if (filename.length() < max_len)
		return filename;

	std::string result;

	std::filesystem::path full_path(filename);

	std::string fname = full_path.filename().string();
	if (fname.length() > max_len)
	{
		if (max_len < 5)
			return fname.substr(0, max_len);

		result = fname.substr(0, max_len-3);
		result += "...";
		return result;
	}

//	std::string path = full_path.parent_path().string();

	return fname;
}

bool nStringUtils::isMonthDirectory(const std::string& dir)
{
	if (dir.size() > 6) return false;

	if (dir.starts_with("Jan")) return true;
	if (dir.starts_with("Feb")) return true;
	if (dir.starts_with("Mar")) return true;
	if (dir.starts_with("Apr")) return true;
	if (dir.starts_with("May")) return true;
	if (dir.starts_with("June")) return true;
	if (dir.starts_with("July")) return true;
	if (dir.starts_with("Aug")) return true;
	if (dir.starts_with("Sept")) return true;
	if (dir.starts_with("Oct")) return true;
	if (dir.starts_with("Nov")) return true;
	if (dir.starts_with("Dec")) return true;

	return false;
}

nStringUtils::eMonth nStringUtils::toMonth(const std::string& dir)
{
	if (dir.starts_with("Jan")) return eMonth::Jan;
	if (dir.starts_with("Feb")) return eMonth::Feb;
	if (dir.starts_with("Mar")) return eMonth::Mar;
	if (dir.starts_with("Apr")) return eMonth::Apr;
	if (dir.starts_with("May")) return eMonth::May;
	if (dir.starts_with("June")) return eMonth::June;
	if (dir.starts_with("July")) return eMonth::July;
	if (dir.starts_with("Aug")) return eMonth::Aug;
	if (dir.starts_with("Sept")) return eMonth::Sept;
	if (dir.starts_with("Oct")) return eMonth::Oct;
	if (dir.starts_with("Nov")) return eMonth::Nov;
	if (dir.starts_with("Dec")) return eMonth::Dec;

	return eMonth::Unknown;
}


int nStringUtils::toDay(const std::string& dir)
{
	auto month = toMonth(dir);
	if (month == eMonth::Unknown)
		return -1;

	int i = 3;

	if ((month == eMonth::June) || (month == eMonth::July) || (month == eMonth::Sept))
		i = 4;

	return std::stoi(dir.substr(i));
}

std::string nStringUtils::make_temp_filename(const std::string& filename)
{
	std::filesystem::path tmp = filename;
	std::string tmp_file_name = "~" + tmp.filename().string();
	tmp.replace_filename(tmp_file_name);
	return tmp.string();
}

bool nStringUtils::has_temp_filename(const std::string& filename)
{
	if (filename.empty())
		return false;

	if (is_temp_filename(filename))
		return true;

	std::filesystem::path tmp = make_temp_filename(filename);

	return std::filesystem::exists(tmp);
}

bool nStringUtils::is_temp_filename(const std::string& filename)
{
	std::filesystem::path tmp = filename;
	std::string tmp_file_name = tmp.filename().string();
	if (tmp_file_name.empty())
		return false;

	return tmp_file_name[0] == '~';
}

std::string nStringUtils::convert_temp_filename(const std::string& filename)
{
	std::filesystem::path tmp = filename;
	std::string tmp_file_name = tmp.filename().string();

	if (!tmp_file_name.empty() && (tmp_file_name[0] == '~'))
	{
		tmp_file_name.erase(0, 1);
	}

	tmp.replace_filename(tmp_file_name);
	return tmp.string();

}

