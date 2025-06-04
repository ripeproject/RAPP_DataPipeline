
#pragma once

#include "datatypes.hpp"
#include "PointCloudUtilTypes.hpp"
#include "KinematicDataTypes.hpp"

#include "RappPointCloud.hpp"

#include <cbdf/SsnxInfoTypes.hpp>
#include <cbdf/SpiderCamInfoTypes.hpp>

#include <ouster/OusterLidarData.h>

#include <string>
#include <numbers>
#include <vector>
#include <deque>


class cPointCloudGenerator
{

public:
	struct sTranslationInfo_t
	{
		double displacement_mm = 0;
		double height_mm = 0;
	};

	struct sRotationInfo_t
	{
		double displacement_mm = 0;
		double pitch_deg = 0;
		double roll_deg = 0;
	};

public:
	cPointCloudGenerator() = default;

	/**
	 * Returns the true if lidar data has been loaded into the generator
	 */
	bool hasData() const;

	/**
	 * Returns the total time of the scan in seconds.
	 * 
	 * The difference between the last timestamp and the first
	 */
	double getScanTime_sec() const;

	double	getComputedStartYaw_deg() const;
	double	getComputedEndYaw_deg() const;


	/**
	 * Set the valid range in meters
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_dist_m	: The minimum distance to be considered valid
	 * @param	max_dist_m	: The maximum distance to be considered valid
	 */
	void resetValidRange();
	void setValidRange_m(double min_dist_m, double max_dist_m);
	std::pair<double, double> getValidRange_m() const;

	/**
	 * Set the azimuth bounds to consider valid data
	 *
	 * The azimuth angle (zero) starts at the connector and increases in the
	 * counter-clock-wise direction.
	 * 		min_azimuth_deg < azimuth_deg < max_azimuth_deg
	 *
	 * @param	min_azimuth_deg	: The minimum azimuth to be considered valid
	 * @param	max_azimuth_deg	: The maximum azimuth to be considered valid
	 */
	void resetAzimuthWindow();
	void setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg);
	std::pair<double, double> getAzimuthWindow_deg() const;

	/**
	 * Set the altitude bounds to consider valid data
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_altitude_deg	: The minimum altitude to be considered valid
	 * @param	max_altitude_deg	: The maximum altitude to be considered valid
	 */
	void resetAltitudeWindow();
	void setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg);
	std::pair<double, double> getAltitudeWindow_deg() const;

	/**
	 */
	eTranslateToGroundModel getTranslateToGroundModel() const;

	void setTranslateToGroundModel(eTranslateToGroundModel model);
	void setTranslateDistance_m(double distance_m);
	void setTranslateThreshold_pct(double threshold_pct);
	void setTranslateInterpTable(const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& table);

	/**
	 */
	eRotateToGroundModel getRotationToGroundModel() const;

	void setRotationToGroundModel(eRotateToGroundModel model);
	void setRotationAngles_deg(double pitch_deg, double roll_deg);
	void setRotationThreshold_pct(double threshold_pct);
	void setRotateInterpTable(const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& table);

	/**
	 */
	void enableAutomaticYawCorrection(bool enable);
	void setPlantHeightThreshold_pct(double threshold_pct);
	void setNearestNeighborThreshold_mm(double threshold_mm);
	void setComputedYawLimits(double lower_limit_deg, double upper_limit_deg);

	void setDollyPath(const std::vector<kdt::sDollyInfo_t>& path);

	const std::vector<kdt::sDollyInfo_t>& getComputedDollyPath() const;

	bool hasPointCloud() const;
	const cRappPointCloud& getPointCloud() const;
	cRappPointCloud& getPointCloud();

	bool recordingFittingData() const;
	void recordFittingData(bool record);

	double getAvgTranslationHeight_m() const;
	const std::vector<sTranslationInfo_t>& getHeights() const;

	double getAvgRotationPitch_deg() const;
	double getAvgRotationRoll_deg() const;
	const std::vector<sRotationInfo_t>& getAngles() const;

	bool referencePointValid() const;
	rfm::rappPoint_t referencePoint() const;
	void setReferencePoint(rfm::rappPoint_t point, bool valid = true);

	bool computePointCloud(int id);

	void clearLidarData();

public:
	void fixTimestampInfo(double updateRate_Hz, uint64_t start_timestamp_ns = 0);

protected:
	void clear();

	void setDimensions(uint16_t columns_per_frame, uint16_t pixels_per_column);

	void setBeamOffset(double lidar_to_beam_origins_mm);

	void setBeamAzimuthAngles_deg(const std::vector<double>& azimuth_angles_deg);
	void setBeamAltitudeAngles_deg(const std::vector<double>& altitude_angles_deg);

	void setLidarToSensorTransform(const std::vector<double>& lidar_to_sensor_transform);

	void addLidarData(const cOusterLidarData& data);

private:
	bool computeYawCorrections();
	bool shiftPointCloudToAGL();
	bool updateDollyPath();

public:
	struct sPoint_t
	{
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;

		sPoint_t() = default;
		sPoint_t(double i, double j, double k) : x(i), y(j), z(k) {}
	};

private:
	struct sLUT_t
	{
		std::vector<sPoint_t> unitVectors;
		std::vector<sPoint_t> offsets;
	};

	sLUT_t generateLookupTable();


private:
	bool mAbort = false;

private:
	double mMinDistance_mm = 1;
	double mMaxDistance_mm = 1'000'000.0;

	double mMinEncoder_rad = 0.0;
	double mMaxEncoder_rad = 2.0 * std::numbers::pi;

	double mMinAltitude_rad = -std::numbers::pi;
	double mMaxAltitude_rad = +std::numbers::pi;

	bool	mAltitudeRangeValid = true;
	bool	mAzimuthRangeValid = true;
	bool	mAllowRotationToGroundData = true;

	eTranslateToGroundModel	mTranslateToGroundModel = eTranslateToGroundModel::NONE;
	double	mTranslationDistance_m = 0.0;
	double	mTranslationThreshold_pct = 1.0;
	std::vector<pointcloud::sPointCloudTranslationInterpPoint_t> mTranslateInterpTable;

	eRotateToGroundModel	mRotateToGroundModel = eRotateToGroundModel::NONE;
	double	mRotationPitch_deg = 0.0;
	double	mRotationRoll_deg = 0.0;
	double	mRotationThreshold_pct = 1.0;
	std::vector<pointcloud::sPointCloudRotationInterpPoint_t>    mRotateInterpTable;

	bool	mEnableAutomaticYawCorrection = false;
	double  mPlantHeightThreshold_pct = 50.0;
	double  mMaxSeparation_mm = 1000.0;

	double	mComputedStartYaw_deg = 0.0;
	double	mComputedEndYaw_deg = 0.0;

	double	mLowerLimitYaw_deg = -30.0;
	double	mUpperLimitYaw_deg = 30.0;

	std::vector<kdt::sDollyInfo_t> mDollyPath;

	std::vector<kdt::sDollyInfo_t> mComputedDollyPath;

	cRappPointCloud mPointCloud;

private:
	bool mRecordFittingData = false;

	std::vector<sTranslationInfo_t> mHeights;
	std::vector<sRotationInfo_t> mAngles;

private:
    uint16_t mColumnsPerFrame = 0;
    uint16_t mPixelsPerColumn = 0;

	double mLidarToBeamOrigins_mm = 0.0;
	std::vector<double> mAzimuthAngles_rad;
	std::vector<double> mAltitudeAngles_rad;

	std::vector<double> mLidarToSensorTransform;

	std::deque<cOusterLidarData> mLidarData;

	friend class cOusterInfo;
};