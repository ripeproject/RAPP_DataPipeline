
#include "LidarMapConfigFile.hpp"
#include "LidarMapConfigCatalog.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <map>


cLidarMapConfigFile::cLidarMapConfigFile()
{}

cLidarMapConfigFile::~cLidarMapConfigFile()
{}

const std::string& cLidarMapConfigFile::getFileName() const
{
	return mFileName;
}

const std::string& cLidarMapConfigFile::getTemporaryFileName() const
{
	return mTmpFileName;
}

bool cLidarMapConfigFile::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mIsDirty || mOptions.isDirty() || mDefaults.isDirty();
}

void cLidarMapConfigFile::clear()
{
	mIsDirty = false;
	mFileName.clear();
	mAllowedExperimentNames.clear();
	mOptions.clear();
	mScans.clear();
	mTmpFileName = "~newfile.lidar2pointcloud";
}

bool cLidarMapConfigFile::open(const std::string& file_name)
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
	mDefaults.load(configDoc);

	if (configDoc.contains("scans"))
	{
		auto scansDoc = configDoc["scans"];

		for (const auto& scanDoc : scansDoc)
		{
			cLidarMapConfigScan scan;
			scan.load(scanDoc);

			mScans.push_back(std::move(scan));
		}
	}
	else if (configDoc.contains("catalog"))
	{
		nlohmann::json catalogDoc = configDoc["catalog"];
		std::map<int, cLidarMapConfigCatalog> _catalog;

		for (const auto& entry : catalogDoc)
		{
			if (entry.contains("date"))
			{
				std::string date = entry["date"];
				auto pos = date.find('/');
				int month = std::stoi(date.substr(0, pos));
				int day = std::stoi(date.substr(pos + 1));
				cLidarMapConfigCatalog catalog(month, day);
				catalog.load(entry);

				_catalog.insert(std::make_pair(catalog.date(), std::move(catalog)));
			}
		}

		mMeasurementNames.clear();

		for (auto& entry : _catalog)
		{
			for (auto& scan : entry.second)
			{
				mMeasurementNames.insert(scan.getMeasurementName());
			}
		}

		for (auto& name : mMeasurementNames)
		{
			auto& scan = add(name);

			for (auto& entry : _catalog)
			{
				auto date = entry.first;
				auto it = entry.second.find(name);

				if (it == entry.second.end())
					continue;

				const auto& catalog_scan = *it;
				
				if (!scan.getReferencePoint().has_value() && catalog_scan.getReferencePoint().has_value())
					scan.setReferencePoint(catalog_scan.getReferencePoint());

				if (!scan.getSensorMountPitch_deg().has_value() && catalog_scan.getSensorMountPitch_deg().has_value())
					scan.setSensorMountPitch_deg(catalog_scan.getSensorMountPitch_deg());
				if (!scan.getSensorMountRoll_deg().has_value() && catalog_scan.getSensorMountRoll_deg().has_value())
					scan.setSensorMountRoll_deg(catalog_scan.getSensorMountRoll_deg());
				if (!scan.getSensorMountYaw_deg().has_value() && catalog_scan.getSensorMountYaw_deg().has_value())
					scan.setSensorMountYaw_deg(catalog_scan.getSensorMountYaw_deg());

				if (!scan.getMinDistance_m().has_value() && catalog_scan.getMinDistance_m().has_value())
					scan.setMinDistance_m(catalog_scan.getMinDistance_m());
				if (!scan.getMaxDistance_m().has_value() && catalog_scan.getMaxDistance_m().has_value())
					scan.setMaxDistance_m(catalog_scan.getMaxDistance_m());
				if (!scan.getMinAzimuth_deg().has_value() && catalog_scan.getMinAzimuth_deg().has_value())
					scan.setMinAzimuth_deg(catalog_scan.getMinAzimuth_deg());
				if (!scan.getMaxAzimuth_deg().has_value() && catalog_scan.getMaxAzimuth_deg().has_value())
					scan.setMaxAzimuth_deg(catalog_scan.getMaxAzimuth_deg());
				if (!scan.getMinAltitude_deg().has_value() && catalog_scan.getMinAltitude_deg().has_value())
					scan.setMinAltitude_deg(catalog_scan.getMinAltitude_deg());
				if (!scan.getMaxAltitude_deg().has_value() && catalog_scan.getMaxAltitude_deg().has_value())
					scan.setMaxAltitude_deg(catalog_scan.getMaxAltitude_deg());

				auto scanIt = scan.find_exact(catalog_scan.month(), catalog_scan.day());

				if (scanIt == scan.end())
				{
					auto& parameters = scan.add(catalog_scan.month(), catalog_scan.day());

					parameters.setKinematicModel(catalog_scan.getKinematicModel());
					parameters.setOrientationModel(catalog_scan.getOrientationModel());
					parameters.setTranslateToGroundModel(catalog_scan.getTranslateToGroundModel());
					parameters.setRotateToGroundModel(catalog_scan.getRotateToGroundModel());

					parameters.setVx_mmps(catalog_scan.getVx_mmps());
					parameters.setVy_mmps(catalog_scan.getVy_mmps());
					parameters.setVz_mmps(catalog_scan.getVz_mmps());

					parameters.setSensorPitchOffset_deg(catalog_scan.getSensorPitchOffset_deg());
					parameters.setSensorRollOffset_deg(catalog_scan.getSensorRollOffset_deg());
					parameters.setSensorYawOffset_deg(catalog_scan.getSensorYawOffset_deg());

					parameters.setStartPitchOffset_deg(catalog_scan.getStartPitchOffset_deg());
					parameters.setStartRollOffset_deg(catalog_scan.getStartRollOffset_deg());
					parameters.setStartYawOffset_deg(catalog_scan.getStartYawOffset_deg());

					parameters.setEndPitchOffset_deg(catalog_scan.getEndPitchOffset_deg());
					parameters.setEndRollOffset_deg(catalog_scan.getEndRollOffset_deg());
					parameters.setEndYawOffset_deg(catalog_scan.getEndYawOffset_deg());

					parameters.setOrientationTable(catalog_scan.getOrientationTable());

					parameters.setTranslateToGround(catalog_scan.getTranslateToGround());
					parameters.setTranslateDistance_m(catalog_scan.getTranslateDistance_m());
					parameters.setTranslateThreshold_pct(catalog_scan.getTranslateThreshold_pct());
					parameters.setTranslateTable(catalog_scan.getTranslateTable());

					parameters.setRotateToGround(catalog_scan.getRotateToGround());
					parameters.setRotateAngles_deg(catalog_scan.getRotatePitch_deg(), catalog_scan.getRotateRoll_deg());
					parameters.setRotateThreshold_pct(catalog_scan.getRotateThreshold_pct());
					parameters.setRotateTable(catalog_scan.getRotateTable());

					parameters.setStart_X_m(catalog_scan.getStart_X_m());
					parameters.setStart_Y_m(catalog_scan.getStart_Y_m());
					parameters.setStart_Z_m(catalog_scan.getStart_Z_m());

					parameters.setEnd_X_m(catalog_scan.getEnd_X_m());
					parameters.setEnd_Y_m(catalog_scan.getEnd_Y_m());
					parameters.setEnd_Z_m(catalog_scan.getEnd_Z_m());
				}
			}
		}

		mIsDirty = true;
		return true;
	}

	mIsDirty = false;

	return true;
}

