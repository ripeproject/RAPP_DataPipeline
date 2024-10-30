
#pragma once

#include "PlotMetaData.hpp"

#include <cbdf/ExperimentInfo.hpp>

#include <cstdint>
#include <vector>
#include <map>
#include <set>


class cPlotData
{
public:
	cPlotData() = default;
	~cPlotData();

	void setExperimentInfo(const cExperimentInfo& info);

	void addPlotInfo(const cPlotMetaData& info);

	void addDate(int month, int day, int year, int doy);

	void addPlotData(int id, int doy, double height_mm, double lowerBound_mm, double upperBount_mm);

	void write_metadata_file(const std::string& directory);
	void write_plot_height_file(const std::string& directory);

protected:
	void splitIntoGroups(cPlotMetaData* pPlotInfo);
	bool inGroup(cPlotMetaData* pPlotInfo1, cPlotMetaData* pPlotInfo2);

private:
	cExperimentMetaData mExperimentInfo;

	std::vector<cPlotMetaData*> mpPlots;

	// Note: mGroups does not own the pointer
	std::vector<std::vector<cPlotMetaData*>> mGroups;

	struct sPlotDate_t
	{
		int month = 0;
		int day = 0;
		int year = 0;
		int doy = 0;

		bool operator<(const sPlotDate_t& rhs) const
		{
			return doy < rhs.doy;
		}
	};

	std::set<sPlotDate_t> mDates;

	struct sPlotHeightData_t
	{
		int doy = 0;
		double height_mm = 0;
		double lowerBound_mm = 0;
		double upperBount_mm = 0;
	};

	std::map<int, std::vector<sPlotHeightData_t> > mPlotHeights;
};

