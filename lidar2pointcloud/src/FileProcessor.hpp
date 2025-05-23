
#pragma once

#include "LidarMapConfigFile.hpp"

#include "lidar2pointcloud.hpp"

#include <cbdf/ProcessingInfoSerializer.hpp>

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <memory>


// Forward Declarations
class cProcessingInfo;
class cProcessingInfoSerializer;
class cExperimentInfo;
class cExperimentSerializer;


class cFileProcessor
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in, std::filesystem::path out);
	~cFileProcessor();

	void saveCompactPointCloud(bool compact);
	void saveFrameIds(bool save);
	void savePixelInfo(bool save);

	void savePlyFiles(bool savePlys);
	void plyUseBinaryFormat(bool binaryFormat);

	/**
	 * Set the default parameters to use in computing the point cloud.
	 *
	 * Some of the defaults are not used depending on the data in the 
	 * Ceres file.
	 */
	void setDefaults(const cLidarMapConfigDefaults& defaults);
	void setParameters(const cLidarMapConfigScan& parameters);


	void process_file();

private:
	void savePointCloudFile(const cLidar2PointCloud& data, const cRappPointCloud& pc);
	void savePlyFiles(const cRappPointCloud& pc);

	void writeProcessingInfo(const cProcessingInfo& info, cProcessingInfoSerializer& serializer);
	void writeExperimentInfo(const cExperimentInfo& info, cExperimentSerializer& serializer);

private:
	const int mID;

	bool mSaveCompactPointCloud = true;
	bool mSavePlyFiles = false;
	bool mPlyUseBinaryFormat = false;

	bool mSaveFrameIds = false;
	bool mSavePixelInfo = false;

	bool mHasGroundData = false;

	bool mFlattenPointCloud = true;
	double mMaxAngle_deg = 10.0;
	double mThreshold_pct = 0.1;

	cLidarMapConfigDefaults mDefaults;
	cLidarMapConfigScan mParameters;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;
};