void cLidarMapConfigFile::save()
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
	configDoc["defaults"] = mDefaults.save();

	nlohmann::json scansDoc;

	for (auto& entry : mScans)
	{
		scansDoc.push_back(entry.save());
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

	mOptions.setDirty(false);
	mDefaults.setDirty(false);

	for (auto& entry : mScans)
	{
		entry.setDirty(false);
	}

	mIsDirty = false;
}

void cLidarMapConfigFile::save_as(const std::string& file_name)
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
	configDoc["defaults"] = mDefaults.save();

	nlohmann::json scansDoc;

	for (auto& entry : mScans)
	{
		scansDoc.push_back(entry.save());
	}

	if (!scansDoc.is_null())
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

	mOptions.setDirty(false);
	mDefaults.setDirty(false);

	for (auto& entry : mScans)
	{
		entry.setDirty(false);
	}

	mIsDirty = false;
}

bool cLidarMapConfigFile::open_temporary_file(const std::string& file_name)
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
	mDefaults.load(configDoc);

	if (configDoc.contains("scans"))
	{
		cLidarMapConfigScan scan;
		scan.load(configDoc);

		mScans.push_back(std::move(scan));
	}

	mIsDirty = true;

	return true;
}

void cLidarMapConfigFile::save_temporary_file()
{
	nlohmann::json configDoc;

	bool options_dirty = mOptions.isDirty();
	configDoc["options"] = mOptions.save();
	mOptions.setDirty(options_dirty);

	bool defaults_dirty = mDefaults.isDirty();
	configDoc["defaults"] = mDefaults.save();
	mDefaults.setDirty(defaults_dirty);

	nlohmann::json scansDoc;

	for (auto& entry : mScans)
	{
		bool scan_dirty = entry.isDirty();
		scansDoc.push_back(entry.save());
		entry.setDirty(scan_dirty);
	}

	if (!scansDoc.is_null())
		configDoc["scans"] = scansDoc;

	std::ofstream out;
	out.open(mTmpFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;
}

void cLidarMapConfigFile::clearAllowedExperimentNames()
{
	if (!mAllowedExperimentNames.empty())
		mIsDirty = true;

	mAllowedExperimentNames.clear();
}

std::set<std::string> cLidarMapConfigFile::getAllowedExperimentNames() const
{
	return mAllowedExperimentNames;
}

void cLidarMapConfigFile::setAllowedExperimentNames(std::set<std::string> names)
{
	mIsDirty |= (mAllowedExperimentNames != names);
	mAllowedExperimentNames = names;
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

bool cLidarMapConfigFile::contains(const std::string& name) const
{
	for (const auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return true;
	}

	return false;
}

bool cLidarMapConfigFile::contains_by_filename(const std::string& measurement_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);

	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto measurement_name = safeFilename(in.filename);

	return contains(measurement_name);
}

const cLidarMapConfigScan& cLidarMapConfigFile::front() const { return mScans.front(); }
cLidarMapConfigScan& cLidarMapConfigFile::front() { return mScans.front(); }

const cLidarMapConfigScan& cLidarMapConfigFile::back() const { return mScans.back(); }
cLidarMapConfigScan& cLidarMapConfigFile::back() { return mScans.back(); }


cLidarMapConfigFile::iterator cLidarMapConfigFile::begin() { return mScans.begin(); }
cLidarMapConfigFile::iterator cLidarMapConfigFile::end() { return mScans.end(); }

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::begin() const { return mScans.cbegin(); }
cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::end() const { return mScans.cend(); }

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::find_by_filename(const std::string& experiment_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(experiment_filename);

	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto measurement_name = safeFilename(in.filename);

	return find_by_measurement_name(measurement_name);
}

cLidarMapConfigFile::iterator		cLidarMapConfigFile::find_by_filename(const std::string& experiment_filename)
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(experiment_filename);

	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto measurement_name = safeFilename(in.filename);

	return find_by_measurement_name(measurement_name);
}

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::find_by_measurement_name(const std::string& name) const
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
		{
			return it;
		}
	}

	return mScans.end();
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find_by_measurement_name(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
		{
			return it;
		}
	}

	return mScans.end();
}

