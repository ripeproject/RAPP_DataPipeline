
#include "LidarMapConfigCatalog.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

cLidarMapConfigCatalog::cLidarMapConfigCatalog(int month, int day)
	: mEffectiveMonth(month), mEffectiveDay(day)

{}

const int cLidarMapConfigCatalog::date() const
{
	return (mEffectiveMonth * 100) + mEffectiveDay;
}

const int cLidarMapConfigCatalog::month() const
{
	return mEffectiveMonth;
}

const int cLidarMapConfigCatalog::day() const
{
	return mEffectiveDay;
}

void cLidarMapConfigCatalog::clear()
{
	mDirty = false;
	mScans.clear();
}

bool cLidarMapConfigCatalog::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mDirty;
}

void cLidarMapConfigCatalog::setDirty(bool dirty)
{
	mDirty = dirty;
}

bool cLidarMapConfigCatalog::empty() const
{
	return mScans.empty();
}

std::size_t cLidarMapConfigCatalog::size() const
{
	return mScans.size();
}

bool cLidarMapConfigCatalog::contains(const std::string& name) const
{
	for (const auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return true;
	}

	return false;
}

const cLidarMapConfigScan& cLidarMapConfigCatalog::front() const { return mScans.front(); }
cLidarMapConfigScan& cLidarMapConfigCatalog::front() { return mScans.front(); }


cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::begin() { return mScans.begin(); }
cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::end() { return mScans.end(); }

cLidarMapConfigCatalog::const_iterator	cLidarMapConfigCatalog::begin() const { return mScans.cbegin(); }
cLidarMapConfigCatalog::const_iterator	cLidarMapConfigCatalog::end() const { return mScans.cend(); }

cLidarMapConfigCatalog::const_iterator	cLidarMapConfigCatalog::find_by_filename(const std::string& measurement_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);
	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		auto name = safeFilename(it->getMeasurementName());
		if (filename == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::find_by_filename(const std::string& measurement_filename)
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);
	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		auto name = safeFilename(it->getMeasurementName());
		if (filename == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigCatalog::const_iterator cLidarMapConfigCatalog::find(const std::string& name) const
{
	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::find(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigScan& cLidarMapConfigCatalog::add(const std::string& name)
{
	for (auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return scan;
	}

	cLidarMapConfigScan scan;

	scan.setMeasurementName(name);

	mScans.push_back(std::move(scan));

	return mScans.back();
}

void cLidarMapConfigCatalog::remove(const std::string& name)
{

}

const cLidarMapConfigScan& cLidarMapConfigCatalog::operator[](int index) const
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

cLidarMapConfigScan& cLidarMapConfigCatalog::operator[](int index)
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}



void cLidarMapConfigCatalog::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("scans"))
	{
		const auto& scans = jdoc["scans"];

		for (const auto& entry : scans)
		{

			cLidarMapConfigScan scan;

			scan.setEffectiveDate(mEffectiveMonth, mEffectiveDay);

			scan.load(entry);

			mScans.push_back(std::move(scan));
		}
	}
}

nlohmann::json cLidarMapConfigCatalog::save()
{
	nlohmann::json catalogDoc;

	std::string date = std::to_string(mEffectiveMonth);
	date += "/";
	date += std::to_string(mEffectiveDay);

	catalogDoc["date"] = date;

	nlohmann::json scansDoc;

	for (auto& scan : mScans)
	{
		scansDoc.push_back(scan.save());
	}

	if (!scansDoc.is_null())
		catalogDoc["scans"] = scansDoc;

	mDirty = false;

	return catalogDoc;
}


