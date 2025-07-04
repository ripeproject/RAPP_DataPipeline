
#pragma once

#include <cbdf/ExperimentInfoTypes.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <optional>
#include <vector>
#include <memory>

//Forward Declaration
class cExperimentInfo;

class cExperimentInfoFromJson
{
public:
	cExperimentInfoFromJson() = default;

	const std::string& measurementTitle() const;
	const std::string& experimentTitle() const;
	const std::string& experimentDoc() const;
	const std::vector<std::string>& comments() const;

	const std::string& principalInvestigator() const;
	const std::vector<std::string>& researchers() const;

	const std::string& species() const;
	const std::string& cultivar() const;
	const std::string& construct() const;

	const std::vector<std::string>& eventNumbers() const;
	const std::vector<std::string>& treatments() const;

	const std::string& fieldDesign() const;
	const std::string& authorization() const;
	const std::string& permit() const;
	const std::string& trial() const;

	const std::optional<nExpTypes::sDateDoy_t>& plantingDate() const;
	const std::optional<nExpTypes::sDateDoy_t>& harvestDate() const;

	void clear();

	void parse(const nlohmann::json& jdoc);
	void parse(const std::string& jdoc);

	bool operator!=(const cExperimentInfo& rhs) const;
	bool operator==(const cExperimentInfo& rhs) const;

private:
	std::string mMeasurementTitle;
	std::string mExperimentTitle;
	std::string mExperimentDoc;
	std::vector<std::string> mComments;

	std::string mPrincipalInvestigator;
	std::vector<std::string> mResearchers;

	std::string mSpecies;
	std::string mCultivar;
	std::string mConstruct;

	std::vector<std::string> mEventNumbers;
	std::vector<std::string> mTreatments;

	std::string mFieldDesign;
	std::string mAuthorization;
	std::string mPermit;
	std::string mTrial;

	std::optional<nExpTypes::sDateDoy_t> mPlantingDate;
	std::optional<nExpTypes::sDateDoy_t> mHarvestDate;
};

