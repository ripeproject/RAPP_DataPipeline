
#include "PlotData.hpp"

#include "Constants.hpp"

//#include <fmt/format.h>
#include <cbdf/PointCloudTypes.hpp>

#include <cstdint>
#include <algorithm>
#include <numeric>
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

void cPlotData::clear()
{
	mExperimentInfo.clear();

	for (auto plot : mpPlots)
	{
		delete plot;
	}

	mpPlots.clear();

	mUseEvent = false;
	mUseSpecies = false;
	mUseCultivar = false;
	mUseTreatment = false;
	mUseConstructName = false;
	mUsePotLabel = false;
	mUseSeedGeneration = false;
	mUseCopyNumber = false;
	mUseLeafType = false;

	for (auto& group : mGroups)
	{
		group.clear();
	}

	mGroups.clear();

	mDates.clear();
	mPlotNumPoints.clear();
	mGroupNumPoints.clear();

	mHeightMetaData.clear();
	mPlotHeights.clear();
	mGroupHeights.clear();

	mBioMassMetaData.clear();
	mPlotBioMasses.clear();
	mGroupBioMasses.clear();

	mLAI_MetaData.clear();
	mPlotLAIs.clear();
	mGroupLAIs.clear();
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

void cPlotData::setExperimentTitle(const std::string& title)
{
	mExperimentInfo.setExperimentTitle(title);
}

void cPlotData::setMeasurementTitle(const std::string& title)
{
	mExperimentInfo.setMeasurementTitle(title);
}

void cPlotData::useEvent(bool use)
{
	mUseEvent = use;
}

void cPlotData::useSpecies(bool use)
{
	mUseSpecies = use;
}

void cPlotData::useCultivar(bool use)
{
	mUseCultivar = use;
}

void cPlotData::useTreatment(bool use)
{
	mUseTreatment = use;
}

void cPlotData::useConstructName(bool use)
{
	mUseConstructName = use;
}

void cPlotData::usePotLabel(bool use)
{
	mUsePotLabel = use;
}

void cPlotData::useSeedGeneration(bool use)
{
	mUseSeedGeneration = use;
}

void cPlotData::useCopyNumber(bool use)
{
	mUseCopyNumber = use;
}

void cPlotData::useLeafType(bool use)
{
	mUseLeafType = use;
}

bool cPlotData::hasGroupInfo()
{
	return mGroups.size() > 0;
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

void cPlotData::clearHeightMetaInfo()
{
	mHeightMetaData.clear();
}

void cPlotData::addHeightMetaInfo(std::string_view info)
{
	mHeightMetaData.push_back(std::string(info));
}

void cPlotData::addHeightMetaInfo(const std::vector<std::string>& info)
{
	if (info.empty())
		return;

	std::copy(info.begin(), info.end(), std::back_inserter(mHeightMetaData));
}


void cPlotData::addPlotHeight(int plot_id, int doy, int num_of_points, double height_mm, double lowerBound_mm, double upperBount_mm)
{
	{
		sPlotNumPointData_t data;

		data.doy = doy;
		data.num_points = num_of_points;

		mPlotNumPoints[plot_id].push_back(data);
	}

	sPlotHeightData_t data;

	data.doy = doy;
	data.height_mm = height_mm;
	data.lowerBound_mm = lowerBound_mm;
	data.upperBount_mm = upperBount_mm;

	mPlotHeights[plot_id].push_back(data);
}

void cPlotData::clearBiomassMetaInfo()
{
	mBioMassMetaData.clear();
}

void cPlotData::addBiomassMetaInfo(std::string_view info)
{
	mBioMassMetaData.push_back(std::string(info));
}

void cPlotData::addBiomassMetaInfo(const std::vector<std::string>& info)
{
	if (info.empty())
		return;

	std::copy(info.begin(), info.end(), std::back_inserter(mBioMassMetaData));
}

void cPlotData::addPlotBiomass(int plot_id, int doy, double biomass)
{
	sPlotBioMassData_t data;

	data.doy = doy;
	data.biomass = biomass;

	mPlotBioMasses[plot_id].push_back(data);
}

void cPlotData::clearLAI_MetaInfo()
{
	mLAI_MetaData.clear();
}

void cPlotData::addLAI_MetaInfo(std::string_view info)
{
	mLAI_MetaData.push_back(std::string(info));
}

void cPlotData::addLAI_MetaInfo(const std::vector<std::string>& info)
{
	if (info.empty())
		return;

	std::copy(info.begin(), info.end(), std::back_inserter(mLAI_MetaData));
}

void cPlotData::addPlotLAI(int plot_id, int doy, double lai)
{
	sPlotLAI_Data_t data;

	data.doy = doy;
	data.lai = lai;

	mPlotLAIs[plot_id].push_back(data);
}

void cPlotData::computeReplicateData()
{
	// Compute Plot Num Points Data
	for (auto& group : mGroupNumPoints)
	{
		group.second.clear();
	}
	mGroupNumPoints.clear();

	for (int groupNum = 0; groupNum < mGroups.size(); ++groupNum)
	{
		auto& group = mGroups[groupNum];

		std::vector<sGroupNumPoints_t> data;

		for (const auto& date : mDates)
		{
			std::valarray<double> num_points(group.size());

			int count = 0;

			for (std::size_t i = 0; i < group.size(); ++i)
			{
				const auto& plot_num_points = mPlotNumPoints[group[i]->id()];

				for (const auto& plot_num_point : plot_num_points)
				{
					if (plot_num_point.doy == date.doy)
					{
						num_points[i] = plot_num_point.num_points;
						++count;
						break;
					}
				}
			}

			if (count > 0)
			{
				double mean = num_points.sum() / count;

				num_points -= mean;
				auto h2 = std::pow(num_points, 2);

				double err = 0.0;

				if (count > 1)
				{
					h2 /= (count - 1);
					err = sqrt(h2.sum());
				}

				data.emplace_back(sGroupNumPoints_t{ date.doy, mean, err });
			}
		}

		mGroupNumPoints[groupNum] = data;
	}

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
	meta_file.replace_extension("metadata.txt");

	std::ofstream out;
	out.open(meta_file, std::ios::trunc);
	if (!out.is_open())
		return;

	out << "\nUnits: \n";
	out << "Plot Heights: millimeters\n";
	out << "Plot BioMass: mm^3 / mm^2\n";

	out << "\n";

	if (!mHeightMetaData.empty())
	{
		out << "\n";

		out << "Plot Height Info:\n";

		for (const auto& info : mHeightMetaData)
		{
			out << info << "\n";
		}

		out << "\n";
	}

	if (!mBioMassMetaData.empty())
	{
		out << "\n";

		out << "Plot Biomass Info:\n";

		for (const auto& info : mBioMassMetaData)
		{
			out << info << "\n";
		}

		out << "\n";
	}

	if (!mLAI_MetaData.empty())
	{
		out << "\n";

		out << "Plot LAI Info:\n";

		for (const auto& info : mLAI_MetaData)
		{
			out << info << "\n";
		}

		out << "\n";
	}

	out << "\n";
	out << "Experiment Information: \n";

	auto title = mExperimentInfo.experimentTitle();
	if (title.empty())
		auto title = mExperimentInfo.measurementTitle();

	out << "Title: " << title << "\n";

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

		if (!plot->leafType().empty())
		{
			out << "Leaf Type: " << plot->leafType() << "\n";
		}

		out << "\n\n";
	}

	if (!mGroups.empty())
	{
		out << "\n\nGroup Information:\n";

		for (int group_num = 0; group_num < mGroups.size(); ++group_num)
		{
			const auto& group = mGroups[group_num];
			const auto* plot = group.front();

			out << "Group Number: " << group_num + 1 << "\n";

			if (group.size() == 1)
			{
				out << "Plot Number: " << plot->id() << "\n";
			}
			else
			{
				out << "Plot Numbers: ";

				auto last = group.end();
				--last;
				for (auto plot = group.begin(); plot != last; ++plot)
				{
					out << (*plot)->id() << ", ";
				}
				out << group.back()->id() << "\n";
			}

			if (mUseSpecies && !plot->species().empty())
			{
				out << "Species: " << plot->species() << "\n";
			}

			if (mUseCultivar && !plot->cultivar().empty())
			{
				out << "Cultivar: " << plot->cultivar() << "\n";
			}

			if (mUseEvent && !plot->event().empty())
			{
				out << "Event: " << plot->event() << "\n";
			}

			if (mUseConstructName && !plot->constructName().empty())
			{
				out << "Construct Name: " << plot->constructName() << "\n";
			}

			if (mUseSeedGeneration && !plot->seedGeneration().empty())
			{
				out << "Seed Generation: " << plot->seedGeneration() << "\n";
			}

			if (mUseCopyNumber && !plot->copyNumber().empty())
			{
				out << "Copy Number: " << plot->copyNumber() << "\n";
			}

			if (mUsePotLabel && !plot->potLabel().empty())
			{
				out << "Pot Label: " << plot->potLabel() << "\n";
			}

			if (mUseLeafType && !plot->leafType().empty())
			{
				out << "Leaf Type: " << plot->leafType() << "\n";
			}

			out << "\n\n";
		}
	}

	out.close();
}

void cPlotData::write_plot_num_points_file(const std::string& directory)
{
	write_plot_num_points_file(directory, mRootFileName);
}

void cPlotData::write_plot_num_points_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mPlotHeights.empty())
		return;

	std::filesystem::path plot_num_points_file = directory;

	plot_num_points_file /= filename;
	plot_num_points_file.replace_extension("plot_num_points.csv");

	std::ofstream out;
	out.open(plot_num_points_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_plot_num_points_file_by_row(out);
	else
		write_plot_num_points_file_by_column(out);

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

void cPlotData::write_plot_num_points_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& plot : mPlotNumPoints)
	{
		out << "Plot_" << plot.first << ", ";

		const auto& numPoints = plot.second;

		for (auto date = mDates.begin(); date != doy_last; ++date)
		{
			auto doy = date->doy;

			auto points = std::find_if(numPoints.begin(), numPoints.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (points == numPoints.end())
				out << ", ";
			else
				out << points->num_points << ", ";
		}

		auto doy = doy_last->doy;

		auto points = std::find_if(numPoints.begin(), numPoints.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (points == numPoints.end())
			out << "\n";
		else
			out << points->num_points << "\n";
	}
}

void cPlotData::write_plot_num_points_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto plot_last = --(mPlotNumPoints.end());
	for (auto it = mPlotNumPoints.begin(); it != plot_last; ++it)
	{
		out << "Plot_" << it->first << ", ";
	}
	out << "Plot_" << plot_last->first << "\n";

	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mPlotNumPoints.begin(); it != plot_last; ++it)
		{
			const auto& numPoints = it->second;

			auto points = std::find_if(numPoints.begin(), numPoints.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (points == numPoints.end())
				out << ", ";
			else
				out << points->num_points << ", ";
		}

		const auto& numPoints = plot_last->second;

		auto points = std::find_if(numPoints.begin(), numPoints.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (points == numPoints.end())
			out << "\n";
		else
			out << points->num_points << "\n";
	}
}

