
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

	void addPlotData(int plot_id, int doy, double height_mm, double lowerBound_mm, double upperBount_mm);
	void addPlotBiomass(int plot_id, int doy, double biomass);

	void computeReplicateData();

	void write_metadata_file(const std::string& directory, const std::string& title);

	void write_plot_height_file(const std::string& directory, const std::string& title);
	void write_replicate_height_file(const std::string& directory, const std::string& title);

	void write_plot_biomass_file(const std::string& directory, const std::string& title);
	void write_replicate_biomass_file(const std::string& directory, const std::string& title);

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

	struct sGroupHeightData_t
	{
		int doy = 0;
		double avgHeight_mm = 0;
		double stdHeight_mm = 0;
	};

	std::map<int, std::vector<sGroupHeightData_t> > mGroupHeights;

	struct sPlotBioMassData_t
	{
		int doy = 0;
		double biomass = 0;
	};

	std::map<int, std::vector<sPlotBioMassData_t> > mPlotBioMasses;

	struct sGroupBioMassData_t
	{
		int doy = 0;
		double avgBioMass = 0;
		double stdBioMass = 0;
	};

	std::map<int, std::vector<sGroupBioMassData_t> > mGroupBioMasses;
};

