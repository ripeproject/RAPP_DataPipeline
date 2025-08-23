
#pragma once

#include "LidarMapConfigOptions.hpp"
#include "LidarMapConfigDefaults.hpp"
#include "LidarMapConfigScan.hpp"

#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>
#include <optional>
#include <system_error>


class cLidarMapConfigFile
{
public:
	typedef std::vector<cLidarMapConfigScan> LidarScan_t;

	typedef LidarScan_t::iterator				iterator;
	typedef LidarScan_t::const_iterator			const_iterator;

/*
	typedef std::map<int, cLidarMapConfigCatalog> LidarCatalog_t;

	typedef LidarCatalog_t::iterator				iterator;
	typedef LidarCatalog_t::const_iterator			const_iterator;
*/

public:
	cLidarMapConfigFile();
	~cLidarMapConfigFile();

	const std::string& getFileName() const;
	const std::string& getTemporaryFileName() const;

	bool isDirty() const;

	void clear();

	bool open(const std::string& file_name);

	void save();

	void save_as(const std::string& file_name);

	bool open_temporary_file(const std::string& file_name);
	void save_temporary_file();

	void clearAllowedExperimentNames();
	std::set<std::string> getAllowedExperimentNames() const;
	void setAllowedExperimentNames(std::set<std::string> names);

	const cLidarMapConfigOptions& getOptions() const;
	cLidarMapConfigOptions& getOptions();

	const cLidarMapConfigDefaults& getDefaults() const;
	cLidarMapConfigDefaults& getDefaults();

	bool empty() const;
	std::size_t size() const;

	bool contains(const std::string& name) const;
	bool contains_by_filename(const std::string& measurement_filename) const;

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const cLidarMapConfigScan& front() const;
	cLidarMapConfigScan& front();

	const cLidarMapConfigScan& back() const;
	cLidarMapConfigScan& back();

	const_iterator	find_by_filename(const std::string& experiment_filename) const;
	iterator		find_by_filename(const std::string& experiment_filename);

	const_iterator	find_by_measurement_name(const std::string& name) const;
	iterator		find_by_measurement_name(const std::string& name);

	cLidarMapConfigScan& add(const std::string& name);
	void remove(const std::string& name);

	const cLidarMapConfigScan& operator[](int index) const;
	cLidarMapConfigScan& operator[](int index);

private:
	std::string mFileName;
	std::string mTmpFileName;

	bool mIsDirty = false;

	std::set<std::string> mAllowedExperimentNames;

	cLidarMapConfigOptions  mOptions;
	cLidarMapConfigDefaults mDefaults;

	std::set<std::string> mMeasurementNames;

	LidarScan_t mScans;

/*
	LidarCatalog_t mCatalog;
*/
};