void cPlotData::write_replicate_num_points_file(const std::string& directory)
{
	write_replicate_num_points_file(directory, mRootFileName);
}

void cPlotData::write_replicate_num_points_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mGroupNumPoints.empty())
		return;

	std::filesystem::path height_file = directory;

	height_file /= filename;
	height_file.replace_extension("num_points.csv");

	std::ofstream out;
	out.open(height_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_replicate_num_points_file_by_row(out);
	else
		write_replicate_num_points_file_by_column(out);

	out.close();
}

void cPlotData::write_replicate_num_points_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "Day of Year, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& group : mGroupNumPoints)
	{
		auto group_num = group.first + 1;

		out << "Group_" << group_num << ", ";

		const auto& num_points = group.second;

		auto last = num_points.end();
		--last;

		for (auto it = num_points.begin(); it != last; ++it)
		{
			out << it->avgNumPoints << ", " << it->stdNumPoints << ", ";
		}
		out << last->avgNumPoints << ", " << last->stdNumPoints << "\n";
	}
}

void cPlotData::write_replicate_num_points_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto group_last = mGroups.size();
	--group_last;

	for (auto i = 0; i < group_last; ++i)
	{
		auto group_num = i + 1;
		out << "Group_" << group_num << " (avg), " << "Group_" << group_num << " (std dev), ";
	}
	++group_last;
	out << "Group_" << group_last << " (avg), " << "Group_" << group_last << " (std dev)\n";

	auto num_points_last = --(mGroupNumPoints.end());

	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mGroupNumPoints.begin(); it != num_points_last; ++it)
		{
			const auto& num_points = it->second;

			auto num_point = std::find_if(num_points.begin(), num_points.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (num_point == num_points.end())
				out << ", , ";
			else
				out << num_point->avgNumPoints << ", " << num_point->stdNumPoints << ", ";
		}

		const auto& num_points = num_points_last->second;

		auto num_point = std::find_if(num_points.begin(), num_points.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (num_point == num_points.end())
			out << ",,\n";
		else
			out << num_point->avgNumPoints << ", " << num_point->stdNumPoints << "\n";
	}
}

