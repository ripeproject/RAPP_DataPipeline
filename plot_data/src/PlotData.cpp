
#include "PlotData.hpp"

#include "Constants.hpp"

//#include <fmt/format.h>
#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <valarray>


cPlotData::~cPlotData()
{
	for (auto plot : mpPlots)
	{
		delete plot;
	}

	mpPlots.clear();

	for (auto& group : mGroups)
	{
		group.clear();
	}

	mGroups.clear();
}

void cPlotData::setRootFileName(const std::string& filename)
{
	mRootFileName = filename;
}

bool cPlotData::saveRowMajor() const
{
	return mSaveRowMajor;
}

void cPlotData::saveRowMajor(bool row_major)
{
	mSaveRowMajor = row_major;
}

void cPlotData::setExperimentInfo(const cExperimentInfo& info)
{
	mExperimentInfo = info;
}

void cPlotData::addPlotInfo(const cPlotMetaData& info)
{
	for (const auto& plot : mpPlots)
	{
		if (plot->id() == info.id())
			return;
	}

	cPlotMetaData* pPlotInfo = new cPlotMetaData(info);
	mpPlots.push_back(pPlotInfo);

	std::sort(mpPlots.begin(), mpPlots.end(), [](const auto* a, const auto* b) { return a->id() < b->id(); });

	splitIntoGroups(pPlotInfo);
}

void cPlotData::addDate(int month, int day, int year, int doy)
{
	sPlotDate_t date;
	date.month = month;
	date.day = day;
	date.year = year;
	date.doy = doy;
	mDates.insert(date);
}

void cPlotData::addPlotData(int plot_id, int doy, double height_mm, double lowerBound_mm, double upperBount_mm)
{
	sPlotHeightData_t data;

	data.doy = doy;
	data.height_mm = height_mm;
	data.lowerBound_mm = lowerBound_mm;
	data.upperBount_mm = upperBount_mm;

	mPlotHeights[plot_id].push_back(data);
}

void cPlotData::addPlotBiomass(int plot_id, int doy, double biomass)
{
	sPlotBioMassData_t data;

	data.doy = doy;
	data.biomass = biomass;

	mPlotBioMasses[plot_id].push_back(data);
}

void cPlotData::computeReplicateData()
{
	// Compute Plot Height Data
	for (auto& group : mGroupHeights)
	{
		group.second.clear();
	}
	mGroupHeights.clear();

	for (int groupNum = 0; groupNum < mGroups.size(); ++groupNum)
	{
		auto& group = mGroups[groupNum];

		std::vector<sGroupHeightData_t> data;

		for (const auto& date : mDates)
		{
			std::valarray<double> heights(group.size());

			int count = 0;

			for (std::size_t i = 0; i < group.size(); ++i)
			{
				const auto& plot_heights = mPlotHeights[group[i]->id()];

				for (const auto& plot_height : plot_heights)
				{
					if (plot_height.doy == date.doy)
					{
						heights[i] = plot_height.height_mm;
						++count;
						break;
					}
				}
			}

			if (count > 0)
			{
				double mean = heights.sum() / count;

				heights -= mean;
				auto h2 = std::pow(heights, 2);

				double err = 0.0;

				if (count > 1)
				{
					h2 /= (count - 1);
					err = sqrt(h2.sum());
				}

				data.emplace_back(sGroupHeightData_t{ date.doy, mean, err });
			}
		}

		mGroupHeights[groupNum] = data;
	}

	// Compute Biomass Data
	for (auto& group : mGroupBioMasses)
	{
		group.second.clear();
	}
	mGroupBioMasses.clear();

	for (int groupNum = 0; groupNum < mGroups.size(); ++groupNum)
	{
		auto& group = mGroups[groupNum];

		std::vector<sGroupBioMassData_t> data;

		for (const auto& date : mDates)
		{
			std::valarray<double> biomasses(group.size());

			int count = 0;

			for (std::size_t i = 0; i < group.size(); ++i)
			{
				const auto& plot_biomasses = mPlotBioMasses[group[i]->id()];

				for (const auto& plot_biomass : plot_biomasses)
				{
					if (plot_biomass.doy == date.doy)
					{
						biomasses[i] = plot_biomass.biomass;
						++count;
						break;
					}
				}
			}

			if (count > 0)
			{
				double mean = biomasses.sum() / count;

				biomasses -= mean;
				auto h2 = std::pow(biomasses, 2);

				double err = 0.0;

				if (count > 1)
				{
					h2 /= (count - 1);
					err = sqrt(h2.sum());
				}

				data.emplace_back(sGroupBioMassData_t{ date.doy, mean, err });
			}
		}

		mGroupBioMasses[groupNum] = data;
	}
}

