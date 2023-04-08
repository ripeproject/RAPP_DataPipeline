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