void cPlotData::write_plot_height_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& plot : mPlotHeights)
	{
		out << "Plot_" << plot.first << ", ";

		const auto& heights = plot.second;

		for (auto date = mDates.begin(); date != doy_last; ++date)
		{
			auto doy = date->doy;

			auto height = std::find_if(heights.begin(), heights.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (height == heights.end())
				out << ", ";
			else
				out << height->height_mm << ", ";
		}

		auto doy = doy_last->doy;

		auto height = std::find_if(heights.begin(), heights.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (height == heights.end())
			out << "\n";
		else
			out << height->height_mm << "\n";
	}
}

void cPlotData::write_plot_height_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto plot_last = --(mPlotHeights.end());
	for (auto it = mPlotHeights.begin(); it != plot_last; ++it)
	{
		out << "Plot_" << it->first << ", ";
	}
	out << "Plot_" << plot_last->first << "\n";

	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mPlotHeights.begin(); it != plot_last; ++it)
		{
			const auto& heights = it->second;

			auto height = std::find_if(heights.begin(), heights.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (height == heights.end())
				out << ", ";
			else
				out << height->height_mm << ", ";
		}

		const auto& heights = plot_last->second;

		auto height = std::find_if(heights.begin(), heights.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (height == heights.end())
			out << "\n";
		else
			out << height->height_mm << "\n";
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

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "Day of Year, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& group : mGroupHeights)
	{
		auto group_num = group.first + 1;

		out << "Group_" << group_num << ", ";

		const auto& heights = group.second;

		auto last = heights.end();
		--last;

		for (auto it = heights.begin(); it != last; ++it)
		{
			out << it->avgHeight_mm << ", " << it->stdHeight_mm << ", ";
		}
		out << last->avgHeight_mm << ", " << last->stdHeight_mm << "\n";
	}
}

void cPlotData::write_replicate_height_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto group_last = mGroups.size();
	--group_last;
	for (auto i = 0; i < group_last; ++i)
	{
		auto group_num = i + 1;
		out << "Group_" << group_num << " (avg), " << "Group_" << group_num << " (std dev), ";
	}
	out << "Group_" << group_last + 1 << " (avg), " << "Group_" << group_last + 1 << " (std dev)\n";

	auto height_last = --(mGroupHeights.end());

	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mGroupHeights.begin(); it != height_last; ++it)
		{
			const auto& heights = it->second;

			auto height = std::find_if(heights.begin(), heights.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (height == heights.end())
				out << ", , ";
			else
				out << height->avgHeight_mm << ", " << height->stdHeight_mm << ", ";
		}

		const auto& heights = height_last->second;

		auto height = std::find_if(heights.begin(), heights.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (height == heights.end())
			out << ",\n";
		else
			out << height->avgHeight_mm << ", " << height->stdHeight_mm << "\n";
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

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& plot : mPlotBioMasses)
	{
		out << "Plot_" << plot.first << ", ";

		const auto& biomasses = plot.second;

		for (auto date = mDates.begin(); date != doy_last; ++date)
		{
			auto doy = date->doy;

			auto biomass = std::find_if(biomasses.begin(), biomasses.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (biomass == biomasses.end())
				out << ", ";
			else
				out << biomass->biomass << ", ";
		}

		auto doy = doy_last->doy;

		auto biomass = std::find_if(biomasses.begin(), biomasses.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (biomass == biomasses.end())
			out << "\n";
		else
			out << biomass->biomass << "\n";
	}
}

void cPlotData::write_plot_biomass_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto plot_last = --(mPlotBioMasses.end());
	for (auto it = mPlotBioMasses.begin(); it != plot_last; ++it)
	{
		out << "Plot_" << it->first << ", ";
	}
	out << "Plot_" << plot_last->first << "\n";

	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mPlotBioMasses.begin(); it != plot_last; ++it)
		{
			const auto& biomasses = it->second;

			auto biomass = std::find_if(biomasses.begin(), biomasses.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (biomass == biomasses.end())
				out << ", ";
			else
				out << biomass->biomass << ", ";
		}

		const auto& biomasses = plot_last->second;

		auto biomass = std::find_if(biomasses.begin(), biomasses.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (biomass == biomasses.end())
			out << "\n";
		else
			out << biomass->biomass << "\n";
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

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& group : mGroupBioMasses)
	{
		auto group_num = group.first + 1;

		out << "Group_" << group_num << ", ";

		const auto& biomasses = group.second;

		auto last = biomasses.end();
		--last;

		for (auto it = biomasses.begin(); it != last; ++it)
		{
			out << it->avgBioMass << ", " << it->stdBioMass << ", ";
		}
		out << last->avgBioMass << ", " << last->stdBioMass << "\n";
	}
}

void cPlotData::write_replicate_biomass_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto group_last = mGroups.size();
	--group_last;
	for (auto i = 0; i < group_last; ++i)
	{
		auto group_num = i + 1;
		out << "Group_" << group_num << " (avg), " << "Group_" << group_num << " (std dev), ";
	}
	++group_last;
	out << "Group_" << group_last << " (avg), " << "Group_"  << group_last << " (std dev)\n";

	auto biomass_last = --(mGroupBioMasses.end());
	auto date_it = mDates.begin();
	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mGroupBioMasses.begin(); it != biomass_last; ++it)
		{
			const auto& biomasses = it->second;

			auto biomass = std::find_if(biomasses.begin(), biomasses.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (biomass == biomasses.end())
				out << ", , ";
			else
				out << biomass->avgBioMass << ", " << biomass->stdBioMass << ", ";
		}

		const auto& biomasses = biomass_last->second;

		auto biomass = std::find_if(biomasses.begin(), biomasses.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (biomass == biomasses.end())
			out << ", \n";
		else
			out << biomass->avgBioMass << ", " << biomass->stdBioMass << "\n";
	}
}