void cPlotData::write_metadata_file(const std::string& directory)
{
	write_metadata_file(directory, mRootFileName);
}

void cPlotData::write_metadata_file(const std::string& directory, const std::string& filename)
{
	std::filesystem::path meta_file = directory;

	meta_file /= filename;
	meta_file.replace_extension("metadata");

	std::ofstream out;
	out.open(meta_file, std::ios::trunc);
	if (!out.is_open())
		return;

	out << "\nUnits: \n";
	out << "Plot Heights: millimeters\n";
	out << "Plot BioMass: ??\n";

	out << "\n";

	out << "\nExperiment Information: \n";
	out << "Title: " << mExperimentInfo.title() << "\n";

	const auto& comments = mExperimentInfo.comments();
	if (!comments.empty())
	{
		if (comments.size() == 1)
			out << "Comment: " << comments.front() << "\n";
		else
		{
			out << "Comments: " << comments.front() << "\n";
			for (std::size_t i = 1; i < comments.size(); ++i)
			{
				out << "          " << comments[i] << "\n";
			}
		}
	}

	if (!mExperimentInfo.principalInvestigator().empty())
	{
		out << "Principal Investigator: " << mExperimentInfo.principalInvestigator() << "\n";
	}

	const auto& researchers = mExperimentInfo.researchers();
	if (!researchers.empty())
	{
		if (researchers.size() == 1)
			out << "Researcher: " << researchers.front() << "\n";
		else
		{
			out << "Researchers: " << researchers.front() << "\n";
			for (std::size_t i = 1; i < researchers.size(); ++i)
			{
				out << "          " << researchers[i] << "\n";
			}
		}
	}

	if (!mExperimentInfo.species().empty())
	{
		out << "Species: " << mExperimentInfo.species() << "\n";
	}

	if (!mExperimentInfo.cultivar().empty())
	{
		out << "Cultivar: " << mExperimentInfo.cultivar() << "\n";
	}

	if (!mExperimentInfo.construct().empty())
	{
		out << "Construct: " << mExperimentInfo.construct() << "\n";
	}

	const auto& eventNumbers = mExperimentInfo.eventNumbers();
	if (!eventNumbers.empty())
	{
		if (eventNumbers.size() == 1)
			out << "Event Number: " << eventNumbers.front() << "\n";
		else
		{
			out << "Event Numbers: " << eventNumbers.front() << ", ";
			for (std::size_t i = 1; i < eventNumbers.size(); ++i)
			{
				out << eventNumbers[i] << ", ";
			}
			out << "\n";
		}
	}

	const auto& treatments = mExperimentInfo.treatments();
	if (!treatments.empty())
	{
		if (treatments.size() == 1)
			out << "Treatment: " << treatments.front() << "\n";
		else
		{
			out << "Treatments: " << treatments.front() << "\n";
			for (std::size_t i = 1; i < treatments.size(); ++i)
			{
				out << "          " << treatments[i] << "\n";
			}
		}
	}

	if (!mExperimentInfo.fieldDesign().empty())
	{
		out << "Field Design: " << mExperimentInfo.fieldDesign() << "\n";
	}

	if (!mExperimentInfo.permit().empty())
	{
		out << "Permit: " << mExperimentInfo.permit() << "\n";
	}

//	const std::optional<nExpTypes::sDateDoy_t>& plantingDate() const;
//	const std::optional<nExpTypes::sDateDoy_t>& harvestDate() const;

	out << "\n\nPlot Information:\n";

	for (const auto& plot : mpPlots)
	{
		out << "Plot Number: " << plot->id() << "\n";

		if (!plot->name().empty())
		{
			out << "Name: " << plot->name() << "\n";
		}

		if (!plot->description().empty())
		{
			out << "Description: " << plot->description() << "\n";
		}

		if (!plot->species().empty())
		{
			out << "Species: " << plot->species() << "\n";
		}

		if (!plot->cultivar().empty())
		{
			out << "Cultivar: " << plot->cultivar() << "\n";
		}

		if (!plot->event().empty())
		{
			out << "Event: " << plot->event() << "\n";
		}

		if (!plot->constructName().empty())
		{
			out << "Construct Name: " << plot->constructName() << "\n";
		}

		if (!plot->seedGeneration().empty())
		{
			out << "Seed Generation: " << plot->seedGeneration() << "\n";
		}

		if (!plot->copyNumber().empty())
		{
			out << "Copy Number: " << plot->copyNumber() << "\n";
		}

		if (!plot->potLabel().empty())
		{
			out << "Pot Label: " << plot->potLabel() << "\n";
		}

		out << "\n\n";
	}

	out.close();
}

