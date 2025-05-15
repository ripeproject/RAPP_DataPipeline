
#pragma once

#include "PlotDataConfigOptions.hpp"
#include "PlotDataConfigGroupBy.hpp"
#include "PlotDataConfigHeight.hpp"
#include "PlotDataConfigBiomass.hpp"
#include "PlotDataConfigLAI.hpp"

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

	const std::string& getResultsRootFileName() const;

	bool hasExperimentTitle() const;
	const std::string& getExperimentTitle() const;

	const cPlotDataConfigOptions& getOptions() const;
	cPlotDataConfigOptions& getOptions();

	const cPlotDataConfigGroupBy& getGroupBy() const;
	cPlotDataConfigGroupBy& getGroupBy();

	const cPlotDataConfigHeight& getHeightParameters() const;
	cPlotDataConfigHeight& getHeightParameters();

	const cPlotDataConfigBiomass& getBiomassParameters() const;
	cPlotDataConfigBiomass& getBiomassParameters();

	const cPlotDataConfigLAI& getLaiParameters() const;
	cPlotDataConfigLAI& getLaiParameters();


private:
	std::string mFileName;
	std::string mTmpFileName;

	std::string mResultsRootFileName;
	std::string mExperimentTitle;

	cPlotDataConfigOptions mOptions;
	cPlotDataConfigGroupBy mGroupBy;
	cPlotDataConfigHeight  mHeightParameters;
	cPlotDataConfigBiomass mBiomassParameters;
	cPlotDataConfigLAI	   mLaiParameters;
};

