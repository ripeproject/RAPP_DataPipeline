
#pragma once

#include "PlotBoundaries.hpp"

#include <string>
#include <map>
#include <memory>

// Forward declarations


class cConfigFileData
{
public:
	explicit cConfigFileData(const std::string& filename);
	~cConfigFileData();

	/**
	 * Load the plot information from the configuration file.
	 */
	bool load();

	bool empty() const;

private:
	std::string mConfigFilename;


	std::map<std::string, cPlotBoundaries> mPlots;
};