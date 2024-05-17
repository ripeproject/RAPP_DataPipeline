
#include "PlotConfigFile.hpp"
#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>


cPlotConfigFile::cPlotConfigFile()
{}

cPlotConfigFile::~cPlotConfigFile()
{}

const std::string& cPlotConfigFile::getFileName() const
{
	return mFileName;
}

bool cPlotConfigFile::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mOptions.isDirty();
}

void cPlotConfigFile::clear()
{
	mFileName.clear();
	mOptions.clear();
	mScans.clear();
}

bool cPlotConfigFile::open(const std::string& file_name)
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

	if (configDoc.contains("scans"))
	{
		const auto& scans = configDoc["scans"];

		for (const auto& entry : scans)
		{
			cPlotConfigScan scan;

			scan.load(entry);

			mScans.push_back(std::move(scan));
		}
	}

	return true;
}

void cPlotConfigFile::save()
{
	if (mFileName.empty()) return;

	nlohmann::json configDoc;

	configDoc["options"] = mOptions.save();

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

void cPlotConfigFile::save_as(const std::string& file_name)
{
	nlohmann::json configDoc;

	configDoc["options"] = mOptions.save();

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

const cPlotConfigOptions& cPlotConfigFile::getOptions() const
{
	return mOptions;
}

cPlotConfigOptions& cPlotConfigFile::getOptions()
{
	return mOptions;
}

bool cPlotConfigFile::empty() const
{
	return mScans.empty();
}

std::size_t cPlotConfigFile::size() const
{
	return mScans.size();
}

bool cPlotConfigFile::contains(const std::string& name)
{
	for (const auto& scan : mScans)
	{
		if (scan.getExperimentName() == name)
			return true;
	}

	return false;
}

const cPlotConfigScan& cPlotConfigFile::front() const { return mScans.front(); }
cPlotConfigScan&	   cPlotConfigFile::front() { return mScans.front(); }


cPlotConfigFile::iterator cPlotConfigFile::begin() { return mScans.begin(); }
cPlotConfigFile::iterator cPlotConfigFile::end() { return mScans.end(); }

cPlotConfigFile::const_iterator	cPlotConfigFile::begin() const { return mScans.cbegin(); }
cPlotConfigFile::const_iterator	cPlotConfigFile::end() const { return mScans.cend(); }

cPlotConfigFile::const_iterator	cPlotConfigFile::find_by_filename(const std::string& experiment_filename) const
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

cPlotConfigFile::iterator cPlotConfigFile::find_by_filename(const std::string& experiment_filename)
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

cPlotConfigFile::const_iterator cPlotConfigFile::find(const std::string& name) const
{
	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		if (it->getExperimentName() == name)
			return it;
	}

	return mScans.cend();
}

cPlotConfigFile::iterator cPlotConfigFile::find(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getExperimentName() == name)
			return it;
	}

	return mScans.end();
}

cPlotConfigScan& cPlotConfigFile::add(const std::string& name)
{
	for (auto& scan : mScans)
	{
		if (scan.getExperimentName() == name)
			return scan;
	}

	cPlotConfigScan scan;

	scan.setExperimentName(name);

	mScans.push_back(std::move(scan));

	return mScans.back();
}

void cPlotConfigFile::remove(const std::string& name)
{

}


const cPlotConfigScan& cPlotConfigFile::operator[](int index) const
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

cPlotConfigScan& cPlotConfigFile::operator[](int index)
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

