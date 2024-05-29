
#pragma once

#include "LidarMapConfigOptions.hpp"
#include "LidarMapConfigDefaults.hpp"
#include "LidarMapConfigScan.hpp"

#include <vector>
#include <string>
#include <memory>


class cLidarMapConfigFile
{
public:
	typedef std::vector<cLidarMapConfigScan> LidarScan_t;

	typedef LidarScan_t::iterator				iterator;
	typedef LidarScan_t::const_iterator			const_iterator;

public:
	cLidarMapConfigFile();
	~cLidarMapConfigFile();

	const std::string& getFileName() const;

	bool isDirty() const;

	void clear();

	bool open(const std::string& file_name);

	void save();

	void save_as(const std::string& file_name);

	const cLidarMapConfigOptions& getOptions() const;
	cLidarMapConfigOptions& getOptions();

	const cLidarMapConfigDefaults& getDefaults() const;
	cLidarMapConfigDefaults& getDefaults();

	bool empty() const;
	std::size_t size() const;

	bool contains(const std::string& name);

	const cLidarMapConfigScan& front() const;
	cLidarMapConfigScan& front();

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const_iterator	find_by_filename(const std::string& experiment_filename) const;
	iterator		find_by_filename(const std::string& experiment_filename);

	const_iterator	find(const std::string& name) const;
	iterator		find(const std::string& name);

	cLidarMapConfigScan& add(const std::string& name);
	void remove(const std::string& name);

	const cLidarMapConfigScan& operator[](int index) const;
	cLidarMapConfigScan& operator[](int index);

private:
	std::string mFileName;

	cLidarMapConfigOptions  mOptions;
	cLidarMapConfigDefaults mDefaults;

	LidarScan_t mScans;
};

