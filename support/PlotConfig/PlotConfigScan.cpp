
#include "PlotConfigScan.hpp"

#include <stdexcept>

namespace plot_config
{
	inline int to_date(int month, int day)
	{
		return month * 100 + day;
	}
}

namespace
{
	const uint32_t HAS_SUB_PLOT_MASK = 0x8000'0000;
	const uint32_t SUB_PLOT_FLAG = 0x8000'0000;

	const uint32_t PLOT_MASK = 0x7FFF'FF00;

	const uint32_t SUB_PLOT_MASK = 0x0000'00FF;
}

cPlotConfigScan::cPlotConfigScan()
{}

cPlotConfigScan::~cPlotConfigScan()
{
	mPlots.clear();
}

bool cPlotConfigScan::isDirty() const
{
	for (const auto& plot : mPlots)
	{
		if (plot.isDirty())
			return true;
	}

	return mDirty;
}

const std::string& cPlotConfigScan::getMeasurementName() const
{
	return mMeasurementName;
}

void cPlotConfigScan::setMeasurementName(const std::string& name)
{
	mDirty |= (mMeasurementName != name);
	mMeasurementName = name;
}


std::optional<std::int32_t> cPlotConfigScan::getGroundLevel_mm() const
{
	return mGroundLevel_mm;
}

void cPlotConfigScan::setGroundLevel_mm(std::int32_t ground_level_mm)
{
	mDirty |= (mGroundLevel_mm != ground_level_mm);
	mGroundLevel_mm = ground_level_mm;
}

void cPlotConfigScan::clearGroundLevel()
{
	mDirty |= mGroundLevel_mm.has_value();
	mGroundLevel_mm.reset();
}

bool cPlotConfigScan::hasGroundLevel() const
{
	return mGroundLevel_mm.has_value();
}


bool cPlotConfigScan::empty() const
{
	return mPlots.empty();
}

std::size_t cPlotConfigScan::size() const
{
	return mPlots.size();
}

bool cPlotConfigScan::contains(int32_t id) const
{
	for (const auto& plot : mPlots)
	{
		if (plot.getPlotNumber() == id)
			return true;
	}

	return false;
}

bool cPlotConfigScan::contains(const std::string& name) const
{
	for (const auto& plot : mPlots)
	{
		if (plot.getPlotName() == name)
			return true;
	}

	return false;
}

cPlotConfigScan::const_iterator cPlotConfigScan::find(int32_t id) const
{
	auto it = mPlots.begin();

	for (; it != mPlots.end(); ++it)
	{
		if (it->getPlotNumber() == id)
			break;
	}

	return it;
}

cPlotConfigScan::iterator cPlotConfigScan::find(int32_t id)
{
	auto it = mPlots.begin();

	for (; it != mPlots.end(); ++it)
	{
		if (it->getPlotNumber() == id)
			break;
	}

	return it;
}

cPlotConfigScan::const_iterator cPlotConfigScan::find(const std::string& name) const
{
	auto it = mPlots.begin();

	for (; it != mPlots.end(); ++it)
	{
		if (it->getPlotName() == name)
			break;
	}

	return it;
}

cPlotConfigScan::iterator cPlotConfigScan::find(const std::string& name)
{
	auto it = mPlots.begin();

	for (; it != mPlots.end(); ++it)
	{
		if (it->getPlotName() == name)
			break;
	}

	return it;
}

const cPlotConfigPlotInfo& cPlotConfigScan::front() const { return mPlots.front(); }
cPlotConfigPlotInfo& cPlotConfigScan::front() { return mPlots.front(); }

const cPlotConfigPlotInfo& cPlotConfigScan::back() const { return mPlots.back(); }
cPlotConfigPlotInfo& cPlotConfigScan::back() { return mPlots.back(); }

cPlotConfigScan::iterator cPlotConfigScan::begin() { return mPlots.begin(); }
cPlotConfigScan::iterator cPlotConfigScan::end()   { return mPlots.end(); }

cPlotConfigScan::const_iterator	cPlotConfigScan::begin() const { return mPlots.cbegin(); }
cPlotConfigScan::const_iterator	cPlotConfigScan::end() const   { return mPlots.cend(); }

void cPlotConfigScan::append(const cPlotConfigPlotInfo& info)
{
	mPlots.push_back(info);

	mDirty = true;
}

void cPlotConfigScan::append(const std::vector<cPlotConfigPlotInfo>& info)
{
	for(const auto& entry : info)
		mPlots.push_back(entry);

	mDirty = true;
}

