#pragma once

#include <string>
#include <vector>
#include <ctime>


namespace nStringUtils
{
	// trim from start (in place)
	void ltrim(std::string& s);

	// trim from end (in place)
	void rtrim(std::string& s);

	// trim from both ends (in place)
	inline void trim(std::string& s) 
	{
		rtrim(s);
		ltrim(s);
	}

	/**
	 * Case insensitive compare of two strings
	 */
	bool iequal(const std::string& lhs, const char* const rhs);

	/**
	 * Test is the string (sub_str) is in the string in.
	 */
	bool icontains(const std::string& in, const char* const sub_str);

	/**
	 * Extract a list of parameters, comma separated, contained
	 * between curly brackets {}
	 */
	std::vector<std::string> get_parameters(const std::string& str);

	/**
	 * Add a measurement timestamp to a filename
	 *
	 * The filename will become: filename_YYYYmmddHHMM.extension
	 */
	std::string addMeasurementTimestamp(std::string filename);
	std::string addMeasurementTimestamp(std::string filename, std::time_t time);
	std::string addMeasurementTimestamp(std::string filename, std::tm* time);

	/**
	 * Extract the measurement timestamp from a filename
	 *
	 * The filename filename_YYYYmmddHHMM.extension results in YYYYmmddHHMM
	 */
	std::tm extractMeasurementTimestamp(std::string filename);
	void extractMeasurementTimestamp(std::string filename, std::string& timestamp);

	/**
	 * Add a process timestamp to a filename
	 *
	 * The filename will become: filename.YYYYmmddHHMM.extension
	 */
	std::string addProcessedTimestamp(std::string filename);
	std::string addProcessedTimestamp(std::string filename, std::time_t time);
	std::string addProcessedTimestamp(std::string filename, std::tm* time);


	struct sFilenameAndExtension
	{
		std::string filename;
		std::string extension;
	};

	sFilenameAndExtension removeMeasurementTimestamp(const std::string& filename);

	sFilenameAndExtension removeProcessedTimestamp(const std::string& filename);

	sFilenameAndExtension splitFilename(const std::string& filename);


	struct sPathAndFilename
	{
		std::string path;
		std::string filename;
	};

	sPathAndFilename splitPathname(const std::string& pathname);

	/**
	 * Compacts a filename to a maximum length
	 *
	 * The filename: d:\test\long_file_name.extension -> d:\...\long_file_name
	 */
	std::string safeFilename(std::string filename);

	/**
	 * Compacts a filename to a maximum length
	 *
	 * The filename: d:\test\long_file_name.extension -> d:\...\long_file_name
	 */
	std::string compactFilename(const std::string& filename, std::size_t max_len);

	/**
	 * Checks to see if a directory is a standard month name used in the CERES framework
	 */
	bool isMonthDirectory(const std::string& dir);

	enum class eMonth { Jan = 1, Feb = 2, Mar = 3, Apr = 4, May = 5, June = 6, July = 7, Aug = 8, Sept = 9, Oct = 10, Nov = 11, Dec = 12, Unknown = 13 };
	eMonth toMonth(const std::string& dir);

	int toDay(const std::string& dir);

	/**
	 * Make a temporary filename from a filename
	 *
	 * The filename: d:\test\long_file_name.extension -> d:\test\~long_file_name.extension
	 */
	std::string make_temp_filename(const std::string& filename);

	/**
	 * Checks to see if a filename is a temporary filename (starts with '~')
	 *
	 * The filename: d:\test\file_name.extension
	 *					and d:\test\~file_name.extension does not exist ->  return false
	 * 
	 *				 d:\test\file_name.extension
	 *					and d:\test\~file_name.extension does exist ->  return true
	 *
	 *				 d:\test\~long_file_name.extension -> return true;
	 */
	bool has_temp_filename(const std::string& filename);

	/**
	 * Checks to see if a filename is a temporary filename (starts with '~')
	 *
	 * The filename: d:\test\file_name.extension ->  return false
	 *				 d:\test\~file_name.extension -> return true;
	 */
	bool is_temp_filename(const std::string& filename);

	/**
	 * Convert temporary filename to filename
	 *
	 * The filename: d:\test\~long_file_name.extension -> d:\test\long_file_name.extension
	 */
	std::string convert_temp_filename(const std::string& filename);

} // End of Namespace nStringUtils