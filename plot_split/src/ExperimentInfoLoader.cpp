
#include "ExperimentInfoLoader.hpp"


cExperimentInfoLoader::cExperimentInfoLoader(cExperimentInfo& info)
	: mInfo(info)
{
}

cExperimentInfoLoader::~cExperimentInfoLoader()
{}


void cExperimentInfoLoader::onBeginHeader()
{
	mInfo.clearResearcherList();
	mInfo.clearCommentList();

	mInfo.clearStartRecordingTimestamp();
	mInfo.clearEndRecordingTimestamp();
	mInfo.clearHeartbeatTimestamp();
}

void cExperimentInfoLoader::onEndOfHeader() {}

void cExperimentInfoLoader::onBeginFooter() {}
void cExperimentInfoLoader::onEndOfFooter() {}

void cExperimentInfoLoader::onTitle(const std::string& title)
{
	mInfo.setTitle(title);
}

void cExperimentInfoLoader::onPrincipalInvestigator(const std::string& investigator)
{
	mInfo.setPrincipalInvestigator(investigator);
}

void cExperimentInfoLoader::onBeginResearcherList()
{
	mInfo.clearResearcherList();
}

void cExperimentInfoLoader::onEndOfResearcherList()
{}

void cExperimentInfoLoader::onResearcher(const std::string& researcher)
{
	mInfo.addResearcher(researcher);
}

void cExperimentInfoLoader::onSpecies(const std::string& species)
{
	mInfo.setSpecies(species);
}

void cExperimentInfoLoader::onCultivar(const std::string& cultivar)
{
	mInfo.setCultivar(cultivar);
}

void cExperimentInfoLoader::onExperimentDoc(const std::string& doc)
{
	mInfo.setExperimentDoc(doc);
}

void cExperimentInfoLoader::onPermitInfo(const std::string& permit)
{
	mInfo.setPermitInfo(permit);
}

void cExperimentInfoLoader::onBeginTreatmentList()
{
	mInfo.clearTreatmentList();
}

void cExperimentInfoLoader::onEndOfTreatmentList()
{}

void cExperimentInfoLoader::onTreatment(const std::string& treatment)
{
	mInfo.addTreatment(treatment);
}

void cExperimentInfoLoader::onConstructName(const std::string& name)
{
	mInfo.setConstructName(name);
}

void cExperimentInfoLoader::onBeginEventNumberList()
{
	mInfo.clearEventNumberList();
}

void cExperimentInfoLoader::onEndOfEventNumberList()
{}

void cExperimentInfoLoader::onEventNumber(const std::string& event_num)
{
	mInfo.addEventNumber(event_num);
}

void cExperimentInfoLoader::onFieldDesign(const std::string& design)
{
	mInfo.setFieldDesign(design);
}

void cExperimentInfoLoader::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	mInfo.setPlantingDate(year, month, day, doy);
}

void cExperimentInfoLoader::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	mInfo.setHarvestDate(year, month, day, doy);
}

void cExperimentInfoLoader::onBeginCommentList()
{
	mInfo.clearCommentList();
}

void cExperimentInfoLoader::onEndOfCommentList()
{}

void cExperimentInfoLoader::onComment(const std::string& comments)
{
	mInfo.addComment(comments);
}

void cExperimentInfoLoader::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day)
{
	mInfo.setFileDate(year, month, day);
}

void cExperimentInfoLoader::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) 
{
	mInfo.setFileTime(hour, minute, seconds);
}

void cExperimentInfoLoader::onDayOfYear(std::uint16_t day_of_year)
{
	mInfo.setDayOfYear(day_of_year);
}

void cExperimentInfoLoader::onBeginSensorList()
{
	mInfo.clearSensorList();
}

void cExperimentInfoLoader::onEndOfSensorList() {}
void cExperimentInfoLoader::onSensorBlockInfo(uint16_t class_id, const std::string& name)
{
	mInfo.addSensorBlockInfo(class_id, name);
}

void cExperimentInfoLoader::onStartTime(sExperimentTime_t time)
{
	mInfo.setStartTime(time.year, time.month, time.day,
		time.hour, time.minutes, time.seconds);
}

void cExperimentInfoLoader::onEndTime(sExperimentTime_t time)
{
	mInfo.setEndTime(time.year, time.month, time.day,
		time.hour, time.minutes, time.seconds);
}

void cExperimentInfoLoader::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
	mInfo.addStartRecordingTimestamp(timestamp_ns);
}

void cExperimentInfoLoader::onEndRecordingTimestamp(uint64_t timestamp_ns) 
{
	mInfo.addEndRecordingTimestamp(timestamp_ns);
}

void cExperimentInfoLoader::onHeartbeatTimestamp(uint64_t timestamp_ns)
{
	mInfo.addHeartbeatTimestamp(timestamp_ns);
}