void cPlotData::write_plot_lai_file(const std::string& directory)
{
	write_plot_lai_file(directory, mRootFileName);
}

void cPlotData::write_plot_lai_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mPlotLAIs.empty())
		return;

	std::filesystem::path plot_lai_file = directory;

	plot_lai_file /= filename;
	plot_lai_file.replace_extension("plot_lai.csv");

	std::ofstream out;
	out.open(plot_lai_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_plot_lai_file_by_row(out);
	else
		write_plot_lai_file_by_column(out);

	out.close();
}

void cPlotData::write_plot_lai_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& plot : mPlotLAIs)
	{
		out << "Plot_" << plot.first << ", ";

		const auto& lais = plot.second;

		for (auto date = mDates.begin(); date != doy_last; ++date)
		{
			auto doy = date->doy;

			auto lai = std::find_if(lais.begin(), lais.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (lai == lais.end())
				out << ", ";
			else
				out << lai->lai << ", ";
		}

		auto doy = doy_last->doy;

		auto lai = std::find_if(lais.begin(), lais.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (lai == lais.end())
			out << "\n";
		else
			out << lai->lai << "\n";
	}
}

void cPlotData::write_plot_lai_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto plot_last = --(mPlotLAIs.end());
	for (auto it = mPlotLAIs.begin(); it != plot_last; ++it)
	{
		out << "Plot_" << it->first << ", ";
	}
	out << "Plot_" << plot_last->first << "\n";

	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mPlotLAIs.begin(); it != plot_last; ++it)
		{
			const auto& lais = it->second;

			auto biomass = std::find_if(lais.begin(), lais.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (biomass == lais.end())
				out << ", ";
			else
				out << biomass->lai << ", ";
		}

		const auto& lais = plot_last->second;

		auto lai = std::find_if(lais.begin(), lais.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (lai == lais.end())
			out << "\n";
		else
			out << lai->lai << "\n";
	}
}

