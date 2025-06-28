
#include "LidarMapConfigConfiguration.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

cLidarMapConfigConfiguration::cLidarMapConfigConfiguration()
{}

void cLidarMapConfigConfiguration::clear()
{
	mDirty = false;
	mScans.clear();
}

bool cLidarMapConfigConfiguration::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mDirty;
}

void cLidarMapConfigConfiguration::setDirty(bool dirty)
{
	mDirty = dirty;
}

bool cLidarMapConfigConfiguration::empty() const
{
	return mScans.empty();
}

std::size_t cLidarMapConfigConfiguration::size() const
{
	return mScans.size();
}

bool cLidarMapConfigConfiguration::contains(const std::string& name)
{
	for (const auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return true;
	}

	return false;
}

const cLidarMapConfigScan& cLidarMapConfigConfiguration::front() const { return mScans.front(); }
cLidarMapConfigScan& cLidarMapConfigConfiguration::front() { return mScans.front(); }


cLidarMapConfigConfiguration::iterator cLidarMapConfigConfiguration::begin() { return mScans.begin(); }
cLidarMapConfigConfiguration::iterator cLidarMapConfigConfiguration::end() { return mScans.end(); }

cLidarMapConfigConfiguration::const_iterator	cLidarMapConfigConfiguration::begin() const { return mScans.cbegin(); }
cLidarMapConfigConfiguration::const_iterator	cLidarMapConfigConfiguration::end() const { return mScans.cend(); }

cLidarMapConfigConfiguration::const_iterator	cLidarMapConfigConfiguration::find_by_filename(const std::string& measurement_filename) const
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

cLidarMapConfigConfiguration::iterator cLidarMapConfigConfiguration::find_by_filename(const std::string& measurement_filename)
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

cLidarMapConfigConfiguration::const_iterator cLidarMapConfigConfiguration::find(const std::string& name) const
{
	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigConfiguration::iterator cLidarMapConfigConfiguration::find(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigScan& cLidarMapConfigConfiguration::add(const std::string& name)
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

void cLidarMapConfigConfiguration::remove(const std::string& name)
{

}

const cLidarMapConfigScan& cLidarMapConfigConfiguration::operator[](int index) const
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

cLidarMapConfigScan& cLidarMapConfigConfiguration::operator[](int index)
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}



void cLidarMapConfigConfiguration::load(const nlohmann::json& jdoc)
{
}

nlohmann::json cLidarMapConfigConfiguration::save()
{
	nlohmann::json configDoc;


	return configDoc;
}


