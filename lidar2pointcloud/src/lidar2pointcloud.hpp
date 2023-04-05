
#pragma once

#include "PointCloudSerializer.hpp"
#include "PointCloudTypes.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <numbers>
#include <optional>
#include <fstream>


enum class Kinematics {NONE, CONSTANT, DOLLY, GPS, SLAM};

struct sPoint_t
{
    double x;
    double y;
    double z;

    sPoint_t() : x(0), y(0), z(0) {}
    sPoint_t(double i, double j, double k) : x(i), y(j), z(k) {}
};


class cLidar2PointCloud : public cOusterParser, public cPointCloudSerializer
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

	static void saveReducedPointCloud();

	static void setKinematicType(Kinematics kinematic);

public:
	cLidar2PointCloud();
	~cLidar2PointCloud();

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

	static bool mSaveReducedPointCloud;

	static Kinematics mKinematicType;

private:
	std::optional<ouster::beam_intrinsics_2_t>   mBeamIntrinsics;
	std::optional<ouster::lidar_intrinsics_2_t>  mLidarIntrinsics;
	std::optional<ouster::lidar_data_format_2_t> mLidarDataFormat;

	ouster::imu_intrinsics_2_t			mImuIntrinsics;
	ouster::cTransformMatrix<double>	mImuTransform;
	ouster::cRotationMatrix<double>		mImuToSensor;

private:
    ouster::matrix_col_major<pointcloud::sCloudPoint_t> mCloud;

    std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;

	uint64_t mStartTimestamp_ns = 0;
};
