/**
 * @file
 * @brief Parser for reading OUSTER LiDAR data from a block based data file
 */
#pragma once

#include "PointCloudTypes.hpp"

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

	virtual void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM config_param) = 0;
	virtual void onImuData(pointcloud::imu_data_t data) = 0;
	virtual void onReducedPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns,
									std::vector<pointcloud::sCloudPoint_t> pointCloud) = 0;
	virtual void onSensorPointCloudByFrame(uint16_t frameID, uint64_t timestamp_ns, 
									std::vector<pointcloud::sCloudPoint_t> pointCloud) = 0;
	virtual void onPointCloudData(std::vector<pointcloud::sCloudPoint_t> pointCloud) = 0;

protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	void processCoordinateSystem(cDataBuffer& buffer);
	void processImuData(cDataBuffer& buffer);
	void processReducedPointCloudByFrame(cDataBuffer& buffer);
	void processSensorPointCloudByFrame(cDataBuffer& buffer);
	void processPointCloudData(cDataBuffer& buffer);

private:
	std::unique_ptr<cPointCloudID> mBlockID;
};