void cPlotData::write_plot_height_file(const std::string& directory)
{
	write_plot_height_file(directory, mRootFileName);
}

void cPlotData::write_plot_height_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mPlotHeights.empty())
		return;

	std::filesystem::path plot_height_file = directory;

	plot_height_file /= filename;
	plot_height_file.replace_extension("plot_heights.csv");

	std::ofstream out;
	out.open(plot_height_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_plot_height_file_by_row(out);
	else
		write_plot_height_file_by_column(out);

	out.close();
}

void cPlotData::write_plot_height_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ",\t";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ",\t";
	}
	out << doy_last->doy << "\n";

	for (const auto& plot : mPlotHeights)
	{
		out << "Plot_" << plot.first << ",\t";

		const auto& heights = plot.second;

		auto last = heights.end();
		--last;

		for (auto it = heights.begin(); it != last; ++it)
		{
			out << it->height_mm << ",\t";
		}
		out << last->height_mm << "\n";
	}
}

void cPlotData::write_plot_height_file_by_column(std::ofstream& out)
{
	out << "Date,\t" << "DayOfYear,\t";

	auto plot_last = --(mPlotHeights.end());
	for (auto it = mPlotHeights.begin(); it != plot_last; ++it)
	{
		out << "Plot_" << it->first << ", ";
	}
	out << "Plot_" << plot_last->first << "\n";

	auto n = mDates.size();
	n = std::min(n, mPlotHeights.size());

	auto date_it = mDates.begin();
	for (std::size_t i = 0; i < n; ++i, ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ",\t";
		out << date_it->doy << ",\t";

		for (auto it = mPlotHeights.begin(); it != plot_last; ++it)
		{
			out << (it->second)[i].height_mm << ", ";
		}
		out << (plot_last->second)[i].height_mm << "\n";
	}
}

void cPlotData::write_replicate_height_file(const std::string& directory)
{
	write_replicate_height_file(directory, mRootFileName);
}

void cPlotData::write_replicate_height_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mGroupHeights.empty())
		return;

	bool useEvent = true;
	bool useSpecies = false;
	bool useCultivar = false;
	bool useConstructName= false;
	bool useSeedGeneration = false;
	bool useCopyNumber = false;

	//useEvent = false; pPlotInfo1->event() == pPlotInfo2->event();
	//useSpecies = false; same &= pPlotInfo1->species() == pPlotInfo2->species();
	//useCultivar = false; same &= pPlotInfo1->cultivar() == pPlotInfo2->cultivar();
	//useConstructName = false; same &= pPlotInfo1->constructName() == pPlotInfo2->constructName();
	//useSeedGeneration = false; same &= pPlotInfo1->seedGeneration() == pPlotInfo2->seedGeneration();
	//useCopyNumber = false; same &= pPlotInfo1->copyNumber() == pPlotInfo2->copyNumber();

	std::filesystem::path height_file = directory;

	height_file /= filename;
	height_file.replace_extension("heights.csv");

	std::ofstream out;
	out.open(height_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_replicate_height_file_by_row(out);
	else
		write_replicate_height_file_by_column(out);

	out.close();
}

