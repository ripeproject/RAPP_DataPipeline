
#include "ExperimentInfoFromJson.hpp"

#include "DateTimeUtils.hpp"

#include <cbdf/ExperimentInfo.hpp>

namespace
{
	std::tuple<std::string, std::string, std::string> date_split(const std::string& date)
	{
		std::array<std::string, 3> results;
		int slot = 0;
		std::size_t startPos = 0;
		std::size_t lastPos = date.find('/');
		while ((lastPos != std::string::npos) && (slot < 2))
		{
			results[slot++] = date.substr(startPos, lastPos - startPos);
			startPos = lastPos + 1;
			lastPos = date.find('/', startPos);
		}

		results[slot++] = date.substr(startPos, date.size() - startPos);

		return { results[0], results[1], results[2] };
	}
}


void cExperimentInfoFromJson::clear()
{
	mTitle.clear();
//	mExperimentDoc.clear();
	mComments.clear();
	
	mPrincipalInvestigator.clear();
	mResearchers.clear();
	
	mSpecies.clear();
	mCultivar.clear();
	mConstruct.clear();

	mEventNumbers.clear();
	mTreatments.clear();

	mFieldDesign.clear();
	mPermit.clear();

	mPlantingDate.reset();
	mHarvestDate.reset();
}

bool cExperimentInfoFromJson::operator!=(const cExperimentInfo& rhs) const
{
	return (mTitle != rhs.title()) ||
		(mPrincipalInvestigator != rhs.principalInvestigator()) ||
		(mResearchers != rhs.researchers()) ||
		(mComments != rhs.comments()) ||
		(mSpecies != rhs.species()) ||
		(mCultivar != rhs.cultivar()) ||
		(mEventNumbers != rhs.eventNumbers()) ||
		(mConstruct != rhs.construct()) ||
		(mTreatments != rhs.treatments()) ||
		(mFieldDesign != rhs.fieldDesign()) ||
		(mPermit != rhs.permit());
}

bool cExperimentInfoFromJson::operator==(const cExperimentInfo& rhs) const
{
	return !operator!=(rhs);
}

const std::string& cExperimentInfoFromJson::title() const
{
	return mTitle;
}

/*
const std::string& cExperimentInfoFromJson::experimentDoc() const
{
	return mExperimentDoc;
}
*/

const std::vector<std::string>& cExperimentInfoFromJson::comments() const
{
	return mComments;
}

const std::string& cExperimentInfoFromJson::principalInvestigator() const
{
	return mPrincipalInvestigator;
}

const std::vector<std::string>& cExperimentInfoFromJson::researchers() const
{
	return mResearchers;
}

const std::string& cExperimentInfoFromJson::species() const
{
	return mSpecies;
}

const std::string& cExperimentInfoFromJson::cultivar() const
{
	return mCultivar;
}

const std::string& cExperimentInfoFromJson::construct() const
{
	return mConstruct;
}

const std::vector<std::string>& cExperimentInfoFromJson::eventNumbers() const
{
	return mEventNumbers;
}

const std::vector<std::string>& cExperimentInfoFromJson::treatments() const
{
	return mTreatments;
}

const std::string& cExperimentInfoFromJson::fieldDesign() const
{
	return mFieldDesign;
}

const std::string& cExperimentInfoFromJson::permit() const
{
	return mPermit;
}

const std::optional<nExpTypes::sDateDoy_t>& cExperimentInfoFromJson::plantingDate() const
{
	return mPlantingDate;
}

const std::optional<nExpTypes::sDateDoy_t>& cExperimentInfoFromJson::harvestDate() const
{
	return mHarvestDate;
}

/**********************************************************
* Parsing
***********************************************************/
void cExperimentInfoFromJson::parse(const std::string& jdoc)
{
	try
	{
		nlohmann::json jsonDoc = nlohmann::json::parse(jdoc, nullptr, false, true);
		parse(jsonDoc);
	}
	catch (const nlohmann::json::parse_error& e)
	{
	}
	catch (const std::exception& e)
	{
	}
}

