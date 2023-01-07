/**
 * @file
 * @brief Parser for reading OUSTER LiDAR data from a block based data file
 */
#pragma once

#include "BlockParser.hpp"

#include <ouster/ouster_defs.h>
#include <ouster/OusterLidarData.h>

#include <memory>

 // Forward Declarations
class cOusterLidarID;


class cOusterParser : public cBlockParser
{
public:
	cOusterParser();
	virtual ~cOusterParser();

	cBlockID& blockID() override;

	virtual void onConfigParam(ouster::config_param_2_t config_param) = 0;
	virtual void onSensorInfo(ouster::sensor_info_2_t sensor_info) = 0;
	virtual void onTimestamp(ouster::timestamp_2_t timestamp) = 0;
	virtual void onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) = 0;
	virtual void onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) = 0;
	virtual void onMultipurposeIo(ouster::multipurpose_io_2_t io) = 0;
	virtual void onNmea(ouster::nmea_2_t nmea) = 0;
	virtual void onTimeInfo(ouster::time_info_2_t time_info) = 0;
	virtual void onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) = 0;
	virtual void onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics) = 0;
	virtual void onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) = 0;
	virtual void onLidarDataFormat(ouster::lidar_data_format_2_t format) = 0;
	virtual void onImuData(ouster::imu_data_t data) = 0;
	virtual void onLidarData(cOusterLidarData data) = 0;

protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	void processConfigParam_2(cDataBuffer& buffer);
	void processSensorInfo_2(cDataBuffer& buffer);
	void processTimestamp_2(cDataBuffer& buffer);
	void processSyncPulseIn_2(cDataBuffer& buffer);
	void processSyncPulseOut_2(cDataBuffer& buffer);
	void processMultipurposeIO_2(cDataBuffer& buffer);
	void processNmea_2(cDataBuffer& buffer);
	void processTimeInfo_2(cDataBuffer& buffer);
	void processBeamIntrinsics_2(cDataBuffer& buffer);
	void processImuIntrinsics_2(cDataBuffer& buffer);
	void processLidarIntrinsics_2(cDataBuffer& buffer);
	void processLidarDataFormat_2(cDataBuffer& buffer);
	void processImuData(cDataBuffer& buffer);
	void processLidarData(cDataBuffer& buffer);
	void processLidarDataFrameTimestamp(cDataBuffer& buffer);

protected:
	cOusterLidarData	mLidarData;

private:
	std::unique_ptr<cOusterLidarID> mBlockID;
};

