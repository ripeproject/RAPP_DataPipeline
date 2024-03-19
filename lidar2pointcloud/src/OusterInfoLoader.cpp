
#include "OusterInfoLoader.hpp"
#include "OusterInfo.hpp"

#include <cbdf/OusterInfoTypes.hpp>


cOusterInfoLoader::cOusterInfoLoader(std::weak_ptr<cOusterInfo> info)
{
	mInfo = info.lock();
}

cOusterInfoLoader::~cOusterInfoLoader()
{}

void cOusterInfoLoader::onConfigParam(ouster::config_param_2_t config_param)
{
	switch (config_param.lidar_mode)
	{
	case ouster::eLIDAR_MODE::MODE_512x10:
	case ouster::eLIDAR_MODE::MODE_1024x10:
	case ouster::eLIDAR_MODE::MODE_2048x10:
		mInfo->setUpdateRate_Hz(10.0);
		break;
	case ouster::eLIDAR_MODE::MODE_512x20:
	case ouster::eLIDAR_MODE::MODE_1024x20:
		mInfo->setUpdateRate_Hz(20.0);
		break;
	}
}

void cOusterInfoLoader::onSensorInfo(ouster::sensor_info_2_t sensor_info)
{}

void cOusterInfoLoader::onTimestamp(ouster::timestamp_2_t timestamp)
{}

void cOusterInfoLoader::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info)
{}

void cOusterInfoLoader::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info)
{}

void cOusterInfoLoader::onMultipurposeIo(ouster::multipurpose_io_2_t io)
{}

void cOusterInfoLoader::onNmea(ouster::nmea_2_t nmea)
{}

void cOusterInfoLoader::onTimeInfo(ouster::time_info_2_t time_info)
{}

void cOusterInfoLoader::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics)
{
	nOusterTypes::beam_intrinsics_2_t beam;

	beam.lidar_to_beam_origins_mm = intrinsics.lidar_to_beam_origins_mm;
	beam.altitude_angles_deg = intrinsics.altitude_angles_deg;
	beam.azimuth_angles_deg = intrinsics.azimuth_angles_deg;

	mInfo->setBeamIntrinsics(beam);
}

void cOusterInfoLoader::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics)
{
	nOusterTypes::imu_intrinsics_2_t imu;

	imu.imu_to_sensor_transform = intrinsics.imu_to_sensor_transform;

	mInfo->setImuIntrinsics(imu);
}

void cOusterInfoLoader::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics)
{
	nOusterTypes::lidar_intrinsics_2_t	lidar;

	lidar.lidar_to_sensor_transform = intrinsics.lidar_to_sensor_transform;

	mInfo->setLidarIntrinsics(lidar);
}

void cOusterInfoLoader::onLidarDataFormat(ouster::lidar_data_format_2_t format)
{
	nOusterTypes::lidar_data_format_2_t	lidar;

	lidar.columns_per_frame = format.columns_per_frame;
	lidar.columns_per_packet = format.columns_per_packet;
	lidar.column_window_max = format.column_window_max;
	lidar.column_window_min = format.column_window_min;
	lidar.pixels_per_column = format.pixels_per_column;
	lidar.pixel_shift_by_row = format.pixel_shift_by_row;

	mInfo->setLidarDataFormat(lidar);
}

void cOusterInfoLoader::onImuData(ouster::imu_data_t data)
{
	nOusterTypes::imu_data_t imu;

	imu.acceleration_Xaxis_g = data.acceleration_Xaxis_g;
	imu.acceleration_Yaxis_g = data.acceleration_Yaxis_g;
	imu.acceleration_Zaxis_g = data.acceleration_Zaxis_g;
	imu.accelerometer_read_time_ns = data.accelerometer_read_time_ns;

	imu.angular_velocity_Xaxis_deg_per_sec = data.angular_velocity_Xaxis_deg_per_sec;
	imu.angular_velocity_Yaxis_deg_per_sec = data.angular_velocity_Yaxis_deg_per_sec;
	imu.angular_velocity_Zaxis_deg_per_sec = data.angular_velocity_Zaxis_deg_per_sec;
	imu.gyroscope_read_time_ns = data.gyroscope_read_time_ns;

	imu.diagnostic_time_ns = data.diagnostic_time_ns;

	mInfo->addImuData(imu);
}

void cOusterInfoLoader::onLidarData(cOusterLidarData data)
{
	mInfo->addLidarData(data);
}


