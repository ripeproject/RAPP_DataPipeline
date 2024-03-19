
#pragma once

#include "datatypes.hpp"

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

	/**
	 */
	void enableTranslateToGroundData(bool enable);
	void setTranslateThreshold_pct(double threshold_pct);

	/**
	 */
	void enableRotationToGroundData(bool enable);
	void setRotationThreshold_pct(double threshold_pct);

	void setDollyPath(const std::vector<rfm::sDollyInfo_t>& path);

	const cRappPointCloud& getPointCloud() const;

	void run();

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
	bool computePointCloud();
	bool shiftPointCloudToAGL();

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
	double mMinDistance_mm = 1;
	double mMaxDistance_mm = 1'000'000.0;

	double mMinEncoder_rad = 0.0;
	double mMaxEncoder_rad = 2.0 * std::numbers::pi;

	double mMinAltitude_rad = -std::numbers::pi;
	double mMaxAltitude_rad = +std::numbers::pi;

	bool	mEnableTranslateToGroundData = false;
	double	mTranslationThreshold_pct = 1.0;

	bool	mAltitudeRangeValid = true;
	bool	mAzimuthRangeValid = true;
	bool	mAllowRotationToGroundData = true;

	bool	mEnableRotationToGroundData = false;
	double	mRotationThreshold_pct = 1.0;

	std::vector<rfm::sDollyInfo_t> mDollyPath;

	cRappPointCloud mPointCloud;

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