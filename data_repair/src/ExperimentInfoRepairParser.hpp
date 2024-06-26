/**
 * @file
 * @brief Parser for reading Experiment Info data from a block based data file
 */
#pragma once

#include <cbdf/ExperimentParser.hpp>
#include <cbdf/ExperimentSerializer.hpp>
#include <cbdf/ExperimentInfoTypes.hpp>

#include <string>
#include <optional>
#include <vector>
#include <memory>

class cExperimentInfoRepairParser : public cExperimentParser
{
public:
	explicit cExperimentInfoRepairParser();
	virtual ~cExperimentInfoRepairParser();

	void attach(cBlockDataFileWriter* pDataFile);
	cBlockDataFileWriter* detach();

protected:
	void onBeginHeader() override;
	void onEndOfHeader() override;

	void onBeginFooter() override;
	void onEndOfFooter() override;

	void onTitle(const std::string& title) override;
	void onPrincipalInvestigator(const std::string& investigator) override;

	void onBeginResearcherList() override;
	void onEndOfResearcherList() override;
	void onResearcher(const std::string& researcher) override;

	void onSpecies(const std::string& species) override;
	void onCultivar(const std::string& cultivar) override;
	void onExperimentDoc(const std::string& doc) override;

	void onPermitInfo(const std::string& permit) override;

	void onBeginTreatmentList() override;
	void onEndOfTreatmentList() override;
	void onTreatment(const std::string& treatment) override;

	void onConstructName(const std::string& name) override;

	void onBeginEventNumberList() override;
	void onEndOfEventNumberList() override;
	void onEventNumber(const std::string& event) override;

	void onFieldDesign(const std::string& design) override;
	void onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;
	void onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;

	void onBeginCommentList() override;
	void onEndOfCommentList() override;
	void onComment(const std::string& comments) override;

	void onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) override;
	void onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) override;

	void onDayOfYear(std::uint16_t day_of_year) override;

	void onBeginSensorList() override;
	void onEndOfSensorList() override;
	void onSensorBlockInfo(uint16_t class_id, const std::string& name) override;

	void onStartTime(sExperimentTime_t time) override;
	void onEndTime(sExperimentTime_t time) override;

	void onStartRecordingTimestamp(uint64_t timestamp_ns) override;
	void onEndRecordingTimestamp(uint64_t timestamp_ns) override;
	void onHeartbeatTimestamp(uint64_t timestamp_ns) override;


private:
	std::string mTitle;
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
	std::string mPermit;

	std::optional<nExpTypes::sDateDoy_t> mPlantingDate;
	std::optional<nExpTypes::sDateDoy_t> mHarvestDate;

	std::optional<nExpTypes::sDate_t> mFileDate;
	std::optional<nExpTypes::sTime_t> mFileTime;

	std::optional<std::uint16_t> mDayOfYear;

	std::optional<nExpTypes::sExperimentDateTime_t> mStartTime;
	std::optional<nExpTypes::sExperimentDateTime_t> mEndTime;

private:
	cExperimentSerializer mSerializer;
};


