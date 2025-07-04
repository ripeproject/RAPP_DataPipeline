
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

const std::string& cLidarMapConfigFile::getTemporaryFileName() const
{
	return mTmpFileName;
}

bool cLidarMapConfigFile::isDirty() const
{
	for (const auto& date : mCatalog)
	{
		if (date.second.isDirty())
			return true;
	}

	return mOptions.isDirty() || mDefaults.isDirty();
}

void cLidarMapConfigFile::clear()
{
	mFileName.clear();
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

	return true;
}

void cLidarMapConfigFile::save()
{
	if (mFileName.empty()) return;

	nlohmann::json configDoc;

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
}

void cLidarMapConfigFile::save_as(const std::string& file_name)
{
	nlohmann::json configDoc;

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


	mOptions.setDirty(true);

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

bool cLidarMapConfigFile::contains(const std::string& name)
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

	return (*catalog).second.find_by_filename(measurement_filename);
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

	return (*catalog).second.find_by_filename(measurement_filename);
}

cLidarMapConfigFile::const_iterator cLidarMapConfigFile::find(const int date) const
{
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

/*
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
*/
