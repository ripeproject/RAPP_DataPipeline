#pragma once

#include <cbdf/OusterParser.hpp>

#include <memory>

// Forward Declarations
class cOusterInfo;

class cOusterInfoLoader : public cOusterParser
{
public:
	explicit cOusterInfoLoader(std::weak_ptr<cOusterInfo> info);
	virtual ~cOusterInfoLoader();

protected:
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
	std::shared_ptr<cOusterInfo> mInfo;
};

