#pragma once

#include "PointCloudInfo.hpp"

#include "PointCloudParser.hpp"

#include <list>
#include <memory>

// Forward Declarations


class cPointCloudLoader : public cPointCloudParser
{
public:
	explicit cPointCloudLoader(std::list<cPointCloudInfo>& infoList);
	virtual ~cPointCloudLoader();

protected:
	void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system) override;
	void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) override;
	void onSensorAngles(double pitch_deg, double roll_deg, double yaw_deg) override;
	void onKinematicSpeed(double vx_mps, double vy_mps, double vz_mps) override;

	void onDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m, double z_min_m, double z_max_m) override;

	void onImuData(pointcloud::imu_data_t data) override;

	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud) override;

	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud) override;

	void onPointCloudData(cPointCloud pointCloud) override;
	void onPointCloudData(cPointCloud_FrameId pointCloud) override;
	void onPointCloudData(cPointCloud_SensorInfo pointCloud) override;

private:
	cPointCloudInfo mInfo;

	std::list<cPointCloudInfo>& mInfoList;
};