cPlotConfigPlotInfo& cPlotConfigScan::append()
{
	cPlotConfigPlotInfo plot;
	mPlots.push_back(std::move(plot));

	mDirty = true;

	return mPlots.back();
}

void cPlotConfigScan::remove(const cPlotConfigPlotInfo& plot)
{

}

const cPlotConfigPlotInfo& cPlotConfigScan::operator[](int index) const
{
	if (index < 0)
		return mPlots.front();

	if (index >= mPlots.size())
		return mPlots.back();

	return mPlots[index];
}

cPlotConfigPlotInfo& cPlotConfigScan::operator[](int index)
{
	if (index < 0)
		return mPlots.front();

	if (index >= mPlots.size())
		return mPlots.back();

	return mPlots[index];
}

std::vector<cPlotConfigPlotInfo>& cPlotConfigScan::data()
{
	return mPlots;
}

const std::vector<cPlotConfigPlotInfo>& cPlotConfigScan::data() const
{
	return mPlots;
}

void cPlotConfigScan::clearDirtyFlag()
{
	for (auto& plot : mPlots)
		plot.clearDirtyFlag();

	mDirty = false;
}

void cPlotConfigScan::setDirtyFlag(bool dirty)
{
	for (auto& plot : mPlots)
		plot.setDirtyFlag(dirty);

	mDirty = dirty;
}

void cPlotConfigScan::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("measurement_name"))
		mMeasurementName = jdoc["measurement_name"];

	else if (jdoc.contains("measurement name"))
		mMeasurementName = jdoc["measurement name"];

	else if (jdoc.contains("experiment_name"))
		mMeasurementName = jdoc["experiment_name"];

	else if (jdoc.contains("experiment name"))
		mMeasurementName = jdoc["experiment name"];

	if (jdoc.contains("ground_level_mm"))
		mGroundLevel_mm = jdoc["ground_level_mm"];

	if (!jdoc.contains("plots"))
		return;

	const auto& plots = jdoc["plots"];

	for (const auto& entry : plots)
	{
		cPlotConfigPlotInfo plot;

		plot.load(entry);
		
		mPlots.push_back(std::move(plot));
	}
}

nlohmann::json cPlotConfigScan::save()
{
	nlohmann::json scanDoc;

	scanDoc["measurement name"] = mMeasurementName;

	if (mGroundLevel_mm.has_value())
	{
		scanDoc["ground_level_mm"] = mGroundLevel_mm.value();
	}

	nlohmann::json plots;

	for (auto& plot : mPlots)
	{
		plots.push_back(plot.save());
	}

	if (!plots.is_null())
		scanDoc["plots"] = plots;

	mDirty = false;

	return scanDoc;
}

const cPlotConfigPlotInfo& cPlotConfigPlotInfo::operator=(const cPlotConfigPlotInfo& rhs)
{
	mDirty |= rhs.mDirty;

	mPlotNumber = rhs.mPlotNumber;
	mPlotName = rhs.mPlotName;
	mDescription = rhs.mDescription;
	mSpecies = rhs.mSpecies;
	mCultivar = rhs.mCultivar;
	mEvent = rhs.mEvent;
	mConstructName = rhs.mConstructName;
	mPotLabel = rhs.mPotLabel;
	mSeedGeneration = rhs.mSeedGeneration;
	mCopyNumber = rhs.mCopyNumber;
	mTreatments = rhs.mTreatments;
	mLeafType = rhs.mLeafType;

	mCorrections = rhs.mCorrections;
//	mBounds = rhs.mBounds;
//	mIsolationMethod = rhs.mIsolationMethod;

	return *this;
}

void cPlotConfigPlotInfo::clear()
{
	mDirty = false;

	mPlotNumber = 0;
	mPlotName.clear();
	mDescription.clear();
	mSpecies.clear();
	mCultivar.clear();
	mEvent.clear();
	mConstructName.clear();
	mPotLabel.clear();
	mSeedGeneration.clear();
	mCopyNumber.clear();
	mLeafType.clear();
	mTreatments.clear();

	mCorrections.clear();
}

bool cPlotConfigPlotInfo::isDirty() const
{
	return mDirty || mCorrections.isDirty();
}

bool cPlotConfigPlotInfo::empty() const
{
	return mCorrections.empty();
}

