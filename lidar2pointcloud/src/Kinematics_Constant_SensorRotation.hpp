
#pragma once

#include "Kinematics_Constant.hpp"

#include <cbdf/OusterParser.hpp>

#include <cstdint>


class cKinematics_Constant_SensorRotation : public cKinematics_Constant, public cOusterParser
{
public:
	cKinematics_Constant_SensorRotation(double Vx_mmps, double Vy_mmps, double Vz_mmps);

	void setInitialOrientation_deg(double yaw_deg, double pitch_deg, double roll_deg);
	void setFinalOrientation_deg(double yaw_deg, double pitch_deg, double roll_deg);

	void addOrientationPoint(double time_sec, double yaw_deg, double pitch_deg, double roll_deg);

	void setRotationalRates_deg(double yawRate_dps, double pitchRate_dps, double rollRate_dps);

	void writeHeader(cPointCloudSerializer& serializer) override;

	/*
	 * Return true, we will need to precompute sensor rotational telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;
	void telemetryPassComplete() override;

	/*
	 * Attach any parsers to the data file.
 	 */
	void attachKinematicParsers(cBlockDataFileReader& file) override;
	void detachKinematicParsers(cBlockDataFileReader& file) override;

	/*
	 * Transform the point cloud coordinated based on kinematic model
	 */
	bool transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud) override;

protected:
	/** Ouster Parser **/
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
	double mStartPitch_deg = 0.0;
	double mStartRoll_deg = 0.0;
	double mStartYaw_deg = 0.0;

	double mEndPitch_deg = 0.0;
	double mEndRoll_deg = 0.0;
	double mEndYaw_deg = 0.0;

	bool   mHaveRotationalRates = false;
	double mPitchRate_dps = 0.0;
	double mRollRate_dps = 0.0;
	double mYawRate_dps = 0.0;

	double mPitchAccel_dps2 = 0.0;
	double mRollAccel_dps2 = 0.0;
	double mYawAccel_dps2 = 0.0;

	uint64_t mStartTimestamp_ns = 0;
	uint64_t mLastTimestamp_ns = 0;

	struct sSensorOrientation_t
	{
		double timestamp_us = 0.0;

		double pitch_deg = 0.0;
		double roll_deg = 0.0;
		double yaw_deg = 0.0;

		double pitchRate_dps = 0.0;
		double rollRate_dps = 0.0;
		double yawRate_dps = 0.0;
	};

	std::vector<sSensorOrientation_t> mSensorInfo;

	struct sImuData_t
	{
		uint64_t timestamp_ns = 0;

		double pitchRate_dps = 0.0;
		double rollRate_dps = 0.0;
		double yawRate_dps = 0.0;
	};

	sImuData_t	mImuData;
	uint32_t	mImuCount = 0;
};


inline bool cKinematics_Constant_SensorRotation::requiresTelemetryPass() { return !mHaveRotationalRates; }


