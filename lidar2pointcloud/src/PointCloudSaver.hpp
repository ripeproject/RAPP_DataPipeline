
#pragma once

#include "PointCloudInfo.hpp"
#include "ProcessingInfoSerializer.hpp"
#include "PointCloudSerializer.hpp"

#include "RappPointCloud.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>

#include <string>
#include <filesystem>

void to_pointcloud(const cRappPointCloud& in, cPointCloud& out);

class cPointCloudSaver : public cOusterParser
{

public:
	cPointCloudSaver(int id, const cRappPointCloud& pointCloud);
	~cPointCloudSaver();

	void setInputFile(const std::string& in);
	void setOutputFile(const std::string& out);

	bool save();

protected:
	bool open();
	void close();

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
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	cRappPointCloud mPointCloud;

	const int mID = 0;
	std::uintmax_t mFileSize = 0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	cProcessingInfoSerializer	mProcessingInfoSerializer;
	cPointCloudSerializer		mPointCloudSerializer;
};
