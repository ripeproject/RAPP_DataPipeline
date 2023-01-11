
#pragma once

#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <numbers>
#include <optional>


struct sPoint_t
{
    double x;
    double y;
    double z;

    sPoint_t() : x(0), y(0), z(0) {}
    sPoint_t(double i, double j, double k) : x(i), y(j), z(k) {}
};

struct sCloudPoint_t : public ouster::lidar_data_block_t
{
    double x;
    double y;
    double z;

    sCloudPoint_t() : x(0), y(0), z(0) {}
};


class cLidar2PointCloud : public cOusterParser
{
public:
	static void setValidRange_m(double min_dist_m, double max_dist_m);
	static void setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg);

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

private:
	std::optional<ouster::beam_intrinsics_2_t>   mBeamIntrinsics;
	std::optional<ouster::lidar_intrinsics_2_t>  mLidarIntrinsics;
	std::optional<ouster::lidar_data_format_2_t> mLidarDataFormat;

	std::optional<ouster::imu_intrinsics_2_t>    mImuIntrinsics;

private:
    ouster::matrix_col_major<sCloudPoint_t> mCloud;

    std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;

private:
	cPointCloudSerializer mSerializer;
};
