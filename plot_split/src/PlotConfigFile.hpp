
#pragma once

#include "PlotConfigOptions.hpp"
#include "PlotConfigScan.hpp"

#include <vector>
#include <string>
#include <memory>


class cPlotConfigFile
{
public:
	typedef std::vector<cPlotConfigScan> PlotScan_t;

	typedef PlotScan_t::iterator				iterator;
	typedef PlotScan_t::const_iterator			const_iterator;

public:
	cPlotConfigFile();
	~cPlotConfigFile();

	const std::string& getFileName() const;

	bool isDirty() const;

	void clear();

	bool open(const std::string& file_name);

	void save();

	void save_as(const std::string& file_name);

	const cPlotConfigOptions& getOptions() const;
	cPlotConfigOptions& getOptions();

	bool empty() const;
	std::size_t size() const;

	bool contains(const std::string& name);

	const cPlotConfigScan& front() const;
	cPlotConfigScan& front();

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const_iterator	find_by_filename(const std::string& experiment_filename) const;
	iterator		find_by_filename(const std::string& experiment_filename);

	const_iterator	find(const std::string& name) const;
	iterator		find(const std::string& name);

	cPlotConfigScan& add(const std::string& name);
	void remove(const std::string& name);

	const cPlotConfigScan& operator[](int index) const;
	cPlotConfigScan& operator[](int index);

private:
	std::string mFileName;

	cPlotConfigOptions mOptions;

	PlotScan_t mScans;
};

