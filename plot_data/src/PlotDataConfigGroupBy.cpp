
#include "PlotDataConfigGroupBy.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

const char* GROUP_BY_SECTION_NAME = "group by";

cPlotDataConfigGroupBy::cPlotDataConfigGroupBy()
{}

void cPlotDataConfigGroupBy::clear()
{
	mDirty = false;
	mGroupBy.clear();
}

bool cPlotDataConfigGroupBy::empty() const
{
	return mGroupBy.empty();
}

bool cPlotDataConfigGroupBy::isDirty() const
{
	return mDirty;
}

const std::vector<eGroupByTypes>& cPlotDataConfigGroupBy::getGroupByTypes() const
{
	return mGroupBy;
}

void cPlotDataConfigGroupBy::setGroupByTypes(const std::vector<eGroupByTypes>& groupby)
{
	mDirty |= (mGroupBy != groupby);
	mGroupBy = groupby;
}

void cPlotDataConfigGroupBy::addGroupByType(eGroupByTypes groupby)
{
	if (std::find(mGroupBy.begin(), mGroupBy.end(), groupby) == mGroupBy.end())
	{
		mDirty = true;
		mGroupBy.push_back(groupby);
	}
}

void cPlotDataConfigGroupBy::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cPlotDataConfigGroupBy::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains(GROUP_BY_SECTION_NAME))
		return;
		
	auto groupby = jdoc[GROUP_BY_SECTION_NAME];

	for (auto entry : groupby)
	{
		if (entry == "event")
			mGroupBy.push_back(eGroupByTypes::EVENT);
		else if (entry == "species")
			mGroupBy.push_back(eGroupByTypes::SPECIES);
		else if (entry == "cultivar")
			mGroupBy.push_back(eGroupByTypes::CULTIVAR);
		else if (entry == "treatment")
			mGroupBy.push_back(eGroupByTypes::TREATMENT);
		else if (entry == "construct")
			mGroupBy.push_back(eGroupByTypes::CONSTRUCT);
		else if ((entry == "pot_label") || (entry == "pot label"))
			mGroupBy.push_back(eGroupByTypes::POT_LABEL);
		else if ((entry == "seed_generation") || (entry == "seed generation"))
			mGroupBy.push_back(eGroupByTypes::SEED_GENERATION);
		else if ((entry == "copy_number") || (entry == "copy number"))
			mGroupBy.push_back(eGroupByTypes::COPY_NUMBER);
		else if ((entry == "leaf_type") || (entry == "leaf type"))
			mGroupBy.push_back(eGroupByTypes::LEAF_TYPE);
	}
}

void cPlotDataConfigGroupBy::save(nlohmann::json& jdoc)
{
	if (mGroupBy.empty())
		return;

	nlohmann::json groupby;

	for (auto group : mGroupBy)
	{
		switch (group)
		{
		case eGroupByTypes::EVENT:
			groupby.push_back("event");
			break;
		case eGroupByTypes::SPECIES:
			groupby.push_back("species");
			break;
		case eGroupByTypes::CULTIVAR:
			groupby.push_back("cultivar");
			break;
		case eGroupByTypes::TREATMENT:
			groupby.push_back("treatment");
			break;
		case eGroupByTypes::CONSTRUCT:
			groupby.push_back("construct");
			break;
		case eGroupByTypes::POT_LABEL:
			groupby.push_back("pot_label");
			break;
		case eGroupByTypes::SEED_GENERATION:
			groupby.push_back("seed_generation");
			break;
		case eGroupByTypes::COPY_NUMBER:
			groupby.push_back("copy_number");
			break;
		case eGroupByTypes::LEAF_TYPE:
			groupby.push_back("leaf_type");
			break;
		default:
			break;
		}
	}

	mDirty = false;

	jdoc[GROUP_BY_SECTION_NAME] = groupby;
}


