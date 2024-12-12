/**
 * @file
 * @brief Parser for reading pointcloud data from a block based data file
 */
#pragma once

#include "PointCloud.hpp"

#include <cbdf/BlockParser.hpp>

#include <memory>

 // Forward Declarations
class cPointCloudID;


class cPointCloudParser : public cBlockParser
{
public:
	cPointCloudParser();
	virtual ~cPointCloudParser();

	cBlockID& blockID() override;

	virtual void onBeginPointCloudBlock() = 0;
	virtual void onEndPointCloudBlock() = 0;

	virtual void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system) = 0;
	virtual void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) = 0;
	virtual void onDistanceWindow(double min_dist_m, double max_dist_m) = 0;
	virtual void onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg) = 0;
	virtual void onAltitudeWindow(double min_altitude_deg, double max_altitude_deg) = 0;

	virtual void onReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm) = 0;

	virtual void onVegetationOnly(const bool vegetation_only) = 0;

	virtual void onDimensions(double x_min_m, double x_max_m,
								double y_min_m, double y_max_m,
								double z_min_m, double z_max_m) = 0;

	virtual void onBeginSensorKinematics() = 0;
	virtual void onEndSensorKinematics() = 0;
	virtual void onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point) = 0;

	virtual void onPointCloudData(cPointCloud pointCloud) = 0;
	virtual void onPointCloudData(cPointCloud_FrameId pointCloud) = 0;
	virtual void onPointCloudData(cPointCloud_SensorInfo pointCloud) = 0;


	virtual void onBeginPointCloudList() = 0;
	virtual void onEndPointCloudList() = 0;

	virtual void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
									cReducedPointCloudByFrame pointCloud) = 0;
	virtual void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
									cReducedPointCloudByFrame_FrameId pointCloud) = 0;
	virtual void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
									cReducedPointCloudByFrame_SensorInfo pointCloud) = 0;

	virtual void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
									cSensorPointCloudByFrame pointCloud) = 0;
	virtual void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
									cSensorPointCloudByFrame_FrameId pointCloud) = 0;
	virtual void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
									cSensorPointCloudByFrame_SensorInfo pointCloud) = 0;

	// These data items have been marked deprecated in the serializer
	// virtual void onImuData(pointcloud::imu_data_t data) = 0;
	// virtual void onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) = 0;
	// virtual void onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) = 0;


protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	void processBeginPointCloudBlock(cDataBuffer& buffer);
	void processEndPointCloudBlock(cDataBuffer& buffer);

	void processCoordinateSystem(cDataBuffer& buffer);
	void processKinematicsModel(cDataBuffer& buffer);
	void processDistanceWindow(cDataBuffer& buffer);
	void processAzimuthWindow(cDataBuffer& buffer);
	void processAltitudeWindow(cDataBuffer& buffer);
	void processDimensions(cDataBuffer& buffer);
	void processReferencePoint(cDataBuffer& buffer);
	void processVegetationOnly(cDataBuffer& buffer);

	void processBeginSensorKinematics(cDataBuffer& buffer);
	void processEndSensorKinematics(cDataBuffer& buffer);
	void processSensorKinematicInfo(cDataBuffer& buffer);

	void processPointCloudData_1_0(cDataBuffer& buffer);
	void processPointCloudData_1_1(cDataBuffer& buffer);
	void processPointCloudData_FrameId(cDataBuffer& buffer);
	void processPointCloudData_SensorInfo_1_0(cDataBuffer& buffer);
	void processPointCloudData_SensorInfo_2_0(cDataBuffer& buffer);

	void processBeginPointCloudList(cDataBuffer& buffer);
	void processEndPointCloudList(cDataBuffer& buffer);

	void processReducedPointCloudByFrame(cDataBuffer& buffer);
	void processReducedPointCloudByFrame_FrameId(cDataBuffer& buffer);
	void processReducedPointCloudByFrame_SensorInfo_1_0(cDataBuffer& buffer);
	void processReducedPointCloudByFrame_SensorInfo_2_0(cDataBuffer& buffer);

	void processSensorPointCloudByFrame(cDataBuffer& buffer);
	void processSensorPointCloudByFrame_FrameId(cDataBuffer& buffer);
	void processSensorPointCloudByFrame_SensorInfo_1_0(cDataBuffer& buffer);
	void processSensorPointCloudByFrame_SensorInfo_2_0(cDataBuffer& buffer);

	// These data items have been marked deprecated in the serializer
	// void processImuData(cDataBuffer& buffer);
	// void processSensorAngles(cDataBuffer& buffer);
	// void processKinematicSpeed(cDataBuffer& buffer);


private:
	std::unique_ptr<cPointCloudID> mBlockID;
};

