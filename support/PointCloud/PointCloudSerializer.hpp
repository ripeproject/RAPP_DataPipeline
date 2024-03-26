/**
 * @file
 * @brief Serializer for storing point cloud data into a block based data file
 */
#pragma once

#include <cbdf/BlockSerializer.hpp>

#include <vector>
#include <memory>


// Forward Declarations
namespace pointcloud
{
	enum class eCOORDINATE_SYSTEM : uint8_t;
	enum class eKINEMATIC_MODEL : uint8_t;
	struct imu_data_t;
	struct sSensorKinematicInfo_t;
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

	void writeDistanceWindow(double min_dist_m, double max_dist_m);
	void writeAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg);
	void writeAltitudeWindow(double min_altitude_deg, double max_altitude_deg);

	void writeBeginSensorKinematics();
	void writeEndSensorKinematics();
	void writeSensorKinematicInfo(const pointcloud::sSensorKinematicInfo_t& point);

	template<class InputIt>
	void writeSensorKinematics(InputIt first, InputIt last);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cPointCloudID> mBlockID;
};



///////////////////////////////////////////////////////////////////////////////
// Implementation Details
///////////////////////////////////////////////////////////////////////////////


template<class InputIt>
inline void cPointCloudSerializer::writeSensorKinematics(InputIt first, InputIt last)
{
	if (first == last)
		return;

	writeBeginSensorKinematics();
	for (; first != last; ++first)
	{
		writeSensorKinematicInfo(*first);
	}
	writeEndSensorKinematics();
}
