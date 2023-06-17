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

//class cReducedPointCloudByFrame;
//class cSensorPointCloudByFrame;
//class cPointCloud;


class cPointCloudParser : public cBlockParser
{
public:
	cPointCloudParser();
	virtual ~cPointCloudParser();

	cBlockID& blockID() override;

	virtual void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) = 0;
	virtual void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) = 0;
	virtual void onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) = 0;
	virtual void onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) = 0;

	virtual void onDimensions(double x_min_m, double x_max_m,
								double y_min_m, double y_max_m,
								double z_min_m, double z_max_m) = 0;
	virtual void onImuData(pointcloud::imu_data_t data) = 0;
	virtual void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns,
											cReducedPointCloudByFrame pointCloud) = 0;
	virtual void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, 
											cSensorPointCloudByFrame pointCloud) = 0;
	virtual void onPointCloudData(cPointCloud pointCloud) = 0;


protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	void processCoordinateSystem(cDataBuffer& buffer);
	void processKinematicsModel(cDataBuffer& buffer);
	void processSensorAngles(cDataBuffer& buffer);
	void processKinematicSpeed(cDataBuffer& buffer);
	void processDimensions(cDataBuffer& buffer);
	void processImuData(cDataBuffer& buffer);
	void processReducedPointCloudByFrame(cDataBuffer& buffer);
	void processSensorPointCloudByFrame(cDataBuffer& buffer);
	void processPointCloudData_1_0(cDataBuffer& buffer);
	void processPointCloudData_1_1(cDataBuffer& buffer);

private:
	std::unique_ptr<cPointCloudID> mBlockID;
};

