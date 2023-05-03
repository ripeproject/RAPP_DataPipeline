
#pragma once

#include "FileProcessor.hpp"

#include "bdf_v1/BlockDataFile.hpp"
#include "bdf_v1/OusterVerificationParser.hpp"
#include "bdf_v1/PvtVerificationParser.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterSerializer.hpp>
#include <cbdf/ExperimentSerializer.hpp>

#include <filesystem>
#include <string>
#include <fstream>


class cLidarData2CeresConverter : public cFileProcessor, 
									private v1::cOusterVerificationParser,
									private v1::cPvtVerificationParser
{
public:
	cLidarData2CeresConverter(std::filesystem::directory_entry in,
		std::filesystem::path out);

	~cLidarData2CeresConverter();

	bool complete() override;

	bool open();

	void run() override;

protected:
	void convert();

protected:
	void onConfigParam(::ouster::config_param_2_t config_param) override;
	void onSensorInfo(::ouster::sensor_info_2_t sensor_info) override;
	void onTimestamp(::ouster::timestamp_2_t timestamp) override;
	void onSyncPulseIn(::ouster::sync_pulse_in_2_t pulse_info) override;
	void onSyncPulseOut(::ouster::sync_pulse_out_2_t pulse_info) override;
	void onMultipurposeIo(::ouster::multipurpose_io_2_t io) override;
	void onNmea(::ouster::nmea_2_t nmea) override;
	void onTimeInfo(::ouster::time_info_2_t time_info) override;
	void onBeamIntrinsics(::ouster::beam_intrinsics_2_t intrinsics) override;
	void onImuIntrinsics(::ouster::imu_intrinsics_2_t intrinsics) override;
	void onLidarIntrinsics(::ouster::lidar_intrinsics_2_t intrinsics) override;
	void onLidarDataFormat(::ouster::lidar_data_format_2_t format) override;
	void onImuData(::ouster::imu_data_t data) override;
	void onLidarData(cOusterLidarData data) override;

protected:
	void onPositionUnits(v1::pvt::ePOSTION_UNITS  positionUnit) override;
	void onVelocityUnits(v1::pvt::eVELOCITY_UNITS velocityUnit) override;
	void onTimeUnits(v1::pvt::eTIME_UNITS timeUnit) override;
	void onPosition1D(double x) override;
	void onPosition2D(double x, double y) override;
	void onPosition3D(double x, double y, double z) override;
	void onVelocity1D(double Vx) override;
	void onVelocity2D(double Vx, double Vy) override;
	void onVelocity3D(double Vx, double Vy, double Vz) override;
	void onTimestamp(std::uint64_t timeStamp) override;

private:
	void processBlock(const v1::cBlockID& id);
	void processBlock(const v1::cBlockID& id, const std::byte* buf, std::size_t len);

private:
	std::filesystem::directory_entry mInputFile;
	std::filesystem::path mOutputFile;

	v1::cBlockDataFileReader mFileReader;
	cBlockDataFileWriter	 mFileWriter;

	cOusterSerializer		mOusterSerializer;
	cExperimentSerializer	mExperimentSerializer;

	double mTimeScaler = 0.0;
};