cLidarMapConfigScan& cLidarMapConfigFile::add(const std::string& name)
{
	for (auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return scan;
	}

	mScans.emplace_back(name);

	mIsDirty = true;

	return mScans.back();
}

void cLidarMapConfigFile::remove(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
		{
			mScans.erase(it);
			mIsDirty = true;
			break;
		}
	}
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











#if 0
bool cLidarMapConfigFile::isDirty() const
{
	for (const auto& date : mCatalog)
	{
		if (date.second.isDirty())
			return true;
	}

	return mIsDirty || mOptions.isDirty() || mDefaults.isDirty();
}

void cLidarMapConfigFile::clear()
{
	mIsDirty = false;
	mFileName.clear();
	mAllowedExperimentNames.clear();
	mOptions.clear();
	mCatalog.clear();
	mTmpFileName = "~newfile.lidar2pointcloud";
}

bool cLidarMapConfigFile::open(const std::string& file_name)
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
	mDefaults.load(configDoc);

	if (configDoc.contains("scans"))
	{
		cLidarMapConfigCatalog catalog(0, 0);
		catalog.load(configDoc);

		mCatalog.insert(std::make_pair(0, std::move(catalog)));
	}

	if (configDoc.contains("catalog"))
	{
		nlohmann::json catalogDoc = configDoc["catalog"];

		for (const auto& entry : catalogDoc)
		{
			if (entry.contains("date"))
			{
				std::string date = entry["date"];
				auto pos = date.find('/');
				int month = std::stoi(date.substr(0, pos));
				int day = std::stoi(date.substr(pos+1));
				cLidarMapConfigCatalog catalog(month, day);
				catalog.load(entry);

				mCatalog.insert(std::make_pair(catalog.date(), std::move(catalog)));
			}
		}
	}
	else if (configDoc.contains("scans"))
	{
		cLidarMapConfigCatalog catalog(0, 0);
		catalog.load(configDoc);

		mCatalog.insert(std::make_pair(0, std::move(catalog)));
	}

	mIsDirty = false;

	return true;
}

