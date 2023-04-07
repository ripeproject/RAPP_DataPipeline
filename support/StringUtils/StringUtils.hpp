#pragma once

#include <string>
#include <vector>


namespace nStringUtils
{
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

	/**
	 * Add a process timestamp to a filename
	 *
	 * The filename will become: filename.YYYYmmddHHMM.extension
	 */
	std::string addProcessedTimestamp(std::string filename);


	struct sFilenameAndExtension
	{
		std::string filename;
		std::string extension;
	};

	sFilenameAndExtension removeMeasurementTimestamp(const std::string& filename);

	sFilenameAndExtension removeProcessedTimestamp(const std::string& filename);

} // End of Namespace nStringUtils