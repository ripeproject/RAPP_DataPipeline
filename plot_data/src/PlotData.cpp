
#include "PlotData.hpp"

#include "PointCloudTypes.hpp"
#include "Constants.hpp"

//#include <fmt/format.h>

#include <cstdint>
#include <algorithm>
#include <fstream>
#include <filesystem>


cPlotData::~cPlotData()
{
	for (auto plot : mpPlots)
	{
		delete plot;
	}

	mpPlots.clear();


	for (auto& group : mGroups)
	{
/*
		for (auto plot : group)
		{
			delete plot;
		}
*/

		group.clear();
	}

	mGroups.clear();
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

void cPlotData::addPlotData(int id, int doy, double height_mm, double lowerBound_mm, double upperBount_mm)
{
	sPlotHeightData_t data;

	data.doy = doy;
	data.height_mm = height_mm;
	data.lowerBound_mm = lowerBound_mm;
	data.upperBount_mm = upperBount_mm;

	mPlotHeights[id].push_back(data);
}

void cPlotData::write_metadata_file(const std::string& directory)
{
	std::filesystem::path meta_file = directory;

	meta_file /= "test.metadata";

	std::ofstream out;
	out.open(meta_file, std::ios::trunc);
	if (!out.is_open())
		return;

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
	if (mDates.empty())
		return;

	if (mPlotHeights.empty())
		return;

	std::filesystem::path plot_height_file = directory;

	plot_height_file /= "test.plot_heights.csv";

	std::ofstream out;
	out.open(plot_height_file, std::ios::trunc);
	if (!out.is_open())
		return;

	auto doy_last = --(mDates.end());

	out << "\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->month << "/" << it->day << "/" << it->year << ",\t";
	}
	out << doy_last->month << "/" << doy_last->day << "/" << doy_last->year << "\n";

	out << "\t";
	for (auto it = mDates.begin(); it != doy_last; ++it)
	{
		out << it->doy << ",\t";
	}
	out << doy_last->doy << "\n";

/*
	auto last = mPlotHeights.end();
	--last;

	for (auto it = mPlotHeights.begin(); it != last; ++it)
	{
		out << it->second. << ",\t";

	}
*/

	out.close();
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