void cLidarMapConfigFile::save()
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

	configDoc["options"]  = mOptions.save();
	configDoc["defaults"] = mDefaults.save();

	nlohmann::json catalogDoc;

	for (auto& entry : mCatalog)
	{
		auto& date = entry.second;
		catalogDoc.push_back(date.save());
	}

	if (!catalogDoc.is_null())
		configDoc["catalog"] = catalogDoc;

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

	mIsDirty = false;
}

void cLidarMapConfigFile::save_as(const std::string& file_name)
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

	configDoc["options"]  = mOptions.save();
	configDoc["defaults"] = mDefaults.save();

	nlohmann::json catalogDoc;

	for (auto& entry : mCatalog)
	{
		auto& date = entry.second;
		catalogDoc.push_back(date.save());
	}

	if (!catalogDoc.is_null())
		configDoc["catalog"] = catalogDoc;

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

	mIsDirty = false;
}

bool cLidarMapConfigFile::open_temporary_file(const std::string& file_name)
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
	mDefaults.load(configDoc);

	if (configDoc.contains("catalog"))
	{
		nlohmann::json catalogDoc = configDoc["catalog"];

		for (const auto& entry : catalogDoc)
		{
			if (entry.contains("date"))
			{
				std::string date = entry["date"];
				auto pos = date.find('/');
				int month = std::stoi(date.substr(0, pos));
				int day = std::stoi(date.substr(pos + 1));
				cLidarMapConfigCatalog catalog(month, day);
				catalog.load(entry);

				mCatalog.insert(std::make_pair(catalog.date(), std::move(catalog)));
			}
		}
	}
	else if (configDoc.contains("scans"))
	{
		cLidarMapConfigCatalog catalog(0, 0);
		catalog.load(configDoc);

		mCatalog.insert( std::make_pair(0, std::move(catalog)));
	}

	mIsDirty = true;

	return true;
}