void cPlotData::write_replicate_height_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ",\t";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "Day of Year,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ",\t";
	}
	out << doy_last->doy << "\n";

	for (const auto& group : mGroupHeights)
	{
		const auto& info = mGroups[group.first].front();

		out << info->event() << ",\t";

		const auto& heights = group.second;

		auto last = heights.end();
		--last;

		for (auto it = heights.begin(); it != last; ++it)
		{
			out << it->avgHeight_mm << ", " << it->stdHeight_mm << ",\t";
		}
		out << last->avgHeight_mm << ", " << last->stdHeight_mm << "\n";
	}
}

void cPlotData::write_replicate_height_file_by_column(std::ofstream& out)
{
	out << "Date,\t" << "DayOfYear,\t";

	auto group_last = --(mGroups.end());
	for (auto it = mGroups.begin(); it != group_last; ++it)
	{
		const auto& info = (*it).front();
		out << info->event() << " (avg), " << info->event() << " (std dev), ";
	}
	const auto& info = (*group_last).front();
	out << info->event() << " (avg), " << info->event() << " (std dev)\n";

	auto n = mDates.size();
	n = std::min(n, mGroupHeights.begin()->second.size());

	auto height_last = --(mGroupHeights.end());
	auto date_it = mDates.begin();
	for (std::size_t i = 0; i < n; ++i, ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ",\t";
		out << date_it->doy << ",\t";

		for (auto it = mGroupHeights.begin(); it != height_last; ++it)
		{
			out << (it->second)[i].avgHeight_mm << ", " << (it->second)[i].stdHeight_mm << ", ";
		}
		out << (height_last->second)[i].avgHeight_mm << ", " << (height_last->second)[i].stdHeight_mm << "\n";
	}
}

void cPlotData::write_plot_biomass_file(const std::string& directory)
{
	write_plot_biomass_file(directory, mRootFileName);
}

void cPlotData::write_plot_biomass_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mPlotBioMasses.empty())
		return;

	std::filesystem::path plot_biomass_file = directory;

	plot_biomass_file /= filename;
	plot_biomass_file.replace_extension("plot_biomass.csv");

	std::ofstream out;
	out.open(plot_biomass_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_plot_biomass_file_by_row(out);
	else
		write_plot_biomass_file_by_column(out);

	out.close();
}

void cPlotData::write_plot_biomass_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ",\t";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ",\t";
	}
	out << doy_last->doy << "\n";

	for (const auto& plot : mPlotBioMasses)
	{
		out << "Plot_" << plot.first << ",\t";

		const auto& heights = plot.second;

		auto last = heights.end();
		--last;

		for (auto it = heights.begin(); it != last; ++it)
		{
			out << it->biomass << ",\t";
		}
		out << last->biomass << "\n";
	}
}

void cPlotData::write_plot_biomass_file_by_column(std::ofstream& out)
{
	out << "Date,\t" << "DayOfYear,\t";

	auto plot_last = --(mPlotBioMasses.end());
	for (auto it = mPlotBioMasses.begin(); it != plot_last; ++it)
	{
		out << "Plot_" << it->first << ", ";
	}
	out << "Plot_" << plot_last->first << "\n";

	auto n = mDates.size();
	n = std::min(n, mPlotBioMasses.size());

	auto date_it = mDates.begin();
	for (std::size_t i = 0; i < n; ++i, ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ",\t";
		out << date_it->doy << ",\t";

		for (auto it = mPlotBioMasses.begin(); it != plot_last; ++it)
		{
			out << (it->second)[i].biomass << ", ";
		}
		out << (plot_last->second)[i].biomass << "\n";
	}
}

void cPlotData::write_replicate_biomass_file(const std::string& directory)
{
	write_replicate_biomass_file(directory, mRootFileName);
}

