
#pragma once

#include "LidarMapConfigFile.hpp"
#include "LidarMapConfigKinematics.hpp"

#include "lidar2pointcloud.hpp"

#include <cbdf/ProcessingInfoSerializer.hpp>

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <memory>
#include <set>


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

	void setAllowedExperimentNames(const std::set<std::string>& experiment_names);

	/**
	 * Set the default parameters to use in computing the point cloud.
	 *
	 * Some of the defaults are not used depending on the data in the 
	 * Ceres file.
	 */
	void setDefaults(const cLidarMapConfigDefaults& defaults);
	void setParameters(const cLidarMapConfigKinematicParameters& parameters);

	void setSensorMountPitch_deg(double pitch_deg);
	void setSensorMountRoll_deg(double roll_deg);
	void setSensorMountYaw_deg(double yaw_deg);

	void setReferencePoint(const std::optional<rfm::rappPoint_t>& ref_point);

	void setMinDistance_m(double dist_m);
	void setMaxDistance_m(double dist_m);
	void setMinAzimuth_deg(double az_deg);
	void setMaxAzimuth_deg(double az_deg);
	void setMinAltitude_deg(double alt_deg);
	void setMaxAltitude_deg(double alt_deg);

	void process_file();

private:
	void savePointCloudFile(const cLidar2PointCloud& data, const cRappPointCloud& pc);
	void savePlyFiles(const cRappPointCloud& pc);

	void writeProcessingInfo(const cProcessingInfo& info, cProcessingInfoSerializer& serializer);
	void writeExperimentInfo(const cExperimentInfo& info, cExperimentSerializer& serializer);

private:
	std::string getExperimentName(const cFieldScanDataModel* data) const;

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

	std::set<std::string> mAllowedExperimentNames;


	// Dolly - Sensor Mounting Parameters
	double mSensorMountPitch_deg;
	double mSensorMountRoll_deg;
	double mSensorMountYaw_deg;

	// Reference point to normalize scans
	std::optional<rfm::rappPoint_t> mReferencePoint;

	// LiDAR Sensor Limits
	double mMinDistance_m;
	double mMaxDistance_m;
	double mMinAzimuth_deg;
	double mMaxAzimuth_deg;
	double mMinAltitude_deg;
	double mMaxAltitude_deg;


	cLidarMapConfigDefaults mDefaults;
	cLidarMapConfigKinematicParameters mParameters;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;
};