void cLidarMapConfigFile::save_temporary_file()
{
	nlohmann::json configDoc;

	bool options_dirty = mOptions.isDirty();
	configDoc["options"] = mOptions.save();
	mOptions.setDirty(options_dirty);

	bool defaults_dirty = mDefaults.isDirty();
	configDoc["defaults"] = mDefaults.save();
	mDefaults.setDirty(defaults_dirty);

	nlohmann::json catalogDoc;

	for (auto& entry : mCatalog)
	{
		auto& date = entry.second;
		bool catalog_dirty = date.isDirty();
		catalogDoc.push_back(date.save());
		date.setDirty(catalog_dirty);
	}

	if (!catalogDoc.is_null())
		configDoc["catalog"] = catalogDoc;

	std::ofstream out;
	out.open(mTmpFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;
}

void cLidarMapConfigFile::clearAllowedExperimentNames()
{
	if (!mAllowedExperimentNames.empty())
		mIsDirty = true;

	mAllowedExperimentNames.clear();
}

std::set<std::string> cLidarMapConfigFile::getAllowedExperimentNames() const
{
	return mAllowedExperimentNames;
}

void cLidarMapConfigFile::setAllowedExperimentNames(std::set<std::string> names)
{
	mIsDirty |= (mAllowedExperimentNames != names);
	mAllowedExperimentNames = names;
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
	return mCatalog.empty();
}

std::size_t cLidarMapConfigFile::size() const
{
	return mCatalog.size();
}

bool cLidarMapConfigFile::contains(const std::string& name) const
{
	for (const auto& entry : mCatalog)
	{
		const auto& scans = entry.second;

		for (const auto& scan : scans)
		{
			if (scan.getMeasurementName() == name)
				return true;
		}
	}

	return false;
}

bool cLidarMapConfigFile::contains_by_filename(const std::string& measurement_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);

	std::tm time_stamp = extractMeasurementTimestamp(scan_file.filename);

	auto catalog = find(time_stamp.tm_mon + 1, time_stamp.tm_mday);

	if (catalog == mCatalog.end())
	{
		return false;
	}

	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	auto it = (*catalog).second.find(filename);

	if (it == (*catalog).second.end())
	{
		while (catalog != mCatalog.begin())
		{
			--catalog;
			it = (*catalog).second.find(filename);
			if (it != (*catalog).second.end())
				return true;
		}

		return false;
	}

	return true;

}

const cLidarMapConfigCatalog& cLidarMapConfigFile::front() const { return mCatalog.begin()->second; }
cLidarMapConfigCatalog&	   cLidarMapConfigFile::front() { return mCatalog.begin()->second; }


cLidarMapConfigFile::iterator cLidarMapConfigFile::begin() { return mCatalog.begin(); }
cLidarMapConfigFile::iterator cLidarMapConfigFile::end() { return mCatalog.end(); }

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::begin() const { return mCatalog.cbegin(); }
cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::end() const { return mCatalog.cend(); }

std::optional<cLidarMapConfigCatalog::const_iterator>	cLidarMapConfigFile::find_by_filename(const std::string& measurement_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);

	std::tm time_stamp = extractMeasurementTimestamp(scan_file.filename);

	auto catalog = find(time_stamp.tm_mon + 1, time_stamp.tm_mday);

	if (catalog == mCatalog.end())
	{
		return std::optional<cLidarMapConfigCatalog::const_iterator>();
	}

	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	auto it = (*catalog).second.find_by_filename(filename);

	if (it == (*catalog).second.end())
	{
		while (catalog != mCatalog.begin())
		{
			--catalog;
			it = (*catalog).second.find(filename);
			if (it != (*catalog).second.end())
				return it;
		}

		return std::optional<cLidarMapConfigCatalog::iterator>();
	}

	return it;
}

