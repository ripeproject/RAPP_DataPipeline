/**
 * @file
 * @brief Parser for reading OUSTER LiDAR data from a block based data file
 */
#pragma once

#include "OusterParser.hpp"

namespace v1
{
	class cOusterVerificationParser : public cOusterParser
	{
	public:
		virtual void onConfigParam(::ouster::config_param_2_t config_param) = 0;
		virtual void onSensorInfo(::ouster::sensor_info_2_t sensor_info) = 0;
		virtual void onTimestamp(::ouster::timestamp_2_t timestamp) = 0;
		virtual void onSyncPulseIn(::ouster::sync_pulse_in_2_t pulse_info) = 0;
		virtual void onSyncPulseOut(::ouster::sync_pulse_out_2_t pulse_info) = 0;
		virtual void onMultipurposeIo(::ouster::multipurpose_io_2_t io) = 0;
		virtual void onNmea(::ouster::nmea_2_t nmea) = 0;
		virtual void onTimeInfo(::ouster::time_info_2_t time_info) = 0;
		virtual void onBeamIntrinsics(::ouster::beam_intrinsics_2_t intrinsics) = 0;
		virtual void onImuIntrinsics(::ouster::imu_intrinsics_2_t intrinsics) = 0;
		virtual void onLidarIntrinsics(::ouster::lidar_intrinsics_2_t intrinsics) = 0;
		virtual void onLidarDataFormat(::ouster::lidar_data_format_2_t format) = 0;
		virtual void onImuData(::ouster::imu_data_t data) = 0;
		virtual void onLidarData(cOusterLidarData data) = 0;

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
}

