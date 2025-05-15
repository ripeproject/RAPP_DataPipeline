
#pragma once

#include <cbdf/PlotMetaData.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <cstdint>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <set>


class cPlotData
{
public:
	cPlotData() = default;
	~cPlotData();

	void clear();

	void setRootFileName(const std::string& filename);

	bool saveRowMajor() const;
	void saveRowMajor(bool row_major);

	void setExperimentInfo(const cExperimentInfo& info);
	void setExperimentTitle(const std::string& title);
	void setMeasurementTitle(const std::string& title);

	void useEvent(bool use);
	void useSpecies(bool use);
	void useCultivar(bool use);
	void useTreatment(bool use);
	void useConstructName(bool use);
	void usePotLabel(bool use);
	void useSeedGeneration(bool use);
	void useCopyNumber(bool use);
	void useLeafType(bool use);

	void clearGroups();
	void splitIntoGroups();

	bool hasGroupInfo();

	void addPlotInfo(const cPlotMetaData& info);

	void addDate(int month, int day, int year, int doy);

	void clearHeightMetaInfo();
	void addHeightMetaInfo(std::string_view info);
	void addHeightMetaInfo(const std::vector<std::string>& info);

	void addPlotHeight(int plot_id, int doy, int num_of_points, double height_mm, double lowerBound_mm, double upperBount_mm);

	void clearBiomassMetaInfo();
	void addBiomassMetaInfo(std::string_view info);
	void addBiomassMetaInfo(const std::vector<std::string>& info);

	void addPlotBiomass(int plot_id, int doy, double biomass);

	void clearLAI_MetaInfo();
	void addLAI_MetaInfo(std::string_view info);
	void addLAI_MetaInfo(const std::vector<std::string>& info);

	void addPlotLAI(int plot_id, int doy, double lai);

	void computeReplicateData();

	void write_metadata_file(const std::string& directory);
	void write_metadata_file(const std::string& directory, const std::string& filename);

	void write_plot_num_points_file(const std::string& directory);
	void write_plot_num_points_file(const std::string& directory, const std::string& filename);

	void write_replicate_num_points_file(const std::string& directory);
	void write_replicate_num_points_file(const std::string& directory, const std::string& filename);

	void write_plot_height_file(const std::string& directory);
	void write_plot_height_file(const std::string& directory, const std::string& filename);

	void write_replicate_height_file(const std::string& directory);
	void write_replicate_height_file(const std::string& directory, const std::string& filename);

	void write_plot_biomass_file(const std::string& directory);
	void write_plot_biomass_file(const std::string& directory, const std::string& filename);

	void write_replicate_biomass_file(const std::string& directory);
	void write_replicate_biomass_file(const std::string& directory, const std::string& filename);

	void write_plot_lai_file(const std::string& directory);
	void write_plot_lai_file(const std::string& directory, const std::string& filename);

	void write_replicate_lai_file(const std::string& directory);
	void write_replicate_lai_file(const std::string& directory, const std::string& filename);

protected:
	void write_plot_num_points_file_by_row(std::ofstream& out);
	void write_plot_num_points_file_by_column(std::ofstream& out);

	void write_replicate_num_points_file_by_row(std::ofstream& out);
	void write_replicate_num_points_file_by_column(std::ofstream& out);

	void write_plot_height_file_by_row(std::ofstream& out);
	void write_plot_height_file_by_column(std::ofstream& out);

	void write_replicate_height_file_by_row(std::ofstream& out);
	void write_replicate_height_file_by_column(std::ofstream& out);

	void write_plot_biomass_file_by_row(std::ofstream& out);
	void write_plot_biomass_file_by_column(std::ofstream& out);

	void write_replicate_biomass_file_by_row(std::ofstream& out);
	void write_replicate_biomass_file_by_column(std::ofstream& out);

	void write_plot_lai_file_by_row(std::ofstream& out);
	void write_plot_lai_file_by_column(std::ofstream& out);

	void write_replicate_lai_file_by_row(std::ofstream& out);
	void write_replicate_lai_file_by_column(std::ofstream& out);

	bool inGroup(cPlotMetaData* pPlotInfo1, cPlotMetaData* pPlotInfo2);

private:
	std::string mRootFileName;

	cExperimentMetaData mExperimentInfo;

	std::vector<cPlotMetaData*> mpPlots;

	bool mSaveRowMajor = false;

	// Group By Information
	bool mUseEvent = false;
	bool mUseSpecies = false;
	bool mUseCultivar = false;
	bool mUseTreatment = false;
	bool mUseConstructName = false;
	bool mUsePotLabel = false;
	bool mUseSeedGeneration = false;
	bool mUseCopyNumber = false;
	bool mUseLeafType = false;

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

	std::map<int, std::vector<sPlotNumPointData_t> > mPlotNumPoints;

	struct sGroupNumPoints_t
	{
		int doy = 0;
		double avgNumPoints = 0;
		double stdNumPoints = 0;
	};

	std::map<int, std::vector<sGroupNumPoints_t> > mGroupNumPoints;

	std::vector<std::string> mHeightMetaData;

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

	std::vector<std::string> mBioMassMetaData;

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

	std::vector<std::string> mLAI_MetaData;

	struct sPlotLAI_Data_t
	{
		int doy = 0;
		double lai = 0;
	};

	std::map<int, std::vector<sPlotLAI_Data_t> > mPlotLAIs;

	struct sGroupLAI_Data_t
	{
		int doy = 0;
		double avgLAI = 0;
		double stdLAI = 0;
	};

	std::map<int, std::vector<sGroupLAI_Data_t> > mGroupLAIs;
};