std::optional<cLidarMapConfigCatalog::iterator> cLidarMapConfigFile::find_by_filename(const std::string& measurement_filename)
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);

	std::tm time_stamp = extractMeasurementTimestamp(scan_file.filename);
	
	auto catalog = find(time_stamp.tm_mon + 1, time_stamp.tm_mday);

	if (catalog == mCatalog.end())
	{
		return std::optional<cLidarMapConfigCatalog::iterator>();
	}

	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	auto it = (*catalog).second.find(filename);

	if (it == (*catalog).second.end())
	{
		while (catalog != mCatalog.begin())
		{
			--catalog;
			it = (*catalog).second.find(filename);
			if (it != (*catalog).second.end())
				return it;
		}

		return std::optional<cLidarMapConfigCatalog::iterator>();
	}

	return it;
}

cLidarMapConfigFile::const_iterator cLidarMapConfigFile::find(const int date) const
{
	if (mCatalog.empty())
		return end();

	if (mCatalog.begin()->first > date)
		return begin();

	cLidarMapConfigFile::const_iterator result = end();

	for (auto it = mCatalog.begin(); it != mCatalog.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find(const int date)
{
	if (mCatalog.empty())
		return end();

	if (mCatalog.begin()->first > date)
		return begin();

	cLidarMapConfigFile::iterator result = end();

	for (auto it = mCatalog.begin(); it != mCatalog.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cLidarMapConfigFile::const_iterator cLidarMapConfigFile::find(const int date, const std::string& measurement_name) const
{
	if (mCatalog.empty())
		return end();

	if (mCatalog.begin()->first > date)
	{
		if (mCatalog.begin()->second.contains(measurement_name))
			return begin();

		return end();
	}

	cLidarMapConfigFile::const_iterator result = end();

	for (auto it = mCatalog.begin(); it != mCatalog.end(); ++it)
	{
		if (it->first > date)
			return result;

		if (it->second.contains(measurement_name))
			result = it;
	}

	return result;
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find(const int date, const std::string& measurement_name)
{
	if (mCatalog.empty())
		return end();

	if (mCatalog.begin()->first > date)
	{
		if (mCatalog.begin()->second.contains(measurement_name))
			return begin();

		return end();
	}

	cLidarMapConfigFile::iterator result = end();

	for (auto it = mCatalog.begin(); it != mCatalog.end(); ++it)
	{
		if (it->first > date)
			return result;

		if (it->second.contains(measurement_name))
			result = it;
	}

	return result;
}

cLidarMapConfigFile::const_iterator cLidarMapConfigFile::find(const int month, const int day) const
{
	return find(month*100 + day);
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find(const int month, const int day)
{
	return find(month * 100 + day);
}

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::find(const int month, const int day, const std::string& measurement_name) const
{
	return find(month * 100 + day, measurement_name);
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find(const int month, const int day, const std::string& measurement_name)
{
	return find(month * 100 + day, measurement_name);
}

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::find_exact(const int date) const
{
	return mCatalog.find(date);
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find_exact(const int date)
{
	return mCatalog.find(date);
}

cLidarMapConfigFile::const_iterator	cLidarMapConfigFile::find_exact(const int month, const int day) const
{
	return find_exact(month * 100 + day);
}

cLidarMapConfigFile::iterator cLidarMapConfigFile::find_exact(const int month, const int day)
{
	return find_exact(month * 100 + day);
}

cLidarMapConfigCatalog& cLidarMapConfigFile::add(const int month, const int day)
{
	int date = month * 100 + day;

	if (mCatalog.contains(date))
	{
		return mCatalog.find(date)->second;
	}

	cLidarMapConfigCatalog catalog = { month, day };

	mCatalog.insert( std::make_pair(date, std::move(catalog)) );

	cLidarMapConfigCatalog& result = mCatalog.find(date)->second;
	result.setDirty(true);

	return result;
}

cLidarMapConfigScan& cLidarMapConfigFile::add(const int month, const int day, const std::string& name)
{
	auto& catalog = add(month, day);

	return catalog.add(name);
}

#endif