uint32_t cPlotConfigPlotInfo::getPlotNumber() const
{
/*
	if (hasSubPlotNumber())
	{
		uint32_t plotNumber = mPlotNumber & PLOT_MASK;
		return (plotNumber >> 8);
	}
*/
		
	return mPlotNumber;
}

/*
bool cPlotConfigPlotInfo::hasSubPlotNumber() const
{
	return (mPlotNumber & HAS_SUB_PLOT_MASK) == SUB_PLOT_FLAG;
}

uint8_t	cPlotConfigPlotInfo::getSubPlotNumber() const
{
	if (hasSubPlotNumber())
	{
		return mPlotNumber & SUB_PLOT_MASK;
	}

	return INVALID_SUB_PLOT_NUMBER;
}
*/

const std::string& cPlotConfigPlotInfo::getPlotName() const
{
	return mPlotName;
}

const std::string& cPlotConfigPlotInfo::getDescription() const
{
	return mDescription;
}

const std::string& cPlotConfigPlotInfo::getEvent() const
{
	return mEvent;
}

const std::string& cPlotConfigPlotInfo::getSpecies() const
{
	return mSpecies;
}

const std::string& cPlotConfigPlotInfo::getCultivar() const
{
	return mCultivar;
}

const std::string& cPlotConfigPlotInfo::getConstructName() const
{
	return mConstructName;
}

const std::string& cPlotConfigPlotInfo::getPotLabel() const
{
	return mPotLabel;
}

const std::string& cPlotConfigPlotInfo::getSeedGeneration() const
{
	return mSeedGeneration;
}

const std::string& cPlotConfigPlotInfo::getCopyNumber() const
{
	return mCopyNumber;
}

const std::string& cPlotConfigPlotInfo::getLeafType() const
{
	return mLeafType;
}

const std::vector<std::string>& cPlotConfigPlotInfo::getTreatments() const
{
	return mTreatments;
}

bool cPlotConfigPlotInfo::contains_point(rfm::rappPoint2D_t point) const
{
	return mCorrections.contains_point(point); // mBounds.contains(point);
}

bool cPlotConfigPlotInfo::contains_point(std::int32_t x_mm, std::int32_t y_mm) const
{
	return mCorrections.contains_point(x_mm, y_mm); // mBounds.contains(x_mm, y_mm);
}

bool cPlotConfigPlotInfo::contains_point(const int date, rfm::rappPoint2D_t point) const
{
	const auto* bounds = getBounds(date);

	if (bounds)
		return bounds->contains(point);

	return false;
}

bool cPlotConfigPlotInfo::contains_point(const int date, std::int32_t x_mm, std::int32_t y_mm) const
{
	const auto* bounds = getBounds(date);

	if (bounds)
		return bounds->contains(x_mm, y_mm);

	return false;
}

bool cPlotConfigPlotInfo::contains_point(const int month, const int day, rfm::rappPoint2D_t point) const
{
	return contains_point(plot_config::to_date(month, day), point);
}

