/**
 * @file
 * @brief Serializer for storing point cloud data into a block based data file
 */
#pragma once

#include <cbdf/BlockSerializer.hpp>

#include <memory>


// Forward Declarations
namespace pointcloud
{
	enum class eCOORDINATE_SYSTEM : uint8_t;
	enum class eKINEMATIC_MODEL : uint8_t;
	struct imu_data_t;
}

class cPointCloudID;

class cReducedPointCloudByFrame;
class cReducedPointCloudByFrame_FrameId;
class cReducedPointCloudByFrame_SensorInfo;

class cSensorPointCloudByFrame;
class cSensorPointCloudByFrame_FrameId;
class cSensorPointCloudByFrame_SensorInfo;

class cPointCloud;
class cPointCloud_FrameId;
class cPointCloud_SensorInfo;


class cPointCloudSerializer : public cBlockSerializer
{
public:
	cPointCloudSerializer();
	explicit cPointCloudSerializer(std::size_t n, cBlockDataFileWriter* pDataFile = nullptr);
	virtual ~cPointCloudSerializer();

	void write(const pointcloud::eCOORDINATE_SYSTEM in);
	void write(const pointcloud::eKINEMATIC_MODEL in);
	void write(const pointcloud::imu_data_t& in);

	void write(const cPointCloud& in);
	void write(const cPointCloud_FrameId& in);
	void write(const cPointCloud_SensorInfo& in);

	void write(const cReducedPointCloudByFrame& in);
	void write(const cReducedPointCloudByFrame_FrameId& in);
	void write(const cReducedPointCloudByFrame_SensorInfo& in);

	void write(const cSensorPointCloudByFrame& in);
	void write(const cSensorPointCloudByFrame_FrameId& in);
	void write(const cSensorPointCloudByFrame_SensorInfo& in);

	void writeSensorAngles(double pitch_deg, double roll_deg, double yaw_deg);
	void writeKinematicSpeed(double vx_mps, double vy_mps, double vz_mps);
	void writeSensorRotationalSpeeds(double pitch_dps, double roll_dps, double yaw_dps);
	void writeDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m, double z_min_m, double z_max_m);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cPointCloudID> mBlockID;
};

