
#pragma once

#include "PointCloudSerializer.hpp"
#include "PointCloudTypes.hpp"
#include "PointCloud.hpp"

#include "Kinematics.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>
#include <ouster/simple_blas.h>

#include <filesystem>
#include <string>
#include <numbers>
#include <optional>
#include <fstream>
#include <memory>


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
	 * Set the azimuth bounds to consider valid data
	 *
	 * The azimuth angle (zero) starts at the connector and increases in the
	 * counter-clock-wise direction.
	 * 		min_azimuth_deg < azimuth_deg < max_azimuth_deg
	 *
	 * @param	min_azimuth_deg	: The minimum azimuth to be considered valid
	 * @param	max_azimuth_deg	: The maximum azimuth to be considered valid
	 */
	static void setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg);

	/**
	 * Set the altitude bounds to consider valid data
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_altitude_deg	: The minimum altitude to be considered valid
	 * @param	max_altitude_deg	: The maximum altitude to be considered valid
	 */
	static void setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg);

	static void saveAggregatePointCloud();
	static void saveReducedPointCloud();


public:
	cLidar2PointCloud();
	~cLidar2PointCloud();

	/**
	 * Set the kinematic type to apply to the pointcloud data.
	 */
	void setKinematicModel(std::unique_ptr<cKinematics> model);
	const cKinematics* getcKinematicModel() const;

	/*
	 * Does the kinematics model require a pass through the
	 * data file to precompute telemetry data.
	 */
	bool requiresTelemetryPass();
	void telemetryPassComplete();

	/*
	 * Attach/Detach parser to for the kinematics model.
	 */
	void attachKinematicParsers(cBlockDataFileReader& file);
	void detachKinematicParsers(cBlockDataFileReader& file);

	/*
	 * Attach/Detach parser/serializer to for the transform phase.
	 */
	void attachTransformParsers(cBlockDataFileReader& file);
	void detachTransformParsers(cBlockDataFileReader& file);

	void attachTransformSerializers(cBlockDataFileWriter& file);
	void detachTransformSerializers(cBlockDataFileWriter& file);

	/*
	 * Write any header data
	 */
	void writeHeader();

	/*
	 * Write and close any pointcloud data
	 */
	void writeAndCloseData();

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

    void computePointCloud(const cOusterLidarData& data);

private:
	static double mMinDistance_m;
	static double mMaxDistance_m;

	static bool mAggregatePointCloud;
	static bool mSaveReducedPointCloud;

private:
	std::optional<ouster::beam_intrinsics_2_t>   mBeamIntrinsics;
	std::optional<ouster::lidar_intrinsics_2_t>  mLidarIntrinsics;
	std::optional<ouster::lidar_data_format_2_t> mLidarDataFormat;

	ouster::imu_intrinsics_2_t			mImuIntrinsics;
	ouster::cTransformMatrix<double>	mImuTransform;
	ouster::cRotationMatrix<double>		mImuToSensor;

private:
	std::unique_ptr<cKinematics>	mKinematic;

    ouster::matrix_col_major<pointcloud::sCloudPoint_t> mCloud;

	cPointCloud mPointCloud;


    std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;

	uint64_t mStartTimestamp_ns = 0;
};
