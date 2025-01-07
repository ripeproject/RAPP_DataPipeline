
#pragma once

#include "KinematicDataTypes.hpp"

#include "PointCloudInfo.hpp"

#include "RappPointCloud.hpp"

#include <cbdf/ProcessingInfoSerializer.hpp>
#include <cbdf/PointCloudSerializer.hpp>

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/OusterParser.hpp>

#include <string>
#include <filesystem>

// Forward Declarations
namespace pointcloud
{
	struct sSensorKinematicInfo_t;
}

void to_pointcloud(const cRappPointCloud& in, cPointCloud& out);
void to_pointcloud(const cRappPointCloud& in, cPointCloud_FrameId& out);
void to_pointcloud(const cRappPointCloud& in, cPointCloud_SensorInfo& out);
pointcloud::sSensorKinematicInfo_t to_sensor_kinematics(const kdt::sDollyInfo_t& in);


class cPointCloudSaver : public cOusterParser
{

public:
	cPointCloudSaver(int id, const cRappPointCloud& pointCloud);
	~cPointCloudSaver();

	void setInputFile(const std::string& in);
	void setOutputFile(const std::string& out);

	void setKinematicModel(pointcloud::eKINEMATIC_MODEL model, const std::vector<kdt::sDollyInfo_t>& dollyPath);
	void setRangeWindow_m(double min_dist_m, double max_dist_m);
	void setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg);
	void setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg);

	bool save(bool isFlattened);

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
	pointcloud::eKINEMATIC_MODEL mKinematicModel = pointcloud::eKINEMATIC_MODEL::UNKNOWN;
	std::vector<pointcloud::sSensorKinematicInfo_t> mComputedDollyPath;

	cRappPointCloud mPointCloud;

	const int mID = 0;
	std::uintmax_t mFileSize = 0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	double mMinDistance_m = 0;
	double mMaxDistance_m = 0;

	double mMinAzimuth_deg = 0;
	double mMaxAzimuth_deg = 0;

	double mMinAltitude_deg = 0;
	double mMaxAltitude_deg = 0;

	cProcessingInfoSerializer	mProcessingInfoSerializer;
	cPointCloudSerializer		mPointCloudSerializer;
};
