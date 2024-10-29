
#pragma once

#include "PlotMetaData.hpp"

#include <cbdf/ExperimentInfo.hpp>

#include <cstdint>
#include <vector>
#include <map>


class cPlotData
{
public:
	cPlotData() = default;
	~cPlotData();

	void setExperimentInfo(const cExperimentInfo& info);

	void addPlotInfo(const cPlotMetaData& info);
	void addPlotData(int id, int doy, double height_mm, double lowerBound_mm, double upperBount_mm);

	void write(const std::string& directory);

protected:
	void splitIntoGroups(cPlotMetaData* pPlotInfo);
	bool group(cPlotMetaData* pPlotInfo1, cPlotMetaData* pPlotInfo2);

private:
	cExperimentMetaData mExperimentInfo;

	std::vector<cPlotMetaData*> mpPlots;

	// Note: mGroups does not own the pointer
	std::vector<std::vector<cPlotMetaData*>> mGroups;

	struct sPlotHeightData_t
	{
		int doy = 0;
		double height_mm = 0;
		double lowerBound_mm = 0;
		double upperBount_mm = 0;
	};

	std::map<int, std::vector<sPlotHeightData_t> > mPlotHeights;
};