void cExperimentInfoFromJson::parse(const nlohmann::json& jdoc)
{
	using namespace nlohmann;

	mExperimentDoc = to_string(jdoc);

	if (jdoc.contains("experiment name"))
		mTitle = jdoc["experiment name"];
	else
		mTitle = jdoc["experiment_name"];

	if (jdoc.contains("principal investigator"))
	{
		mPrincipalInvestigator = jdoc["principal investigator"];
	}

	if (jdoc.contains("researcher"))
	{
		mResearchers.push_back(jdoc["researcher"]);
	}

	if (jdoc.contains("researchers"))
	{
		auto researchers = jdoc["researchers"];
		for (auto it = researchers.begin(); it != researchers.end(); ++it)
			mResearchers.push_back(*it);
	}

	if (jdoc.contains("species"))
	{
		mSpecies = jdoc["species"];
	}

	if (jdoc.contains("cultivar"))
	{
		mCultivar = jdoc["cultivar"];
	}

	if (jdoc.contains("permit info"))
	{
		mPermit = jdoc["permit info"];
	}

	if (jdoc.contains("construct"))
	{
		mConstruct = jdoc["construct"];
	}

	if (jdoc.contains("event number"))
	{
		mEventNumbers.push_back(jdoc["event number"]);
	}

	if (jdoc.contains("event numbers"))
	{
		auto event_numbers = jdoc["event numbers"];
		for (auto it = event_numbers.begin(); it != event_numbers.end(); ++it)
			mEventNumbers.push_back(*it);
	}

	if (jdoc.contains("field design"))
	{
		mFieldDesign = jdoc["field design"];
	}

	if (jdoc.contains("treatment"))
	{
		mTreatments.push_back(jdoc["treatment"]);
	}

	if (jdoc.contains("treatments"))
	{
		auto treatments = jdoc["treatments"];
		if (treatments.is_string())
		{
			mTreatments.push_back(treatments);
		}
		else if (treatments.is_array())
		{
			for (auto it = treatments.begin(); it != treatments.end(); ++it)
				mTreatments.push_back(*it);
		}
	}

	if (jdoc.contains("comment"))
	{
		mComments.push_back(jdoc["comment"]);
	}

	if (jdoc.contains("comments"))
	{
		auto comments = jdoc["comments"];
		if (comments.is_string())
		{
			mComments.push_back(comments);
		}
		else if (comments.is_array())
		{
			for (auto it = comments.begin(); it != comments.end(); ++it)
				mComments.push_back(*it);
		}
	}

	std::string month;
	std::string day;
	std::string year;

	if (jdoc.contains("planting date (m/d/y)"))
	{
		std::string date_string = jdoc["planting date (m/d/y)"];
		std::tie(month, day, year) = date_split(date_string);

		nExpTypes::sDateDoy_t date;
		date.month = std::stoi(month);
		date.day = std::stoi(day);
		date.year = std::stoi(year);
		date.doy = nDateUtils::to_day_of_year(month, day, year);

		mPlantingDate = date;
	}

	if (jdoc.contains("planting date (d/m/y)"))
	{
		std::string date_string = jdoc["planting date (d/m/y)"];
		std::tie(day, month, year) = date_split(date_string);

		nExpTypes::sDateDoy_t date;
		date.month = std::stoi(month);
		date.day = std::stoi(day);
		date.year = std::stoi(year);
		date.doy = nDateUtils::to_day_of_year(month, day, year);

		mPlantingDate = date;
	}

	if (jdoc.contains("planting date (y/m/d)"))
	{
		std::string date_string = jdoc["planting date (y/m/d)"];
		std::tie(year, month, day) = date_split(date_string);

		nExpTypes::sDateDoy_t date;
		date.month = std::stoi(month);
		date.day = std::stoi(day);
		date.year = std::stoi(year);
		date.doy = nDateUtils::to_day_of_year(month, day, year);

		mPlantingDate = date;
	}

	if (jdoc.contains("target harvest date (m/d/y)"))
	{
		std::string date_string = jdoc["target harvest date (m/d/y)"];
		std::tie(month, day, year) = date_split(date_string);

		nExpTypes::sDateDoy_t date;
		date.month = std::stoi(month);
		date.day = std::stoi(day);
		date.year = std::stoi(year);
		date.doy = nDateUtils::to_day_of_year(month, day, year);

		mHarvestDate = date;
	}

	if (jdoc.contains("target harvest date (d/m/y)"))
	{
		std::string date_string = jdoc["target harvest date (d/m/y)"];
		std::tie(day, month, year) = date_split(date_string);

		nExpTypes::sDateDoy_t date;
		date.month = std::stoi(month);
		date.day = std::stoi(day);
		date.year = std::stoi(year);
		date.doy = nDateUtils::to_day_of_year(month, day, year);

		mHarvestDate = date;
	}

	if (jdoc.contains("target harvest date (y/m/d)"))
	{
		std::string date_string = jdoc["target harvest date (y/m/d)"];
		std::tie(year, month, day) = date_split(date_string);

		nExpTypes::sDateDoy_t date;
		date.month = std::stoi(month);
		date.day = std::stoi(day);
		date.year = std::stoi(year);
		date.doy = nDateUtils::to_day_of_year(month, day, year);

		mHarvestDate = date;
	}
}


