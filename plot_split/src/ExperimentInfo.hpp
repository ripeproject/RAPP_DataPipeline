
#pragma once

#include "ExperimentInfoTypes.hpp"

#include <string>
#include <optional>
#include <vector>

// Forward Declarations

class cExperimentInfo
{
public:
	cExperimentInfo() = default;
	virtual ~cExperimentInfo() = default;

	const std::string& title() const;
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
	const std::string& permit() const;

	const std::optional<nExpTypes::sDateDoy_t>& plantingDate() const;
	const std::optional<nExpTypes::sDateDoy_t>& harvestDate() const;

	const std::optional<nExpTypes::sDate_t>& fileDate() const;
	const std::optional<nExpTypes::sTime_t>& fileTime() const;

	const std::optional<std::uint16_t>& dayOfYear() const;

	const std::optional<nExpTypes::sExperimentDateTime_t>& startTime() const;
	const std::optional<nExpTypes::sExperimentDateTime_t>& endTime() const;

	const std::vector<std::string>& sensors() const;

	const std::vector<uint64_t>& startRecordingTimestamps() const;
	const std::vector<uint64_t>& endRecordingTimestamps() const;
	const std::vector<uint64_t>& heartbeatTimestamps() const;

	void clear();

protected:
	void setTitle(const std::string& title);
	void setPrincipalInvestigator(const std::string& investigator);

	void clearResearcherList();
	void addResearcher(const std::string& researcher);

	void setSpecies(const std::string& species);
	void setCultivar(const std::string& cultivar);
	void setExperimentDoc(const std::string& doc);

	void setPermitInfo(const std::string& permit);

	void clearTreatmentList();
	void addTreatment(const std::string& treatment);

	void setConstructName(const std::string& name);

	void clearEventNumberList();
	void addEventNumber(const std::string& event_num);

	void setFieldDesign(const std::string& design);
	void setPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy);
	void setHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy);

	void clearCommentList();
	void addComment(const std::string& comments);

	void setFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day);
	void setFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds);

	void setDayOfYear(std::uint16_t day_of_year);

	void clearSensorList();
	void addSensorBlockInfo(uint16_t class_id, const std::string& name);

	void setStartTime(std::uint16_t year, std::uint8_t month, std::uint8_t day,
						std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds);

	void setEndTime(std::uint16_t year, std::uint8_t month, std::uint8_t day,
					std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds);

	void clearStartRecordingTimestamp();
	void addStartRecordingTimestamp(uint64_t timestamp_ns);

	void clearEndRecordingTimestamp();
	void addEndRecordingTimestamp(uint64_t timestamp_ns);

	void clearHeartbeatTimestamp();
	void addHeartbeatTimestamp(uint64_t timestamp_ns);

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

	std::vector<std::string> mSensors;

	std::vector<uint64_t> mStartRecordingTimestamps;
	std::vector<uint64_t> mEndRecordingTimestamps;
	std::vector<uint64_t> mHeartbeatTimestamps;

	friend class cExperimentInfoLoader;
};

