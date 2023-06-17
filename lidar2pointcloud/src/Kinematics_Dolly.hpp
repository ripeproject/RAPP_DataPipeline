
#pragma once

#include "Kinematics.hpp"

#include <cbdf/ExperimentParser.hpp>
#include <cbdf/ExperimentSerializer.hpp>
#include <cbdf/SpidercamParser.hpp>
#include <cbdf/OusterParser.hpp>

#include <vector>


class cKinematics_Dolly : public cKinematics, public cSpidercamParser, 
						public cExperimentParser, public cOusterParser
{
public:
	cKinematics_Dolly();

	bool usingImuData() const;
	void useImuData(bool useImuData);

	bool averagingImuData() const;
	void averageImuData(bool average);

	double sensorPitchOffset_deg() const;
	void setSensorPitchOffset_deg(double offset_deg);

	double sensorRollOffset_deg() const;
	void setSensorRollOffset_deg(double offset_deg);

	void writeHeader(cPointCloudSerializer& serializer) override;

	/*
	 * Return true, we need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;
	void telemetryPassComplete() override;

	/*
	 * Attach/detach any parsers to the data file for the kinematic phase.
	 */
	void attachKinematicParsers(cBlockDataFileReader& file) override;
	void detachKinematicParsers(cBlockDataFileReader& file) override;

	/*
	 * Attach/detach any parsers/serializers to the data file for the transform phase.
	 */
	void attachTransformParsers(cBlockDataFileReader& file) override;
	void detachTransformParsers(cBlockDataFileReader& file) override;

	void attachTransformSerializers(cBlockDataFileWriter& file) override;
	void detachTransformSerializers(cBlockDataFileWriter& file) override;

	/*
	 * Transform the point cloud coordinated based on kinematic model
	 */
	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override;

protected:
	/** Spidercam Parser **/
	void onPosition(spidercam::sPosition_1_t position) override;

	/** Experiment Parser **/
	void onBeginHeader() override;
	void onEndOfHeader() override;

	void onBeginFooter() override;
	void onEndOfFooter() override;

	void onTitle(const std::string& title) override;
	void onPrincipalInvestigator(const std::string& investigator) override;
	void onResearcher(const std::string& researcher) override;
	void onSpecies(const std::string& species) override;
	void onCultivar(const std::string& cultivar) override;
	void onPermitInfo(const std::string& permit) override;
	void onExperimentDoc(const std::string& doc) override;

	void onBeginTreatmentList() override;
	void onEndOfTreatmentList() override;
	void onTreatment(const std::string& treatment) override;

	void onConstructName(const std::string& name) override;
	void onEventNumber(const std::string& event) override;
	void onFieldDesign(const std::string& design) override;
	void onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;
	void onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;

	void onBeginCommentList() override;
	void onEndOfCommentList() override;
	void onComment(const std::string& comment) override;

	void onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) override;
	void onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) override;

	void onDayOfYear(std::uint16_t day_of_year) override;

	void onBeginSensorList() override;
	void onEndOfSensorList() override;
	void onSensorBlockInfo(uint16_t class_id, const std::string& name) override;

	void onStartTime(sExperimentTime_t time) override;
	void onEndTime(sExperimentTime_t time) override;

	void onStartRecordingTimestamp(uint64_t timestamp_ns) override;
	void onEndRecordingTimestamp(uint64_t timestamp_ns) override;
	void onHeartbeatTimestamp(uint64_t timestamp_ns) override;

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

	bool mStartPath = false;
	bool mDataActive = false;
	uint64_t mStartTimestamp = 0;

	double mInitSouthPos_m  = 0.0;
	double mInitEastPos_m   = 0.0;
	double mInitHeightPos_m = 0.0;

	double mSouthPos_m  = 0.0;
	double mEastPos_m   = 0.0;
	double mHeightPos_m = 0.0;

	double mSouthOffset_m = 0.0;
	double mEastOffset_m = 0.0;
	double mHeightOffset_m = 0.0;

	/*
	 * The sensor uses a south/east/up coordinate system
	 * x axis is positive heading south
	 * y axis is positive heading east
	 * z axis is positive going up
	 */
	struct sSensorInfo_t
	{
		double timestamp_us = 0.0;
		double x_m = 0.0;
		double y_m = 0.0;
		double z_m = 0.0;
		double vx_mps = 0.0;
		double vy_mps = 0.0;
		double vz_mps = 0.0;

		double pitch_deg = 0.0;
		double roll_deg  = 0.0;
		double pitchRate_dps = 0.0;
		double rollRate_dps  = 0.0;
	};

	std::vector<sSensorInfo_t> mDollyInfo;
	std::vector<sSensorInfo_t>::size_type mDollyInfoIndex = 0;
	std::vector<sSensorInfo_t>::size_type mDollyPassIndex = 0;
	std::vector<sSensorInfo_t>::size_type mDollyInfoMax = 0;

	bool mUseImuData = false;

	ouster::imu_intrinsics_2_t			mImuIntrinsics;
	ouster::cTransformMatrix<double>	mImuTransform;
	ouster::cRotationMatrix<double>		mImuToSensor;

	double mInitPitch_deg = 0.0;
	double mInitRoll_deg = 0.0;

	double mOffsetPitch_deg = 0.0;
	double mOffsetRoll_deg = -5.0;

	bool mUseAverageOrientation = false;
	double mPitchSum = 0.0;
	double mRollSum = 0.0;
	uint32_t mImuCount = 0;

	std::size_t mPassNumber = 0;
	std::vector<std::size_t> mPassIndex;

	cExperimentSerializer mSerializer;
};


inline bool cKinematics_Dolly::requiresTelemetryPass() { return true; }