void cPlotData::write_replicate_biomass_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mGroupBioMasses.empty())
		return;

	bool useEvent = true;
	bool useSpecies = false;
	bool useCultivar = false;
	bool useConstructName = false;
	bool useSeedGeneration = false;
	bool useCopyNumber = false;

	//useEvent = false; pPlotInfo1->event() == pPlotInfo2->event();
	//useSpecies = false; same &= pPlotInfo1->species() == pPlotInfo2->species();
	//useCultivar = false; same &= pPlotInfo1->cultivar() == pPlotInfo2->cultivar();
	//useConstructName = false; same &= pPlotInfo1->constructName() == pPlotInfo2->constructName();
	//useSeedGeneration = false; same &= pPlotInfo1->seedGeneration() == pPlotInfo2->seedGeneration();
	//useCopyNumber = false; same &= pPlotInfo1->copyNumber() == pPlotInfo2->copyNumber();

	std::filesystem::path biomass_file = directory;

	biomass_file /= filename;
	biomass_file.replace_extension("biomass.csv");

	std::ofstream out;
	out.open(biomass_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_replicate_biomass_file_by_row(out);
	else
		write_replicate_biomass_file_by_column(out);

	out.close();
}

void cPlotData::write_replicate_biomass_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ",\t";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear,\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ",\t";
	}
	out << doy_last->doy << "\n";

	for (const auto& group : mGroupBioMasses)
	{
		const auto& info = mGroups[group.first].front();

		out << info->event() << ",\t";

		const auto& biomasses = group.second;

		auto last = biomasses.end();
		--last;

		for (auto it = biomasses.begin(); it != last; ++it)
		{
			out << it->avgBioMass << ", " << it->stdBioMass << ",\t";
		}
		out << last->avgBioMass << ", " << last->stdBioMass << "\n";
	}
}

void cPlotData::write_replicate_biomass_file_by_column(std::ofstream& out)
{
	out << "Date,\t" << "DayOfYear,\t";

	auto group_last = --(mGroups.end());
	for (auto it = mGroups.begin(); it != group_last; ++it)
	{
		const auto& info = (*it).front();
		out << info->event() << " (avg), " << info->event() << " (std dev), ";
	}
	const auto& info = (*group_last).front();
	out << info->event() << " (avg), " << info->event() << " (std dev)\n";

	auto n = mDates.size();
	n = std::min(n, mGroupBioMasses.begin()->second.size());

	auto biomass_last = --(mGroupBioMasses.end());
	auto date_it = mDates.begin();
	for (std::size_t i = 0; i < n; ++i, ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ",\t";
		out << date_it->doy << ",\t";

		for (auto it = mGroupBioMasses.begin(); it != biomass_last; ++it)
		{
			out << (it->second)[i].avgBioMass << ", " << (it->second)[i].stdBioMass << ", ";
		}
		out << (biomass_last->second)[i].avgBioMass << ", " << (biomass_last->second)[i].stdBioMass << "\n";
	}
}

void cPlotData::splitIntoGroups(cPlotMetaData* pPlotInfo)
{
	// Check to see if the plot is already placed in a group
	for (auto& group : mGroups)
	{
		for (auto plot : group)
		{
			if (plot->id() == pPlotInfo->id())
				return;
		}
	}

	for (auto& group : mGroups)
	{
		if (inGroup(group.front(), pPlotInfo))
		{
			group.push_back(pPlotInfo);
			std::sort(group.begin(), group.end(), [](const auto* a, const auto* b) { return a->id() < b->id(); });

			return;
		}
	}

	std::vector<cPlotMetaData*> group;
	group.push_back(pPlotInfo);
	mGroups.push_back(group);
}

bool cPlotData::inGroup(cPlotMetaData* pPlotInfo1, cPlotMetaData* pPlotInfo2)
{
	bool same = pPlotInfo1->event() == pPlotInfo2->event();
	same &= pPlotInfo1->species() == pPlotInfo2->species();
	same &= pPlotInfo1->cultivar() == pPlotInfo2->cultivar();
	same &= pPlotInfo1->constructName() == pPlotInfo2->constructName();
	same &= pPlotInfo1->seedGeneration() == pPlotInfo2->seedGeneration();
	same &= pPlotInfo1->copyNumber() == pPlotInfo2->copyNumber();

	return same;
}
