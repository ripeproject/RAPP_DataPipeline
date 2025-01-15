
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotConfigOptions
{
public:
	cPlotConfigOptions();
	~cPlotConfigOptions() = default;

	void clear();

	bool isDirty() const;

	bool getSavePlotsInSingleFile() const;
	bool getSaveFrameIds() const;
	bool getSavePixelInfo() const;

	bool getSavePlyFiles() const;
	bool getPlysUseBinaryFormat() const;

	void setSavePlotsInSingleFile(bool option);
	void setSaveFrameIds(bool option);
	void setSavePixelInfo(bool option);

	void setSavePlyFiles(bool option);
	void setPlysUseBinaryFormat(bool option);

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	bool mSavePlotsInSingleFile = true;
	bool mSavePlyFiles = false;
	bool mPlysUseBinaryFormat = true;
	bool mSaveFrameIds = false;
	bool mSavePixelInfo = false;

	friend class cPlotConfigFile;
};


