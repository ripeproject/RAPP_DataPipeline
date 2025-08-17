
#pragma once

#include "PointCloudInfo.hpp"
#include "RappPointCloud.hpp"
#include "RappPlot.hpp"

#include <cbdf/ProcessingInfo.hpp>
#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <filesystem>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <set>

// Forward Declarations
class cPlotBoundaries;
class cPlotConfigPlotInfo;
class cProcessingInfoSerializer;
class cExperimentSerializer;
class cPlotConfigScan;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out, const cPlotConfigScan& plot_info);
	~cFileProcessor();

	void savePlotsInSingleFile(bool singleFile);
	void savePlyFiles(bool savePlys);
	void plyUseBinaryFormat(bool binaryFormat);

	void enableSavingFrameIDs(bool enableFrameIDs);
	void enableSavingPixelInfo(bool enablePixelInfo);

	void setAllowedExperimentNames(const std::set<std::string>& experiment_names);

//	void setPlotInfo(std::shared_ptr<cPlotBoundaries> plot_info);

	void process_file();

protected:
	bool open();
	void run();

private:
	bool loadFileData();

private:
	void doPlotSplit();
	void savePlotFile();
	void savePlotFiles();
	void savePlyFiles();

	void writeProcessingInfo(cProcessingInfoSerializer& serializer);
	void writeExperimentInfo(cExperimentSerializer& serializer);

	void fillPlotInformation(cRappPlot* plot, const cPlotConfigPlotInfo& info);

private:
	std::string getExperimentName() const;

private:
	const cPlotConfigScan& mPlotInfo;

	std::set<std::string> mAllowedExperimentNames;

	const int mID;

	bool mSavePlotsInSingleFile = false;
	bool mSavePlyFiles = false;
	bool mPlyUseBinaryFormat = false;

	bool mEnableFrameIDs  = false;
	bool mEnablePixelInfo = false;

	std::uintmax_t mFileSize = 0;
	double		   mFilePos = 0.0;

	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

//	std::shared_ptr<cPlotBoundaries> mPlotInfo;

	std::shared_ptr<cProcessingInfo> mProcessingInfo;
	std::shared_ptr<cExperimentInfo> mExpInfo;
	std::shared_ptr<cPointCloudInfo> mPointCloudInfo;

	std::vector<cRappPlot*> mPlots;
};
