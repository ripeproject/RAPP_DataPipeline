/**
 * @file
 * @brief Parser for reading OUSTER LiDAR data from a block based data file
 */
#pragma once

#include <cbdf/OusterParser.hpp>


class cOusterRepairParser : public cOusterParser
{
public:
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

protected:
	void processConfigParam_2(cDataBuffer& buffer) override;
	void processSensorInfo_2(cDataBuffer& buffer) override;
	void processTimestamp_2(cDataBuffer& buffer) override;
	void processSyncPulseIn_2(cDataBuffer& buffer) override;
	void processSyncPulseOut_2(cDataBuffer& buffer) override;
	void processMultipurposeIO_2(cDataBuffer& buffer) override;
	void processNmea_2(cDataBuffer& buffer) override;
	void processTimeInfo_2(cDataBuffer& buffer) override;
	void processBeamIntrinsics_2(cDataBuffer& buffer) override;
	void processImuIntrinsics_2(cDataBuffer& buffer) override;
	void processLidarIntrinsics_2(cDataBuffer& buffer) override;
	void processLidarDataFormat_2(cDataBuffer& buffer) override;
	void processImuData(cDataBuffer& buffer) override;
	void processLidarData(cDataBuffer& buffer) override;
	void processLidarDataFrameTimestamp(cDataBuffer& buffer) override;
};

