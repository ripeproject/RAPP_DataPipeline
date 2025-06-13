

#include "ExperimentInfoRepairParser.hpp"
#include "ParserExceptions.hpp"

#include "ExperimentInfoFromJson.hpp"

#include <stdexcept>


namespace
{
	bool update(std::string& value, const std::string& default_value)
	{
		if (default_value.empty())
			return false;

		if (value.empty())
		{
			value = default_value;
			return true;
		}

		return false;
	}

	bool update(std::vector<std::string>& value, const std::vector<std::string>& default_value)
	{
		if (default_value.empty())
			return false;

		if (value.empty())
		{
			value = default_value;
			return true;
		}

		return false;
	}

	bool update(std::optional<nExpTypes::sDateDoy_t>& value, const std::optional<nExpTypes::sDateDoy_t>& default_value)
	{
		if (!default_value.has_value())
			return false;

		if (!value.has_value())
		{
			value = default_value;
			return true;
		}

		return false;
	}
	
}


cExperimentInfoRepairParser::cExperimentInfoRepairParser()
{
	mSerializer.setBufferCapacity(256 * 1024 * 1024);
}

cExperimentInfoRepairParser::~cExperimentInfoRepairParser()
{}

const std::string& cExperimentInfoRepairParser::experimentDoc() const
{
	return mExperimentDoc;
}

void cExperimentInfoRepairParser::attach(cBlockDataFileWriter* pDataFile)
{
	mSerializer.attach(pDataFile);
}

cBlockDataFileWriter* cExperimentInfoRepairParser::detach()
{
	return mSerializer.detach();
}

void cExperimentInfoRepairParser::setReferenceInfo(const cExperimentInfoFromJson& info)
{
	mNeedsUpdating |= update(mMeasurementTitle, info.measurementTitle());
	mNeedsUpdating |= update(mExperimentTitle, info.experimentTitle());
	mNeedsUpdating |= update(mExperimentDoc, info.experimentDoc());
	mNeedsUpdating |= update(mComments, info.comments());
	mNeedsUpdating |= update(mPrincipalInvestigator, info.principalInvestigator());
	mNeedsUpdating |= update(mResearchers, info.researchers());
	mNeedsUpdating |= update(mSpecies, info.species());
	mNeedsUpdating |= update(mCultivar, info.cultivar());
	mNeedsUpdating |= update(mConstruct, info.construct());
	mNeedsUpdating |= update(mEventNumbers, info.eventNumbers());
	mNeedsUpdating |= update(mTreatments, info.treatments());
	mNeedsUpdating |= update(mFieldDesign, info.fieldDesign());
	mNeedsUpdating |= update(mPermit, info.permit());
	mNeedsUpdating |= update(mPlantingDate, info.plantingDate());
	mNeedsUpdating |= update(mHarvestDate, info.harvestDate());
}

void cExperimentInfoRepairParser::onBeginHeader()
{
	if (mSerializer && mNeedsUpdating)
	{
		mSerializer.writeBeginHeader();

		if (!mExperimentTitle.empty())
			mSerializer.writeExperimentTitle(mExperimentTitle);

		if (!mMeasurementTitle.empty())
			mSerializer.writeMeasurementTitle(mMeasurementTitle);

		if (!mComments.empty())
			mSerializer.writeComments(mComments);

		if (!mExperimentDoc.empty())
			mSerializer.writeExperimentDoc(mExperimentDoc);

		if (!mPrincipalInvestigator.empty())
			mSerializer.writePrincipalInvestigator(mPrincipalInvestigator);

		if (!mResearchers.empty())
			mSerializer.writeResearchers(mResearchers);
		
		if (!mSpecies.empty())
			mSerializer.writeSpecies(mSpecies);
		
		if (!mCultivar.empty())
			mSerializer.writeCultivar(mCultivar);
		
		if (!mConstruct.empty())
			mSerializer.writeConstructName(mConstruct);

		if (!mEventNumbers.empty())
			mSerializer.writeEventNumbers(mEventNumbers);
		
		if (!mTreatments.empty())
			mSerializer.writeTreatments(mTreatments);

		if (!mFieldDesign.empty())
			mSerializer.writeFieldDesign(mFieldDesign);
		
		if (!mPermit.empty())
			mSerializer.writePermitInfo(mPermit);

		if (mPlantingDate.has_value())
		{
			auto date = mPlantingDate.value();
			mSerializer.writePlantingDate(date.year, date.month, date.day, date.doy);
		}

		if (mHarvestDate.has_value())
		{
			auto date = mHarvestDate.value();
			mSerializer.writeHarvestDate(date.year, date.month, date.day, date.doy);
		}

		mSerializer.writeEndOfHeader();

		return;
	}

	mExperimentTitle.clear();
	mMeasurementTitle.clear();
	mExperimentDoc.clear();

	mResearchers.clear();
	mComments.clear();
	mEventNumbers.clear();
	mTreatments.clear();

	mPlantingDate.reset();
	mHarvestDate.reset();
}

