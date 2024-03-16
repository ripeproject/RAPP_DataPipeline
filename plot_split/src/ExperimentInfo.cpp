
#include "ExperimentInfo.hpp"


void cExperimentInfo::clear()
{
	mTitle.clear();
	mExperimentDoc.clear();
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

	mFileDate.reset();
	mFileTime.reset();

	mDayOfYear.reset();

	mStartTime.reset();
	mEndTime.reset();

	mSensors.clear();

	mStartRecordingTimestamps.clear();
	mEndRecordingTimestamps.clear();
	mHeartbeatTimestamps.clear();
}

const std::string& cExperimentInfo::title() const
{
	return mTitle;
}

const std::string& cExperimentInfo::experimentDoc() const
{
	return mExperimentDoc;
}

const std::vector<std::string>& cExperimentInfo::comments() const
{
	return mComments;
}

const std::string& cExperimentInfo::principalInvestigator() const
{
	return mPrincipalInvestigator;
}

const std::vector<std::string>& cExperimentInfo::researchers() const
{
	return mResearchers;
}

const std::string& cExperimentInfo::species() const
{
	return mSpecies;
}

const std::string& cExperimentInfo::cultivar() const
{
	return mCultivar;
}

const std::string& cExperimentInfo::construct() const
{
	return mConstruct;
}

const std::vector<std::string>& cExperimentInfo::eventNumbers() const
{
	return mEventNumbers;
}

const std::vector<std::string>& cExperimentInfo::treatments() const
{
	return mTreatments;
}

const std::string& cExperimentInfo::fieldDesign() const
{
	return mFieldDesign;
}

const std::string& cExperimentInfo::permit() const
{
	return mPermit;
}

const std::optional<nExpTypes::sDateDoy_t>& cExperimentInfo::plantingDate() const
{
	return mPlantingDate;
}

const std::optional<nExpTypes::sDateDoy_t>& cExperimentInfo::harvestDate() const
{
	return mHarvestDate;
}

const std::optional<nExpTypes::sDate_t>& cExperimentInfo::fileDate() const
{
	return mFileDate;
}

const std::optional<nExpTypes::sTime_t>& cExperimentInfo::fileTime() const
{
	return mFileTime;
}

const std::optional<std::uint16_t>& cExperimentInfo::dayOfYear() const
{
	return mDayOfYear;
}

const std::optional<nExpTypes::sExperimentDateTime_t>& cExperimentInfo::startTime() const
{
	return mStartTime;
}

const std::optional<nExpTypes::sExperimentDateTime_t>& cExperimentInfo::endTime() const
{
	return mEndTime;
}

const std::vector<std::string>& cExperimentInfo::sensors() const
{
	return mSensors;
}

const std::vector<uint64_t>& cExperimentInfo::startRecordingTimestamps() const
{
	return mStartRecordingTimestamps;
}

const std::vector<uint64_t>& cExperimentInfo::endRecordingTimestamps() const
{
	return mEndRecordingTimestamps;
}

const std::vector<uint64_t>& cExperimentInfo::heartbeatTimestamps() const
{
	return mHeartbeatTimestamps;
}

/**********************************************************
* Setters
***********************************************************/

void cExperimentInfo::setTitle(const std::string& title)
{
	mTitle = title;
}

void cExperimentInfo::setPrincipalInvestigator(const std::string& investigator)
{
	mPrincipalInvestigator = investigator;
}

void cExperimentInfo::clearResearcherList()
{
	mResearchers.clear();
}

void cExperimentInfo::addResearcher(const std::string& researcher)
{
	mResearchers.push_back(researcher);
}

void cExperimentInfo::setSpecies(const std::string& species)
{
	mSpecies = species;
}

void cExperimentInfo::setCultivar(const std::string& cultivar)
{
	mCultivar = cultivar;
}

void cExperimentInfo::setExperimentDoc(const std::string& doc)
{
	mExperimentDoc = doc;
}

void cExperimentInfo::setPermitInfo(const std::string& permit)
{
	mPermit = permit;
}

void cExperimentInfo::clearTreatmentList()
{
	mTreatments.clear();
}

void cExperimentInfo::addTreatment(const std::string& treatment)
{
	mTreatments.push_back(treatment);
}

void cExperimentInfo::setConstructName(const std::string& name)
{
	mConstruct = name;
}

void cExperimentInfo::clearEventNumberList()
{
	mEventNumbers.clear();
}

void cExperimentInfo::addEventNumber(const std::string& event)
{
	mEventNumbers.push_back(event);
}

void cExperimentInfo::setFieldDesign(const std::string& design)
{
	mFieldDesign = design;
}

void cExperimentInfo::setPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	nExpTypes::sDateDoy_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.doy = doy;

	mPlantingDate = date;
}

void cExperimentInfo::setHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	mHarvestDate;
}

void cExperimentInfo::clearCommentList()
{
	mComments.clear();
}

void cExperimentInfo::addComment(const std::string& comments)
{
	mComments.push_back(comments);
}

void cExperimentInfo::setFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day)
{
	nExpTypes::sDate_t date;
	date.year = year;
	date.month = month;
	date.day = day;

	mFileDate = date;
}

void cExperimentInfo::setFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds)
{
	nExpTypes::sTime_t time;
	time.hour = hour;
	time.minute = minute;
	time.seconds = seconds;

	mFileTime = time;
}

void cExperimentInfo::setDayOfYear(std::uint16_t day_of_year)
{
	mDayOfYear = day_of_year;
}

void cExperimentInfo::clearSensorList()
{
	mSensors.clear();
}

void cExperimentInfo::addSensorBlockInfo(uint16_t class_id, const std::string& name)
{
	mSensors.push_back(name);
}

void cExperimentInfo::setStartTime(std::uint16_t year, std::uint8_t month, std::uint8_t day,
									std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds)
{
	nExpTypes::sExperimentDateTime_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.hour = hour;
	date.minute = minute;
	date.seconds = seconds;

	mStartTime = date;
}

void cExperimentInfo::setEndTime(std::uint16_t year, std::uint8_t month, std::uint8_t day,
									std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds)
{
	nExpTypes::sExperimentDateTime_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.hour = hour;
	date.minute = minute;
	date.seconds = seconds;

	mEndTime = date;
}

void cExperimentInfo::clearStartRecordingTimestamp()
{
	mStartRecordingTimestamps.clear();
}

void cExperimentInfo::addStartRecordingTimestamp(uint64_t timestamp_ns)
{
	mStartRecordingTimestamps.push_back(timestamp_ns);
}

void cExperimentInfo::clearEndRecordingTimestamp()
{
	mEndRecordingTimestamps.clear();
}

void cExperimentInfo::addEndRecordingTimestamp(uint64_t timestamp_ns)
{
	mEndRecordingTimestamps.push_back(timestamp_ns);
}

void cExperimentInfo::clearHeartbeatTimestamp()
{
	mHeartbeatTimestamps.clear();
}

void cExperimentInfo::addHeartbeatTimestamp(uint64_t timestamp_ns)
{
	mHeartbeatTimestamps.push_back(timestamp_ns);
}

