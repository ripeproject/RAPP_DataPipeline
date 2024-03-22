
#pragma once

#include "PointCloudInfo.hpp"
#include "ProcessingInfo.hpp"
#include "RappPointCloud.hpp"
#include "RappPlot.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentInfo.hpp>

#include <filesystem>
#include <string>
#include <memory>
#include <vector>
#include <list>

// Forward Declarations
class cPlotBoundaries;
class cProcessingInfoSerializer;
class cExperimentSerializer;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void savePlotsInSingleFile(bool singleFile);
	void savePlyFiles(bool savePlys);
	void plyUseBinaryFormat(bool binaryFormat);

	void enableSavingFrameIDs(bool enableFrameIDs);
	void enableSavingPixelInfo(bool enablePixelInfo);

	void setPlotInfo(std::shared_ptr<cPlotBoundaries> plot_info);

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

private:
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

	std::shared_ptr<cPlotBoundaries> mPlotInfo;

	std::shared_ptr<cProcessingInfo> mProcessingInfo;
	std::shared_ptr<cExperimentInfo> mExpInfo;
	std::shared_ptr<cPointCloudInfo> mPointCloudInfo;

	std::vector<cRappPlot*> mPlots;
};
