
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

	bool savePlotsInSingleFile() const;
	bool savePlysFiles() const;
	bool plysUseBinaryFormat() const;

	bool saveFrameIds() const;
	bool savePixelInfo() const;

	bool hasPlotInfo(const std::string& scan_name) const;

	std::shared_ptr<cPlotBoundaries> getPlotInfo(const std::string& scan_name) const;

private:
	std::string mConfigFilename;

	bool mSavePlotsInSingleFile = false;
	bool mSavePlyFiles = false;
	bool mPlyUseBinaryFormat = false;

	bool mSaveFrameIds = false;
	bool mSavePixelInfo = false;

	std::map<std::string, std::shared_ptr<cPlotBoundaries>> mPlots;
};