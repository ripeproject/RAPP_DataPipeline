
#include "LidarMapConfigFile.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>


cLidarMapConfigFile::cLidarMapConfigFile()
{}

cLidarMapConfigFile::~cLidarMapConfigFile()
{}

const std::string& cLidarMapConfigFile::getFileName() const
{
	return mFileName;
}

bool cLidarMapConfigFile::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mOptions.isDirty() || mDefaults.isDirty();
}

void cLidarMapConfigFile::clear()
{
	mFileName.clear();
	mOptions.clear();
	mScans.clear();
}

bool cLidarMapConfigFile::open(const std::string& file_name)
{
	if (file_name.empty()) return false;

	std::ifstream in;
	in.open(file_name);
	if (!in.is_open())
		return false;

	nlohmann::json configDoc;
	try
	{
		in >> configDoc;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		auto msg = e.what();
		return false;
	}
	catch (const std::exception& e)
	{
		return false;
	}

	mFileName = file_name;

	mOptions.load(configDoc);
	mDefaults.load(configDoc);

	if (configDoc.contains("experiments"))
	{
		const auto& experiments = configDoc["experiments"];

		for (const auto& entry : experiments)
		{

			cLidarMapConfigScan scan;

			scan.load(entry);

			mScans.push_back(std::move(scan));
		}
	}

	return true;
}

void cLidarMapConfigFile::save()
{
	if (mFileName.empty()) return;

	nlohmann::json configDoc;

	configDoc["options"]  = mOptions.save();
	configDoc["defaults"] = mDefaults.save();

	nlohmann::json scansDoc;

	for (auto& scan : mScans)
	{
		scansDoc.push_back(scan.save());
	}

	if (!scansDoc.is_null())
		configDoc["scans"] = scansDoc;

	std::ofstream out;
	out.open(mFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;
}

void cLidarMapConfigFile::save_as(const std::string& file_name)
{
	nlohmann::json configDoc;

	configDoc["options"]  = mOptions.save();
	configDoc["defaults"] = mDefaults.save();

	nlohmann::json scansDoc;

	for (auto& scan : mScans)
	{
		scansDoc.push_back(scan.save());
	}

	configDoc["scans"] = scansDoc;

	std::ofstream out;
	out.open(file_name);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;

	mFileName = file_name;
}

const cLidarMapConfigOptions& cLidarMapConfigFile::getOptions() const
{
	return mOptions;
}

cLidarMapConfigOptions& cLidarMapConfigFile::getOptions()
{
	return mOptions;
}

const cLidarMapConfigDefaults& cLidarMapConfigFile::getDefaults() const
{
	return mDefaults;
}

cLidarMapConfigDefaults& cLidarMapConfigFile::getDefaults()
{
	return mDefaults;
}

bool cLidarMapConfigFile::empty() const
{
	return mScans.empty();
}

std::size_t cLidarMapConfigFile::size() const
{
	return mScans.size();
}

bool cLidarMapConfigFile::contains(const std::string& name)
{
	for (const auto& scan : mScans)
	{
		if (scan.getExperimentName() == name)
			return true;
	}

	return false;
}

const cLidarMapConfigScan& cLidarMapConfigFile::front() const { return mScans.front(); }
cLidarMapConfigScan&	   cLidarMapConfigFile::front() { return mScans.front(); }


cLidarMapConfigFile::iterator cLidarMapConfigFile::begin() { return mScans.begin(); }
cLidarMapConfigFile::iterator cLidarMapConfigFile::end() { return mScans.end(); }

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::begin() const { return mScans.cbegin(); }
cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::end() const { return mScans.cend(); }

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::find_by_filename(const std::string& experiment_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(experiment_filename);
	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		auto name = safeFilename(it->getExperimentName());
		if (filename == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find_by_filename(const std::string& experiment_filename)
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(experiment_filename);
	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		auto name = safeFilename(it->getExperimentName());
		if (filename == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigFile::const_iterator cLidarMapConfigFile::find(const std::string& name) const
{
	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		if (it->getExperimentName() == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getExperimentName() == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigScan& cLidarMapConfigFile::add(const std::string& name)
{
	for (auto& scan : mScans)
	{
		if (scan.getExperimentName() == name)
			return scan;
	}

	cLidarMapConfigScan scan;

	scan.setExperimentName(name);

	mScans.push_back(std::move(scan));

	return mScans.back();
}

void cLidarMapConfigFile::remove(const std::string& name)
{

}

const cLidarMapConfigScan& cLidarMapConfigFile::operator[](int index) const
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

cLidarMapConfigScan& cLidarMapConfigFile::operator[](int index)
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

