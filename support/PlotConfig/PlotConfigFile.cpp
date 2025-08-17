
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

const std::string& cPlotConfigFile::getTemporaryFileName() const
{
	return mTmpFileName;
}

bool cPlotConfigFile::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mIsDirty || mOptions.isDirty();
}

void cPlotConfigFile::clear()
{
	mIsDirty = false;
	mFileName.clear();
	mAllowedExperimentNames.clear();
	mOptions.clear();
	mScans.clear();
	mTmpFileName = "~newfile.plotsplit";
}

bool cPlotConfigFile::open(const std::string& file_name)
{
	using namespace nStringUtils;

	if (file_name.empty()) return false;

	if (is_temp_filename(file_name))
		return open_temporary_file(file_name);

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
	mTmpFileName = nStringUtils::make_temp_filename(mFileName);

	if (configDoc.contains("allowed_experiment_names"))
	{
		auto allowed = configDoc["allowed_experiment_names"];

		if (allowed.is_array())
		{
			for (const auto& name : allowed)
				mAllowedExperimentNames.insert(name);
		}
		else if (allowed.is_string())
		{
			auto experiment_name = allowed.get<std::string>();
			mAllowedExperimentNames.insert(experiment_name);
		}
	}

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

	mIsDirty = false;

	return true;
}

void cPlotConfigFile::save()
{
	if (mFileName.empty()) return;

	nlohmann::json configDoc;

	if (mAllowedExperimentNames.size() == 1)
	{
		configDoc["allowed_experiment_names"] = *(mAllowedExperimentNames.begin());
	}
	else if (mAllowedExperimentNames.size() > 1)
	{
		for (const auto& name : mAllowedExperimentNames)
		{
			configDoc["allowed_experiment_names"].push_back(name);
		}
	}

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

	if (out.good())
	{
		// Remove the temporary file
		std::filesystem::path tmp = mTmpFileName;
		if (std::filesystem::exists(tmp))
		{
			std::filesystem::remove(tmp);
		}
	}

	clearDirtyFlag();
}

void cPlotConfigFile::save_as(const std::string& file_name)
{
	nlohmann::json configDoc;

	if (mAllowedExperimentNames.size() == 1)
	{
		configDoc["allowed_experiment_names"] = *(mAllowedExperimentNames.begin());
	}
	else if (mAllowedExperimentNames.size() > 1)
	{
		for (const auto& name : mAllowedExperimentNames)
		{
			configDoc["allowed_experiment_names"].push_back(name);
		}
	}

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

	if (out.good())
	{
		// Remove the temporary file
		std::filesystem::path tmp = mTmpFileName;
		if (std::filesystem::exists(tmp))
		{
			std::filesystem::remove(tmp);
		}
	}

	mFileName = file_name;
	mTmpFileName = nStringUtils::make_temp_filename(mFileName);

	clearDirtyFlag();
}

bool cPlotConfigFile::open_temporary_file(const std::string& file_name)
{
	if (file_name.empty()) return false;
	if (!nStringUtils::is_temp_filename(file_name)) return false;

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

	mTmpFileName = file_name;

	mFileName = nStringUtils::convert_temp_filename(mTmpFileName);

	if (mFileName.find("newfile") != std::string::npos)
	{
		mFileName.clear();
	}

	if (configDoc.contains("allowed_experiment_names"))
	{
		auto allowed = configDoc["allowed_experiment_names"];

		if (allowed.is_array())
		{
			for (const auto& name : allowed)
				mAllowedExperimentNames.insert(name);
		}
		else if (allowed.is_string())
		{
			auto experiment_name = allowed.get<std::string>();
			mAllowedExperimentNames.insert(experiment_name);
		}
	}

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

	mIsDirty = true;

	return true;
}

void cPlotConfigFile::save_temporary_file()
{
	nlohmann::json configDoc;

	bool options_dirty = mOptions.isDirty();
	configDoc["options"] = mOptions.save();
	mOptions.setDirty(options_dirty);

	nlohmann::json scansDoc;

	for (auto& scan : mScans)
	{
		bool scan_dirty = scan.isDirty();
		scansDoc.push_back(scan.save());
		scan.setDirtyFlag(scan_dirty);
	}

	if (!scansDoc.is_null())
		configDoc["scans"] = scansDoc;

	std::ofstream out;
	out.open(mTmpFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;
}

void cPlotConfigFile::clearAllowedExperimentNames()
{
	if (!mAllowedExperimentNames.empty())
		mIsDirty = true;

	mAllowedExperimentNames.clear();
}

std::set<std::string> cPlotConfigFile::getAllowedExperimentNames() const
{
	return mAllowedExperimentNames;
}

void cPlotConfigFile::setAllowedExperimentNames(std::set<std::string> names)
{
	mIsDirty |= (mAllowedExperimentNames != names);
	mAllowedExperimentNames = names;
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
		if (scan.getMeasurementName() == name)
			return true;
	}

	return false;
}

const cPlotConfigScan& cPlotConfigFile::front() const { return mScans.front(); }
cPlotConfigScan&	   cPlotConfigFile::front() { return mScans.front(); }

const cPlotConfigScan&	cPlotConfigFile::back() const { return mScans.back(); }
cPlotConfigScan&		cPlotConfigFile::back() { return mScans.back(); }

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
		auto name = safeFilename(it->getMeasurementName());
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
		auto name = safeFilename(it->getMeasurementName());
		if (filename == name)
			return it;
	}

	return mScans.end();
}

cPlotConfigFile::const_iterator cPlotConfigFile::find_by_measurement_name(const std::string& name) const
{
	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.cend();
}

cPlotConfigFile::iterator cPlotConfigFile::find_by_measurement_name(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.end();
}

cPlotConfigScan& cPlotConfigFile::add(const std::string& name)
{
	for (auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return scan;
	}

	cPlotConfigScan scan;

	scan.setMeasurementName(name);

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

void cPlotConfigFile::clearDirtyFlag()
{
	mIsDirty = false;

	mOptions.setDirty(false);

	for (auto& scan : mScans)
	{
		scan.setDirtyFlag(false);
	}
}