void cPlotData::write_replicate_lai_file(const std::string& directory)
{
	write_replicate_lai_file(directory, mRootFileName);
}

void cPlotData::write_replicate_lai_file(const std::string& directory, const std::string& filename)
{
	if (mDates.empty())
		return;

	if (mGroupLAIs.empty())
		return;

	std::filesystem::path lai_file = directory;

	lai_file /= filename;
	lai_file.replace_extension("lai.csv");

	std::ofstream out;
	out.open(lai_file, std::ios::trunc);
	if (!out.is_open())
		return;

	if (mSaveRowMajor)
		write_replicate_lai_file_by_row(out);
	else
		write_replicate_lai_file_by_column(out);

	out.close();
}

void cPlotData::write_replicate_lai_file_by_row(std::ofstream& out)
{
	auto doy_last = --(mDates.end());

	out << "Date, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ", ";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "DayOfYear, ";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ", ";
	}
	out << doy_last->doy << "\n";

	for (const auto& group : mGroupLAIs)
	{
		auto group_num = group.first + 1;

		out << "Group_" << group_num << ", ";

		const auto& lais = group.second;

		auto last = lais.end();
		--last;

		for (auto it = lais.begin(); it != last; ++it)
		{
			out << it->avgLAI << ", " << it->stdLAI << ", ";
		}
		out << last->avgLAI << ", " << last->stdLAI << "\n";
	}
}

