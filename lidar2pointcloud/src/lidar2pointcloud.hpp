
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
	cLidar2PointCloud();
	~cLidar2PointCloud();

	/**
	 * Set the valid range in meters
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_dist_m	: The minimum distance to be considered valid
	 * @param	max_dist_m	: The maximum distance to be considered valid
	 */
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
	void setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg);

	void setInitialPosition_m(double x_m, double y_m, double z_m);
	void setFinalPosition_m(double x_m, double y_m, double z_m);

	void setDollySpeed(double Vx_mmps, double Vy_mmps, double Vz_mmps);

	void setSensorMountOrientation(double yaw_deg, double pitch_deg, double roll_deg);

	void setOrientationOffset_deg(double yaw_deg, double pitch_deg, double roll_deg);

	void setInitialOffset_deg(double yaw_deg, double pitch_deg, double roll_deg);
	void setFinalOffset_deg(double yaw_deg, double pitch_deg, double roll_deg);

	void enableTranslateToGround(bool enable, double threshold_pct);
	void enableRotateToGround(bool enable, double threshold_pct);

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
	 * Write and clear any pointcloud data
	 */
	void writeAndClearData();

private:
	void writeReducedPointCloud(uint16_t frameID, uint64_t timestamp_ns, 
								std::size_t columns_per_frame, std::size_t pixels_per_column);
	void writeSensorPointCloud(uint16_t frameID, uint64_t timestamp_ns,
								std::size_t columns_per_frame, std::size_t pixels_per_column);

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
	double mMinDistance_m = 0.001;
	double mMaxDistance_m = 1000.0;

	double mMinEncoder_rad = 0.0;
	double mMaxEncoder_rad = 2.0 * std::numbers::pi;

	double mMinAltitude_rad = -std::numbers::pi;
	double mMaxAltitude_rad = +std::numbers::pi;

	double mStartX_mm = 0.0;
	double mStartY_mm = 0.0;
	double mStartZ_mm = 0.0;

	double mEndX_mm = 0.0;
	double mEndY_mm = 0.0;
	double mEndZ_mm = 0.0;

	double mVx_mmps = 0.0;
	double mVy_mmps = 0.0;
	double mVz_mmps = 0.0;

	double mSensorMountPitch_deg = -90.0;
	double mSensorMountRoll_deg = 0.0;
	double mSensorMountYaw_deg = 90.0;

	double mStartPitchOffset_deg = 0.0;
	double mStartRollOffset_deg = 0.0;
	double mStartYawOffset_deg = 0.0;

	double mEndPitchOffset_deg = 0.0;
	double mEndRollOffset_deg = 0.0;
	double mEndYawOffset_deg = 0.0;

	bool   mTranslateToGround = true;
	double mTranslateThreshold_pct = 1.0;
	bool   mRotateToGround = true;
	double mRotateThreshold_pct = 1.0;


private:
	std::optional<ouster::beam_intrinsics_2_t>   mBeamIntrinsics;
	std::optional<ouster::lidar_intrinsics_2_t>  mLidarIntrinsics;
	std::optional<ouster::lidar_data_format_2_t> mLidarDataFormat;
	std::vector<int> mPixelShiftByRow;

	ouster::imu_intrinsics_2_t			mImuIntrinsics;
	ouster::cTransformMatrix<double>	mImuTransform;
	ouster::cRotationMatrix<double>		mImuToSensor;

private:
	std::unique_ptr<cKinematics>	mKinematic;

	/*
	 * The rotation needed to convert sensor orientation
	 * to the spidercam South\East\Up coordination system
	 */
	ouster::cRotationMatrix<double> mSensorToSEU;

    ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t> mCloud;

	cPointCloud_SensorInfo mPointCloud;

	std::vector<bool>     mExcludePoint;
	std::vector<sPoint_t> mUnitVectors;
    std::vector<sPoint_t> mOffsets;
	std::vector<double>   mTheta_rad;
	std::vector<double>   mPhi_rad;

	uint64_t mStartTimestamp_ns = 0;
	uint32_t mFrameID = 0;
	uint64_t mTimeStep_ns = 100'000'000;
};
