

#include "ExperimentInfoRepairParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>



cExperimentInfoRepairParser::cExperimentInfoRepairParser()
{
	mSerializer.setBufferCapacity(256 * 1024 * 1024);
}

cExperimentInfoRepairParser::~cExperimentInfoRepairParser()
{}

void cExperimentInfoRepairParser::attach(cBlockDataFileWriter* pDataFile)
{
	mSerializer.attach(pDataFile);
}

cBlockDataFileWriter* cExperimentInfoRepairParser::detach()
{
	return mSerializer.detach();
}

void cExperimentInfoRepairParser::onBeginHeader()
{
	if (mSerializer)
	{
		return;
	}

	mResearchers.clear();
	mComments.clear();
	mEventNumbers.clear();
	mTreatments.clear();

	mPlantingDate.reset();
	mHarvestDate.reset();

	mFileDate.reset();
	mFileTime.reset();

	mDayOfYear.reset();

	mStartTime.reset();
	mEndTime.reset();
}

void cExperimentInfoRepairParser::onEndOfHeader()
{
	if (mSerializer)
		mSerializer.writeEndOfHeader();
}

void cExperimentInfoRepairParser::onBeginFooter()
{
	if (mSerializer)
		mSerializer.writeBeginFooter();
}

void cExperimentInfoRepairParser::onEndOfFooter()
{
	if (mSerializer)
		mSerializer.writeEndOfFooter();
}

void cExperimentInfoRepairParser::onTitle(const std::string& title)
{
	mTitle = title;
}

void cExperimentInfoRepairParser::onPrincipalInvestigator(const std::string& investigator)
{
	mPrincipalInvestigator = investigator;
}

void cExperimentInfoRepairParser::onBeginResearcherList()
{
	mResearchers.clear();
}

void cExperimentInfoRepairParser::onEndOfResearcherList()
{}

void cExperimentInfoRepairParser::onResearcher(const std::string& researcher)
{
	mResearchers.push_back(researcher);
}

void cExperimentInfoRepairParser::onSpecies(const std::string& species)
{
	mSpecies = species;
}

void cExperimentInfoRepairParser::onCultivar(const std::string& cultivar)
{
	mCultivar = cultivar;
}

void cExperimentInfoRepairParser::onExperimentDoc(const std::string& doc)
{
	mExperimentDoc = doc;
}

void cExperimentInfoRepairParser::onPermitInfo(const std::string& permit)
{
	mPermit = permit;
}

void cExperimentInfoRepairParser::onBeginTreatmentList()
{
	mTreatments.clear();
}

void cExperimentInfoRepairParser::onEndOfTreatmentList()
{}

void cExperimentInfoRepairParser::onTreatment(const std::string& treatment)
{
	mTreatments.push_back(treatment);
}

void cExperimentInfoRepairParser::onConstructName(const std::string& name)
{
	mConstruct = name;
}

void cExperimentInfoRepairParser::onBeginEventNumberList()
{
	mEventNumbers.clear();
}

void cExperimentInfoRepairParser::onEndOfEventNumberList()
{}

void cExperimentInfoRepairParser::onEventNumber(const std::string& event_num)
{
	mEventNumbers.push_back(event_num);
}

void cExperimentInfoRepairParser::onFieldDesign(const std::string& design)
{
	mFieldDesign = design;
}

void cExperimentInfoRepairParser::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	nExpTypes::sDateDoy_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.doy = doy;

	mPlantingDate = date;
}

void cExperimentInfoRepairParser::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	nExpTypes::sDateDoy_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.doy = doy;

	mHarvestDate = date;
}

void cExperimentInfoRepairParser::onBeginCommentList()
{
	mComments.clear();
}

void cExperimentInfoRepairParser::onEndOfCommentList()
{}

void cExperimentInfoRepairParser::onComment(const std::string& comments)
{
	mComments.push_back(comments);
}

void cExperimentInfoRepairParser::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day)
{
	if (mSerializer)
		mSerializer.writeFileDate(year, month, day);
}

void cExperimentInfoRepairParser::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds)
{
	if (mSerializer)
		mSerializer.writeFileTime(hour, minute, seconds);
}

void cExperimentInfoRepairParser::onDayOfYear(std::uint16_t day_of_year)
{
	if (mSerializer)
		mSerializer.writeDayOfYear(day_of_year);
}

void cExperimentInfoRepairParser::onBeginSensorList()
{
	if (mSerializer)
		mSerializer.writeBeginSensorList();
}

void cExperimentInfoRepairParser::onEndOfSensorList()
{
	if (mSerializer)
		mSerializer.writeEndOfSensorList();
}

void cExperimentInfoRepairParser::onSensorBlockInfo(uint16_t class_id, const std::string& name)
{
	if (mSerializer)
		mSerializer.writeSensorBlockInfo(class_id, name);
}

void cExperimentInfoRepairParser::onStartTime(sExperimentTime_t time)
{
	if (mSerializer)
		mSerializer.startTime(time.year, time.month, time.day,
			time.hour, time.minutes, time.seconds);
}

void cExperimentInfoRepairParser::onEndTime(sExperimentTime_t time)
{
	if (mSerializer)
		mSerializer.endTime(time.year, time.month, time.day,
			time.hour, time.minutes, time.seconds);
}

void cExperimentInfoRepairParser::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
	if (mSerializer)
		mSerializer.startRecordingTimestamp(timestamp_ns);
}

void cExperimentInfoRepairParser::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
	if (mSerializer)
		mSerializer.endRecordingTimestamp(timestamp_ns);
}

void cExperimentInfoRepairParser::onHeartbeatTimestamp(uint64_t timestamp_ns)
{
	if (mSerializer)
		mSerializer.heartbeatTimestamp(timestamp_ns);
}







