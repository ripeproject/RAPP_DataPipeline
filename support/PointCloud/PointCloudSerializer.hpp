/**
 * @file
 * @brief Serializer for storing point cloud data into a block based data file.
 * 
 * @usage
 * 
 * Saving a single point cloud should contain the following calls:
 * 
 *		writeBeginPointCloudBlock
 * 
 * 			write pointcloud::eCOORDINATE_SYSTEM
 *
 *			writeDistanceWindow;
 *			writeAzimuthWindow;
 *			writeAltitudeWindow;
 *
 *			write pointcloud::eKINEMATIC_MODEL
 *
 *			writeSensorKinematics
 *				or
 *			writeBeginSensorKinematics
 *				writeSensorKinematicInfo
 *				writeSensorKinematicInfo
 *				...
 *			writeEndSensorKinematics
 *
 *			writeDimensions
 * 
 *			write cPointCloud  or  write cPointCloud_FrameId  or  write cPointCloud_SensorInfo
 * 
 *		writeEndPointCloudBlock
 *
 * 
 * Saving a list of point cloud should contain the following calls:
 * 
 *		writeBeginPointCloudList
 *			Point Cloud Block (as above)
 *			Point Cloud Block
 *				...
 *		writeEndPointCloudList
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

	void writeBeginPointCloudBlock();
	void writeEndPointCloudBlock();

	void write(const pointcloud::eCOORDINATE_SYSTEM in);
	void write(const pointcloud::eKINEMATIC_MODEL in);

	void writeDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m, double z_min_m, double z_max_m);

	void writeDistanceWindow(double min_dist_m, double max_dist_m);
	void writeAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg);
	void writeAltitudeWindow(double min_altitude_deg, double max_altitude_deg);

	void writeReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm);

	void writeVegetationOnly(const bool vegetation_only);

	void writeBeginSensorKinematics();
	void writeEndSensorKinematics();
	void writeSensorKinematicInfo(const pointcloud::sSensorKinematicInfo_t& point);

	template<class InputIt>
	void writeSensorKinematics(InputIt first, InputIt last);

	void write(const cPointCloud& in);
	void write(const cPointCloud_FrameId& in);
	void write(const cPointCloud_SensorInfo& in);

	// These methods are used to write out a list of point clouds.
	// Useful when trying to save each LiDAR FRAME -> Point Cloud
	void writeBeginPointCloudList();
	void writeEndPointCloudList();

	void write(const cReducedPointCloudByFrame& in);
	void write(const cReducedPointCloudByFrame_FrameId& in);
	void write(const cReducedPointCloudByFrame_SensorInfo& in);

	void write(const cSensorPointCloudByFrame& in);
	void write(const cSensorPointCloudByFrame_FrameId& in);
	void write(const cSensorPointCloudByFrame_SensorInfo& in);

	[[deprecated]]
	void write(const pointcloud::imu_data_t& in);

	[[deprecated]]
	void writeSensorAngles(double pitch_deg, double roll_deg, double yaw_deg);

	[[deprecated]]
	void writeKinematicSpeed(double vx_mps, double vy_mps, double vz_mps);

	[[deprecated]]
	void writeSensorRotationalSpeeds(double pitch_dps, double roll_dps, double yaw_dps);

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