void cPlotData::write_replicate_lai_file_by_column(std::ofstream& out)
{
	out << "Date, " << "DayOfYear, ";

	auto group_last = mGroups.size();
	--group_last;
	for (auto i = 0; i < group_last; ++i)
	{
		auto group_num = i + 1;
		out << "Group_" << group_num << " (avg), " << "Group_" << group_num << " (std dev), ";
	}
	++group_last;
	out << "Group_" << group_last << " (avg), " << "Group_" << group_last << " (std dev)\n";

	auto lai_last = --(mGroupLAIs.end());
	auto date_it = mDates.begin();
	for (auto date_it = mDates.begin(); date_it != mDates.end(); ++date_it)
	{
		out << date_it->month << "/" << date_it->day << "/" << date_it->year << ", ";
		out << date_it->doy << ", ";

		auto doy = date_it->doy;

		for (auto it = mGroupLAIs.begin(); it != lai_last; ++it)
		{
			const auto& lais = it->second;

			auto lai = std::find_if(lais.begin(), lais.end(), [doy](const auto& h)
				{
					return h.doy == doy;
				}
			);

			if (lai == lais.end())
				out << ", , ";
			else
				out << lai->avgLAI << ", " << lai->stdLAI << ", ";
		}

		const auto& lais = lai_last->second;

		auto lai = std::find_if(lais.begin(), lais.end(), [doy](const auto& h)
			{
				return h.doy == doy;
			}
		);

		if (lai == lais.end())
			out << ", \n";
		else
			out << lai->avgLAI << ", " << lai->stdLAI << "\n";
	}
}

void cPlotData::clearGroups()
{
	for (auto& group : mGroups)
	{
		group.clear();
	}

	mGroups.clear();
}

void cPlotData::splitIntoGroups()
{
	if (mpPlots.empty())
		return;

	if (!mUseEvent && !mUseSpecies && !mUseCultivar && !mUseTreatment && !mUseConstructName
		&& !mUsePotLabel && !mUseSeedGeneration && !mUseCopyNumber && !mUseLeafType)
		return;

	if (!mGroups.empty())
		clearGroups();

	auto plots = mpPlots;

	while (!plots.empty())
	{
		if (plots.size() == 1)
		{
			std::vector<cPlotMetaData*> group;
			group.push_back(plots[0]);
			mGroups.push_back(group);
		}
		else
		{
			std::vector<cPlotMetaData*> group;
			auto* ref = plots[0];

			for (auto it = plots.begin(); it != plots.end();)
			{
				if (inGroup(ref, *it))
				{
					group.push_back(*it);
					it = plots.erase(it);
				}
				else
					++it;
			}

			mGroups.push_back(group);
		}
	}
}

bool cPlotData::inGroup(cPlotMetaData* pPlotInfo1, cPlotMetaData* pPlotInfo2)
{
	bool group = false;
	group |= mUseEvent ? pPlotInfo1->event() == pPlotInfo2->event() : false;
	group |= mUseSpecies ? pPlotInfo1->species() == pPlotInfo2->species() : false;
	group |= mUseCultivar ? pPlotInfo1->cultivar() == pPlotInfo2->cultivar() : false;
	group |= mUseConstructName ? pPlotInfo1->constructName() == pPlotInfo2->constructName() : false;
	group |= mUsePotLabel ? pPlotInfo1->potLabel() == pPlotInfo2->potLabel() : false;
	group |= mUseSeedGeneration ? pPlotInfo1->seedGeneration() == pPlotInfo2->seedGeneration() : false;
	group |= mUseCopyNumber ? pPlotInfo1->copyNumber() == pPlotInfo2->copyNumber() : false;
	group |= mUseLeafType ? pPlotInfo1->leafType() == pPlotInfo2->leafType() : false;

	if (mUseTreatment)
	{
	}

	return group;
}
