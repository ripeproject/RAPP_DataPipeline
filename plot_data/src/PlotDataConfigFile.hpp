
#pragma once

#include "PlotDataConfigHeight.hpp"

#include <vector>
#include <string>
#include <memory>


class cPlotDataConfigFile
{
public:

public:
	cPlotDataConfigFile();
	~cPlotDataConfigFile();

	const std::string& getFileName() const;
	const std::string& getTemporaryFileName() const;

	bool isDirty() const;

	void clear();

	bool open(const std::string& file_name);

	void save();

	void save_as(const std::string& file_name);

	bool open_temporary_file(const std::string& file_name);
	void save_temporary_file();

	const cPlotDataConfigHeight& getHeightParameters() const;
	cPlotDataConfigHeight& getHeightParameters();


private:
	std::string mFileName;
	std::string mTmpFileName;

	cPlotDataConfigHeight mHeightParameters;
};

