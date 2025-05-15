
#pragma once

#include "RappPlot.hpp"
#include "PlotData.hpp"

#include <cbdf/PlotInfo.hpp>
#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <filesystem>
#include <string>
#include <memory>
#include <vector>
#include <list>

// Forward Declarations
class cPlotBoundaries;
class cPlotConfigPlotInfo;
class cPlotDataConfigFile;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
		cPlotData& results, const cPlotDataConfigFile& plot_cfg);

	~cFileProcessor();


	void process_file();

protected:
	bool open();
	void run();

private:
	bool loadFileData();

private:
	void fillGroupInfo();
	void fillPlotInfo();
	void computePlotHeights();
	void computePlotBioMasses();
	void computePlotLAIs();

private:
	int mDayOfYear = 0;
	cPlotData& mResults;

	const cPlotDataConfigFile& mConfigInfo;

	const int mID;

	std::uintmax_t mFileSize = 0;
	double		   mFilePos = 0.0;

	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;


	std::shared_ptr<cExperimentInfo> mExpInfo;
	std::shared_ptr<cPlotInfo>		 mPlotInfo;

	std::vector<cRappPlot*> mPlots;
};
