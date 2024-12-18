
#include "PlotMetaData.hpp"


cPlotMetaData::cPlotMetaData(int plotId) : mID(plotId)
{}

cPlotMetaData::~cPlotMetaData()
{}


int cPlotMetaData::id() const { return mID; }

const std::string& cPlotMetaData::name() const { return mName; }
const std::string& cPlotMetaData::event() const { return mEvent; }
const std::string& cPlotMetaData::description() const { return mDescription; }
const std::string& cPlotMetaData::species() const { return mSpecies; }
const std::string& cPlotMetaData::cultivar() const { return mCultivar; }
const std::string& cPlotMetaData::constructName() const { return mConstructName; }
const std::string& cPlotMetaData::potLabel() const { return mPotLabel; }
const std::string& cPlotMetaData::seedGeneration() const { return mSeedGeneration; }
const std::string& cPlotMetaData::copyNumber() const { return mCopyNumber; }
const std::string& cPlotMetaData::leafType() const { return mLeafType; }


void cPlotMetaData::setPlotName(const std::string& name)
{
	mName = name;
}

void cPlotMetaData::setEvent(const std::string& event)
{
	mEvent = event;
}

void cPlotMetaData::setDescription(const std::string& description)
{
	mDescription = description;
}

void cPlotMetaData::setSpecies(const std::string& species)
{
	mSpecies = species;
}

void cPlotMetaData::setCultivar(const std::string& cultivar)
{
	mCultivar = cultivar;
}

void cPlotMetaData::setConstructName(const std::string& constructName)
{
	mConstructName = constructName;
}

void cPlotMetaData::setPotLabel(const std::string& potLabel)
{
	mPotLabel = potLabel;
}

void cPlotMetaData::setSeedGeneration(const std::string& seedGeneration)
{
	mSeedGeneration = seedGeneration;
}

void cPlotMetaData::setCopyNumber(const std::string& copyNumber)
{
	mCopyNumber = copyNumber;
}

void cPlotMetaData::setLeafType(const std::string& leafType)
{
	mLeafType = leafType;
}

void cPlotMetaData::addTreatment(const std::string& treatment)
{
	mTreatments.push_back(treatment);
}



