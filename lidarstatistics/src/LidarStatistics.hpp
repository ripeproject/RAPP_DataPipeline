
#pragma once

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <numbers>
#include <optional>
#include <fstream>


struct sPoint_t
{
    double x_mm;
    double y_mm;
    double z_mm;

    sPoint_t() : x_mm(0), y_mm(0), z_mm(0) {}
    sPoint_t(double i, double j, double k) : x_mm(i), y_mm(j), z_mm(k) {}
};

struct sCloudPoint_t
{
	double x_m;
	double y_m;
	double z_m;
	double r_m;

	sCloudPoint_t() : x_m(0), y_m(0), z_m(0), r_m(0) {}
	sCloudPoint_t(double i, double j, double k) : x_m(i), y_m(j), z_m(k), r_m(0) {}
};


class cLidarStatistics : public cOusterParser
{
public:
	/**
	 * Set the valid range in meters
	 * 
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 * 
	 * @param	min_dist_m	: The minimum distance to be considered valid
	 * @param	max_dist_m	: The maximum distance to be considered valid
	 */
	static void setValidRange_m(double min_dist_m, double max_dist_m);

	/**
	 * Sets the sensor orientation
	 * 
	 * Compute the rotation matrix needed to convert the pointcloud from
	 * the LiDAR sensor coordinate system to a earth East-North-Up (ENU)
	 * coordinate system.
	 * 
	 * @param	yaw_deg		: The yaw angle of the sensor referenced to the east direction
	 * @param	pitch_deg	: The pitch angle of the sensor
	 * @param	roll_deg	: The roll angle of the sensor
	 */
	static void setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg);

public:
	cLidarStatistics();
	~cLidarStatistics();

	void setOutputPath(std::filesystem::path path);

	double mX_g = 0.0;
	double mY_g = 0.0;
	double mZ_g = 0.0;
	double mPitchRate_deg_per_sec = 0.0;
	double mRollRate_deg_per_sec = 0.0;
	double mYawRate_deg_per_sec = 0.0;

private:
	void onConfigParam(ouster::config_param_2_t config_param) override;
	void onSensorInfo(ouster::sensor_info_2_t sensor_info) override;
	void onTimestamp(ouster::timestamp_2_t timestamp) override;
	void onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) override;
	void onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) override;
	void onMultipurposeIo(ouster::multipurpose_io_2_t io) override;
	void onNmea(ouster::nmea_2_t nmea) override;
	void onTimeInfo(ouster::time_info_2_t time_info) override;
	void onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) override;
	void onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics) override;
	void onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) override;
	void onLidarDataFormat(ouster::lidar_data_format_2_t format) override;
	void onImuData(ouster::imu_data_t data) override;
	void onLidarData(cOusterLidarData data) override;

private:
    void createXyzLookupTable(const ouster::beam_intrinsics_2_t& beam,
                              const ouster::lidar_intrinsics_2_t& lidar,
                              const ouster::lidar_data_format_2_t& format);

    void computerPointCloud(const cOusterLidarData& data);

private:
	static double mMinDistance_m;
	static double mMaxDistance_m;

	static ouster::cRotationMatrix<double> mSensorToENU;

private:
	std::optional<ouster::beam_intrinsics_2_t>   mBeamIntrinsics;
	std::optional<ouster::lidar_intrinsics_2_t>  mLidarIntrinsics;
	std::optional<ouster::lidar_data_format_2_t> mLidarDataFormat;

	ouster::imu_intrinsics_2_t			mImuIntrinsics;
	ouster::cTransformMatrix<double>	mImuTransform;
	ouster::cRotationMatrix<double>		mImuToSensor;

private:
    ouster::matrix_col_major<sCloudPoint_t> mCloud;

    std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;

	std::filesystem::path mOutDir;
	std::string mBaseFilename;

	std::ofstream mAccelerations;
	std::ofstream mOrientationRates;
	std::ofstream mPosition;

	uint64_t mLidarStartTimestamp_ns = 0;
	uint64_t mAccelStartTimestamp_ns = 0;
	uint64_t mGyroStartTimestamp_ns = 0;

	/**
	 * Data used to compute average imu data
	 */
	std::size_t mCount = 0;
	double mAvg_X_g = 0.0;
	double mAvg_Y_g = 0.0;
	double mAvg_Z_g = 0.0;
	double mAvgPitchRate_deg_per_sec = 0.0;
	double mAvgRollRate_deg_per_sec = 0.0;
	double mAvgYawRate_deg_per_sec = 0.0;

	/**
	 * Imu Histogram Data for packet arrives
	 */
	int mImuCount = 0;
	std::array<int, 20> mImuHistogram = { 0 };

	/**
	 * LiDAR center point statistics
	 */
	uint32_t mCenterCol = 0;
	uint32_t mCenterRow = 0;
	double mSumCenterRange = 0.0;
	double mSumCenterRange2 = 0.0;
};
