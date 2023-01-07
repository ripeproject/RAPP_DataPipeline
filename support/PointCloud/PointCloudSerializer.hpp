/**
 * @file
 * @brief Serializer for storing OUSTER LiDAR data into a block based data file
 */
#pragma once

#include "BlockSerializer.hpp"

#include <memory>


// Forward Declarations
namespace ouster
{
	struct config_param_2_t;
	struct sensor_info_2_t;
	struct timestamp_2_t;
	struct sync_pulse_in_2_t;
	struct sync_pulse_out_2_t;
	struct multipurpose_io_2_t;
	struct nmea_2_t;
	struct time_info_2_t;
	struct beam_intrinsics_2_t;
	struct imu_intrinsics_2_t;
	struct lidar_intrinsics_2_t;
	struct lidar_data_format_2_t;
	struct imu_data_t;
}
class cOusterLidarData;
class cOusterLidarID;


class cOusterSerializer : public cBlockSerializer
{
public:
	cOusterSerializer();
	explicit cOusterSerializer(std::size_t n, cBlockDataFileWriter* pDataFile);
	virtual ~cOusterSerializer();

	void write(const ouster::config_param_2_t& in);
	void write(const ouster::sensor_info_2_t& in);
	void write(const ouster::timestamp_2_t& in);
	void write(const ouster::sync_pulse_in_2_t& in);
	void write(const ouster::sync_pulse_out_2_t& in);
	void write(const ouster::multipurpose_io_2_t& in);
	void write(const ouster::nmea_2_t& in);
	void write(const ouster::time_info_2_t& in);
	void write(const ouster::beam_intrinsics_2_t& in);
	void write(const ouster::imu_intrinsics_2_t& in);
	void write(const ouster::lidar_intrinsics_2_t& in);
	void write(const ouster::lidar_data_format_2_t& in);
	void write(const ouster::imu_data_t& in);
	void write(uint16_t frameID, const cOusterLidarData& data);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cOusterLidarID> mBlockID;
};

