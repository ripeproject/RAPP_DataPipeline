
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
	~cPlotData() = default;

	void setExperimentInfo(const cExperimentInfo& info);

	void addPlotInfo(const cPlotMetaData& info);
	void addPlotData(int id, int doy, double height_mm, double lowerBound_mm, double upperBount_mm);

	void write(const std::string& directory);

private:
	cExperimentMetaData mExperimentInfo;

	std::vector<cPlotMetaData> mPlots;

	struct sPlotHeightData_t
	{
		int doy = 0;
		double height_mm = 0;
		double lowerBound_mm = 0;
		double upperBount_mm = 0;
	};

	std::map<int, std::vector<sPlotHeightData_t> > mPlotHeights;
};

