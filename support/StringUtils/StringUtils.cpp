
#include "StringUtils.hpp"

#include <ctime>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <cctype>


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
	char timestamp[100] = { '\0' };

	std::time_t t = std::time(nullptr);
	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", std::localtime(&t));

	filename += "_";
	filename += timestamp;

	return filename;
}

std::string nStringUtils::addProcessedTimestamp(std::string filename)
{
	char timestamp[100] = { '\0' };

	std::time_t t = std::time(nullptr);
	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M", std::localtime(&t));

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
	if (dir.starts_with( "Jan")) return true;
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

