/**
 * @file
 * @brief Parser for reading OUSTER LiDAR data from a block based data file
 */
#pragma once

#include "BlockParser.hpp"
#include "OusterBlockId.hpp"

#include "ouster_defs.h"
#include "OusterLidarData.h"

// Forward Declarations
class cOusterLidarData;

namespace v1
{
	class cOusterParser : public cBlockParser
	{
	public:
		cOusterParser();
		~cOusterParser() = default;

		cBlockID& blockID() override;

		ouster::config_param_2_t		getConfigParam_2() const { return mConfigParams; }
		ouster::sensor_info_2_t			getSensorInfo_2() const { return mSensorInfo; }
		ouster::timestamp_2_t			getTimestamp_2() const { return mTimestamp; }
		ouster::sync_pulse_in_2_t		getSyncPulseIn_2() const { return mSyncPulseIn; }
		ouster::sync_pulse_out_2_t		getSyncPulseOut_2() const { return mSyncPulseOut; }
		ouster::multipurpose_io_2_t		getMultipurposeIo_2() const { return mMultipurposeIo; }
		ouster::nmea_2_t				getNmea_2() const { return mNmea; }
		ouster::time_info_2_t			getTimeInfo_2() const { return mTimeInfo; }
		ouster::beam_intrinsics_2_t		getBeamIntrinsics_2() const { return mBeamIntrinsics; }
		ouster::imu_intrinsics_2_t		getImuIntrinsics_2() const { return mImuIntrinsics; }
		ouster::lidar_intrinsics_2_t	getLidarIntrinsics_2() const { return mLidarIntrinsics; }
		ouster::lidar_data_format_2_t	getLidarDataFormat_2() const { return mLidarDataFormat; }
		ouster::imu_data_t				getImuData() const { return mImuData; }
		const cOusterLidarData& getLidarData() const { return mLidarData; }


		void processData(BLOCK_MAJOR_VERSION_t major_version,
			BLOCK_MINOR_VERSION_t minor_version,
			BLOCK_DATA_ID_t data_id,
			cDataBuffer& buffer) override;

		virtual void processConfigParam_2(cDataBuffer& buffer);
		virtual void processSensorInfo_2(cDataBuffer& buffer);
		virtual void processTimestamp_2(cDataBuffer& buffer);
		virtual void processSyncPulseIn_2(cDataBuffer& buffer);
		virtual void processSyncPulseOut_2(cDataBuffer& buffer);
		virtual void processMultipurposeIO_2(cDataBuffer& buffer);
		virtual void processNmea_2(cDataBuffer& buffer);
		virtual void processTimeInfo_2(cDataBuffer& buffer);
		virtual void processBeamIntrinsics_2(cDataBuffer& buffer);
		virtual void processImuIntrinsics_2(cDataBuffer& buffer);
		virtual void processLidarIntrinsics_2(cDataBuffer& buffer);
		virtual void processLidarDataFormat_2(cDataBuffer& buffer);
		virtual void processImuData(cDataBuffer& buffer);
		virtual void processLidarData(cDataBuffer& buffer);
		virtual void processLidarDataFrameTimestamp(cDataBuffer& buffer);

	protected:
		ouster::config_param_2_t		mConfigParams;
		ouster::sensor_info_2_t			mSensorInfo;
		ouster::timestamp_2_t			mTimestamp;
		ouster::sync_pulse_in_2_t		mSyncPulseIn;
		ouster::sync_pulse_out_2_t		mSyncPulseOut;
		ouster::multipurpose_io_2_t		mMultipurposeIo;
		ouster::nmea_2_t				mNmea;
		ouster::time_info_2_t			mTimeInfo;
		ouster::beam_intrinsics_2_t		mBeamIntrinsics;
		ouster::imu_intrinsics_2_t		mImuIntrinsics;
		ouster::lidar_intrinsics_2_t	mLidarIntrinsics;
		ouster::lidar_data_format_2_t	mLidarDataFormat;
		ouster::imu_data_t				mImuData;
		cOusterLidarData				mLidarData;

	private:
		cOusterLidarID    mBlockID;
	};
}