bool cPlotConfigPlotInfo::contains_point(const int month, const int day, std::int32_t x_mm, std::int32_t y_mm) const
{
	return contains_point(plot_config::to_date(month, day), x_mm, y_mm);
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::find(const int date) const
{
	return mCorrections.find(date);
}

cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::find(const int date)
{
	return mCorrections.find(date);
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::find(const int month, const int day) const
{
	return mCorrections.find(month, day);
}

cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::find(const int month, const int day)
{
	return mCorrections.find(month, day);
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::find_exact(const int month, const int day) const
{
	return mCorrections.find(month, day);
}

cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::find_exact(const int month, const int day)
{
	return mCorrections.find(month, day);
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::find_prev(const int date) const
{
	auto it = mCorrections.find(date);

	if (it == mCorrections.begin())
		return it;

	if (mCorrections.begin() == mCorrections.end())
		return it;

	return --it;
}

cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::find_prev(const int date)
{
	auto it = mCorrections.find(date);

	if (it == mCorrections.begin())
		return it;

	if (mCorrections.begin() == mCorrections.end())
		return it;

	return --it;
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::find_prev(const int month, const int day) const
{
	auto it = mCorrections.find(month, day);

	if (it == mCorrections.begin())
		return it;

	if (mCorrections.begin() == mCorrections.end())
		return it;

	return --it;
}

cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::find_prev(const int month, const int day)
{
	auto it = mCorrections.find(month, day);

	if (it == mCorrections.begin())
		return it;

	if (mCorrections.begin() == mCorrections.end())
		return it;

	return --it;
}


bool cPlotConfigPlotInfo::contains(const int date) const
{
	return mCorrections.contains(date);
}

bool cPlotConfigPlotInfo::contains(const int month, const int day) const
{
	return contains(plot_config::to_date(month, day));
}

const cPlotConfigCorrection& cPlotConfigPlotInfo::front() const
{
	if (mCorrections.empty())
		throw std::logic_error("oops");

	return mCorrections.front();
}

cPlotConfigCorrection& cPlotConfigPlotInfo::front()
{
	if (mCorrections.empty())
		throw std::logic_error("oops");

	return mCorrections.front();
}


cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::begin()
{
	return mCorrections.begin();
}

cPlotConfigPlotInfo::iterator cPlotConfigPlotInfo::end()
{
	return mCorrections.end();
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::begin() const
{
	return mCorrections.begin();
}

cPlotConfigPlotInfo::const_iterator	cPlotConfigPlotInfo::end() const
{
	return mCorrections.end();
}

const cPlotConfigBoundary* const cPlotConfigPlotInfo::getBounds(const int date) const
{
	if (mCorrections.empty())
		return nullptr;

	auto it = find(date);
	if (it == mCorrections.end())
		return &(mCorrections.begin()->second.getBounds());

	return &(it->second.getBounds());
}

cPlotConfigBoundary* const cPlotConfigPlotInfo::getBounds(const int date)
{
	if (mCorrections.empty())
		return nullptr;

	auto it = find(date);
	if (it == mCorrections.end())
		return &(mCorrections.begin()->second.getBounds());

	return &(it->second.getBounds());
}

const cPlotConfigBoundary* const cPlotConfigPlotInfo::getBounds(const int month, const int day) const
{
	return getBounds(plot_config::to_date(month, day));
}

cPlotConfigBoundary* const cPlotConfigPlotInfo::getBounds(const int month, const int day)
{
	return getBounds(plot_config::to_date(month, day));
}

const cPlotConfigIsolationMethod* const cPlotConfigPlotInfo::getIsolationMethod(const int date) const
{
	if (mCorrections.empty())
		return nullptr;

	auto it = find(date);
	if (it == mCorrections.end())
		return &(mCorrections.begin()->second.getIsolationMethod());

	return &(it->second.getIsolationMethod());
}

cPlotConfigIsolationMethod* const cPlotConfigPlotInfo::getIsolationMethod(const int date)
{
	if (mCorrections.empty())
		return nullptr;

	auto it = find(date);
	if (it == mCorrections.end())
		return &(mCorrections.begin()->second.getIsolationMethod());

	return &(it->second.getIsolationMethod());
}


const cPlotConfigIsolationMethod* const cPlotConfigPlotInfo::getIsolationMethod(const int month, const int day) const
{
	return getIsolationMethod(plot_config::to_date(month, day));
}

cPlotConfigIsolationMethod* const cPlotConfigPlotInfo::getIsolationMethod(const int month, const int day)
{
	return getIsolationMethod(plot_config::to_date(month, day));
}

const std::vector<cPlotConfigExclusion>* const cPlotConfigPlotInfo::getExclusions(const int date) const
{
	if (mCorrections.empty())
		return nullptr;

	auto it = find(date);
	if (it == mCorrections.end())
		return &(mCorrections.begin()->second.getExclusions());

	return &(it->second.getExclusions());
}

std::vector<cPlotConfigExclusion>* const cPlotConfigPlotInfo::getExclusions(const int date)
{
	if (mCorrections.empty())
		return nullptr;

	auto it = find(date);
	if (it == mCorrections.end())
		return &(mCorrections.begin()->second.getExclusions());

	return &(it->second.getExclusions());
}

const std::vector<cPlotConfigExclusion>* const cPlotConfigPlotInfo::getExclusions(const int month, const int day) const
{
	return getExclusions(plot_config::to_date(month, day));
}

std::vector<cPlotConfigExclusion>* const cPlotConfigPlotInfo::getExclusions(const int month, const int day)
{
	return getExclusions(plot_config::to_date(month, day));
}

void cPlotConfigPlotInfo::setPlotNumber(uint32_t num)
{
	uint32_t plotNumber = num;

/*
	if (hasSubPlotNumber())
	{
		num = num << 8;
		plotNumber = mPlotNumber & ~PLOT_MASK;
		plotNumber = plotNumber | num;
	}
*/

	mDirty |= (mPlotNumber != plotNumber);
	mPlotNumber = plotNumber;
}

/*
void cPlotConfigPlotInfo::setSubPlotNumber(uint8_t num)
{
	if (num == INVALID_SUB_PLOT_NUMBER)
	{
		if (hasSubPlotNumber())
		{
			mPlotNumber &= ~HAS_SUB_PLOT_MASK;
			mPlotNumber >>= 8;
			mDirty = true;
		}

		return;
	}

	uint32_t plotNumber = mPlotNumber;

	if (hasSubPlotNumber())
	{
		plotNumber = (mPlotNumber & PLOT_MASK) | SUB_PLOT_FLAG | num;
	}
	else
	{
		plotNumber = mPlotNumber << 8;
		plotNumber = plotNumber | SUB_PLOT_FLAG | num;
	}

	mDirty |= (mPlotNumber != plotNumber);
	mPlotNumber = plotNumber;
}
*/

void cPlotConfigPlotInfo::setPlotName(const std::string& name)
{
	mDirty |= (mPlotName != name);
	mPlotName = name;
}

void cPlotConfigPlotInfo::setEvent(const std::string& event)
{
	mDirty |= (mEvent != event);
	mEvent = event;
}

void cPlotConfigPlotInfo::setDescription(const std::string& desc)
{
	mDirty |= (mDescription != desc);
	mDescription = desc;
}

void cPlotConfigPlotInfo::setSpecies(const std::string& species)
{
	mDirty |= (mSpecies != species);
	mSpecies = species;
}

void cPlotConfigPlotInfo::setCultivar(const std::string& cultivar)
{
	mDirty |= (mCultivar != cultivar);
	mCultivar = cultivar;
}

void cPlotConfigPlotInfo::setConstructName(const std::string& name)
{
	mDirty |= (mConstructName != name);
	mConstructName = name;
}

void cPlotConfigPlotInfo::setPotLabel(const std::string& pot)
{
	mDirty |= (mPotLabel != pot);
	mPotLabel = pot;
}

void cPlotConfigPlotInfo::setSeedGeneration(const std::string& generation)
{
	mDirty |= (mSeedGeneration != generation);
	mSeedGeneration = generation;
}

void cPlotConfigPlotInfo::setCopyNumber(const std::string& num)
{
	mDirty |= (mCopyNumber != num);
	mCopyNumber = num;
}

void cPlotConfigPlotInfo::setLeafType(const std::string& leaf_type)
{
	mDirty |= (mLeafType != leaf_type);
	mLeafType = leaf_type;
}

void cPlotConfigPlotInfo::setTreatment(const std::string& treatment)
{
	if (mTreatments.size() != 1)
	{
		clearTreatments();
		addTreatment(treatment);
	}
	else
	{
		mDirty |= (mTreatments.front() != treatment); 
		mTreatments.front() = treatment;
	}
}

void cPlotConfigPlotInfo::setTreatments(const std::vector<std::string>& treatments)
{
	if (mTreatments.size() != treatments.size())
	{
		clearTreatments();

		for (const auto& treatment : treatments)
			mTreatments.push_back(treatment);
	}
	else
	{
		auto n = treatments.size();

		for (int i = 0; i < n; ++i)
		{
			mDirty |= (mTreatments[i] != treatments[i]);
			mTreatments[i] = treatments[i];
		}
	}
}

void cPlotConfigPlotInfo::clearTreatments()
{
	mDirty |= (!mTreatments.empty());
	mTreatments.clear();
}

void cPlotConfigPlotInfo::addTreatment(const std::string& treatment)
{
	if (treatment.empty()) return;

	mDirty = true;
	mTreatments.push_back(treatment);
}

cPlotConfigCorrection& cPlotConfigPlotInfo::add(const int month, const int day)
{
	return mCorrections.add(month, day);
}

void cPlotConfigPlotInfo::setBounds(const int month, const int day, const cPlotConfigBoundary& bounds)
{
	auto it = find_exact(month, day);

	if (it == mCorrections.end())
	{
		mCorrections.add(month, day).setBounds(bounds);
	}
	else
		it->second.setBounds(bounds);
}

void cPlotConfigPlotInfo::setIsolationMethod(const int month, const int day,const cPlotConfigIsolationMethod& method)
{
	auto it = find_exact(month, day);

	if (it == mCorrections.end())
	{
		mCorrections.add(month, day).setIsolationMethod(method);
	}
	else
		it->second.setIsolationMethod(method);
}

void cPlotConfigPlotInfo::setExclusions(const int month, const int day, const std::vector<cPlotConfigExclusion>& exclusions)
{
	auto it = find_exact(month, day);

	if (it == mCorrections.end())
	{
		mCorrections.add(month, day).setExclusions(exclusions);
	}
	else
		it->second.setExclusions(exclusions);
}

void cPlotConfigPlotInfo::clearDirtyFlag()
{
	mDirty = false;
	mCorrections.clearDirtyFlag();
}

void cPlotConfigPlotInfo::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
	mCorrections.setDirtyFlag(dirty);
}

void cPlotConfigPlotInfo::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("number"))
		mPlotNumber = jdoc["number"];
	else
	{ }

/*
	if (jdoc.contains("sub plot number"))
	{
		uint8_t subPlotNumber = jdoc["sub plot number"];
		setSubPlotNumber(subPlotNumber);
	}
*/

	if (jdoc.contains("name"))
		mPlotName = jdoc["name"];

	if (jdoc.contains("description"))
		mDescription = jdoc["description"];

	if (jdoc.contains("event"))
		mEvent = jdoc["event"];

	if (jdoc.contains("species"))
		mSpecies = jdoc["species"];

	if (jdoc.contains("cultivar"))
		mCultivar = jdoc["cultivar"];

	if (jdoc.contains("construct"))
		mConstructName = jdoc["construct"];

	if (jdoc.contains("pot label"))
		mPotLabel = jdoc["pot label"];

	if (jdoc.contains("seed generation"))
		mSeedGeneration = jdoc["seed generation"];

	if (jdoc.contains("copy number"))
		mCopyNumber = jdoc["copy number"];

	if (jdoc.contains("leaf type"))
		mLeafType = jdoc["leaf type"];

	if (jdoc.contains("treatment"))
	{
		mTreatments.clear();
		mTreatments.push_back(jdoc["treatment"]);
	}

	if (jdoc.contains("treatments"))
	{
		mTreatments.clear();
		nlohmann::json treatments = jdoc["treatments"];

		for (const auto& treatment : treatments)
			mTreatments.push_back(treatment);
	}

	if (jdoc.contains("corrections"))
	{
		const auto& corrections = jdoc["corrections"];
		mCorrections.load(corrections);
	}

	if (jdoc.contains("sub plots") || jdoc.contains("corners"))
	{
		auto& entry = mCorrections.add(0, 0);

		cPlotConfigBoundary bounds;

		bounds.load(jdoc);

		entry.setBounds(bounds);

		if (jdoc.contains("isolation method"))
		{
			cPlotConfigIsolationMethod isolationMethod;
			isolationMethod.load(jdoc);
			entry.setIsolationMethod(isolationMethod);
		}
	}
}

nlohmann::json cPlotConfigPlotInfo::save()
{
	nlohmann::json infoDoc;

	infoDoc["number"] = getPlotNumber();

/*
	if (hasSubPlotNumber())
		infoDoc["sub plot number"] = getSubPlotNumber();
*/

	if (!mPlotName.empty())
		infoDoc["name"] = mPlotName;

	if (!mDescription.empty())
		infoDoc["description"] = mDescription;

	if (!mEvent.empty())
		infoDoc["event"] = mEvent;

	if (!mSpecies.empty())
		infoDoc["species"] = mSpecies;

	if (!mCultivar.empty())
		infoDoc["cultivar"] = mCultivar;

	if (!mConstructName.empty())
		infoDoc["construct"] = mConstructName;

	if (!mPotLabel.empty())
		infoDoc["pot label"] = mPotLabel;

	if (!mSeedGeneration.empty())
		infoDoc["seed generation"] = mSeedGeneration;

	if (!mCopyNumber.empty())
		infoDoc["copy number"] = mCopyNumber;

	if (!mLeafType.empty())
		infoDoc["leaf type"] = mLeafType;

	if (mTreatments.size() == 1)
	{
		infoDoc["treatment"] = mTreatments.front();
	}
	else if (mTreatments.size() > 1)
	{
		nlohmann::json treatments;

		for (const auto& treatment : mTreatments)
			treatments.push_back(treatment);

		infoDoc["treatments"] = treatments;
	}

//	mBounds.save(infoDoc);
//	infoDoc["isolation method"] = mIsolationMethod.save();

	infoDoc["corrections"] = mCorrections.save();

	mDirty = false;

	return infoDoc;
}


