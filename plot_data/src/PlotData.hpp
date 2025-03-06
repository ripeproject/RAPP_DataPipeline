
#pragma once

#include <cbdf/PlotMetaData.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <set>


class cPlotData
{
public:
	cPlotData() = default;
	~cPlotData();

	void setRootFileName(const std::string& filename);

	bool saveRowMajor() const;
	void saveRowMajor(bool row_major);

	void setExperimentInfo(const cExperimentInfo& info);

	void addPlotInfo(const cPlotMetaData& info);

	void addDate(int month, int day, int year, int doy);

	void addPlotData(int plot_id, int doy, int num_of_points, double height_mm, double lowerBound_mm, double upperBount_mm);
	void addPlotBiomass(int plot_id, int doy, double biomass);

	void computeReplicateData();

	void write_metadata_file(const std::string& directory);
	void write_metadata_file(const std::string& directory, const std::string& filename);

	void write_plot_num_points_file(const std::string& directory);
	void write_plot_num_points_file(const std::string& directory, const std::string& filename);

	void write_plot_height_file(const std::string& directory);
	void write_plot_height_file(const std::string& directory, const std::string& filename);

	void write_replicate_height_file(const std::string& directory);
	void write_replicate_height_file(const std::string& directory, const std::string& filename);

	void write_plot_biomass_file(const std::string& directory);
	void write_plot_biomass_file(const std::string& directory, const std::string& filename);

	void write_replicate_biomass_file(const std::string& directory);
	void write_replicate_biomass_file(const std::string& directory, const std::string& filename);

protected:
	void write_plot_num_points_file_by_row(std::ofstream& out);
	void write_plot_num_points_file_by_column(std::ofstream& out);

	void write_plot_height_file_by_row(std::ofstream& out);
	void write_plot_height_file_by_column(std::ofstream& out);

	void write_replicate_height_file_by_row(std::ofstream& out);
	void write_replicate_height_file_by_column(std::ofstream& out);

	void write_plot_biomass_file_by_row(std::ofstream& out);
	void write_plot_biomass_file_by_column(std::ofstream& out);

	void write_replicate_biomass_file_by_row(std::ofstream& out);
	void write_replicate_biomass_file_by_column(std::ofstream& out);

	void splitIntoGroups(cPlotMetaData* pPlotInfo);
	bool inGroup(cPlotMetaData* pPlotInfo1, cPlotMetaData* pPlotInfo2);

private:
	std::string mRootFileName;

	cExperimentMetaData mExperimentInfo;

	std::vector<cPlotMetaData*> mpPlots;

	bool mSaveRowMajor = false;

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

	struct sPlotNumPointData_t
	{
		int doy = 0;
		int num_points = 0;
	};

	std::map<int, std::vector<sPlotNumPointData_t> > mPlotSizes;

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