void cExperimentInfoRepairParser::onEndOfHeader()
{
	if (mSerializer && !mNeedsUpdating)
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

void cExperimentInfoRepairParser::onExperimentTitle(const std::string& title)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeExperimentTitle(title);

		return;
	}

	mExperimentTitle = title;
}

void cExperimentInfoRepairParser::onMeasurementTitle(const std::string& title)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeMeasurementTitle(title);

		return;
	}

	mMeasurementTitle = title;
}

void cExperimentInfoRepairParser::onPrincipalInvestigator(const std::string& investigator)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writePrincipalInvestigator(investigator);

		return;
	}

	mPrincipalInvestigator = investigator;
}

void cExperimentInfoRepairParser::onBeginResearcherList()
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeBeginResearchers();
		return;
	}

	mResearchers.clear();
}

void cExperimentInfoRepairParser::onEndOfResearcherList()
{
	if (mSerializer && !mNeedsUpdating)
	{
		mSerializer.writeEndOfResearchers();
	}
}

void cExperimentInfoRepairParser::onResearcher(const std::string& researcher)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeResearcher(researcher);

		return;
	}

	mResearchers.push_back(researcher);
}

void cExperimentInfoRepairParser::onSpecies(const std::string& species)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeSpecies(species);

		return;
	}

	mSpecies = species;
}

void cExperimentInfoRepairParser::onCultivar(const std::string& cultivar)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeCultivar(cultivar);

		return;
	}

	mCultivar = cultivar;
}

void cExperimentInfoRepairParser::onExperimentDoc(const std::string& doc)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeExperimentDoc(doc);

		return;
	}

	mExperimentDoc = doc;
}

void cExperimentInfoRepairParser::onPermitInfo(const std::string& permit)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writePermitInfo(permit);

		return;
	}

	mPermit = permit;
}

void cExperimentInfoRepairParser::onBeginTreatmentList()
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeBeginTreatments();
		
		return;
	}

	mTreatments.clear();
}

void cExperimentInfoRepairParser::onEndOfTreatmentList()
{
	if (mSerializer && !mNeedsUpdating)
	{
		mSerializer.writeEndOfTreatments();
	}

}

void cExperimentInfoRepairParser::onTreatment(const std::string& treatment)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeTreatment(treatment);

		return;
	}

	mTreatments.push_back(treatment);
}

void cExperimentInfoRepairParser::onConstructName(const std::string& name)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeConstructName(name);

		return;
	}

	mConstruct = name;
}

void cExperimentInfoRepairParser::onBeginEventNumberList()
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeBeginEventNumbers();

		return;
	}

	mEventNumbers.clear();
}

void cExperimentInfoRepairParser::onEndOfEventNumberList()
{
	if (mSerializer && !mNeedsUpdating)
		mSerializer.writeEndOfEventNumbers();
}

void cExperimentInfoRepairParser::onEventNumber(const std::string& event_num)
{
	if (mSerializer && !mNeedsUpdating)
	{
		if (!mNeedsUpdating)
			mSerializer.writeEventNumber(event_num);

		return;
	}

	mEventNumbers.push_back(event_num);
}

void cExperimentInfoRepairParser::onFieldDesign(const std::string& design)
{
	if (mSerializer && !mNeedsUpdating)
	{
		if (!mNeedsUpdating)
			mSerializer.writeFieldDesign(design);

		return;
	}

	mFieldDesign = design;
}

void cExperimentInfoRepairParser::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writePlantingDate(year, month, day, doy);

		return;
	}

	nExpTypes::sDateDoy_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.doy = doy;

	mPlantingDate = date;
}

void cExperimentInfoRepairParser::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeHarvestDate(year, month, day, doy);

		return;
	}


	nExpTypes::sDateDoy_t date;
	date.year = year;
	date.month = month;
	date.day = day;
	date.doy = doy;

	mHarvestDate = date;
}

void cExperimentInfoRepairParser::onBeginCommentList()
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeBeginComments();

		return;
	}

	mComments.clear();
}

void cExperimentInfoRepairParser::onEndOfCommentList()
{
	if (mSerializer && !mNeedsUpdating)
		mSerializer.writeEndOfComments();
}

void cExperimentInfoRepairParser::onComment(const std::string& comments)
{
	if (mSerializer)
	{
		if (!mNeedsUpdating)
			mSerializer.writeComment(comments);

		return;
	}

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

void cExperimentInfoRepairParser::onSensorBlockInfo(uint16_t class_id, const std::string& name, const std::string& instance,
	const std::string& manufacturer, const std::string& model, const std::string& serial_number, uint8_t device_id)
{
	if (mSerializer)
		mSerializer.writeSensorBlockInfo(class_id, name, instance, manufacturer, model, serial_number, device_id);
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







