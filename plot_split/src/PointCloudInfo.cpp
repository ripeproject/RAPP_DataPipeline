
#include "PointCloudInfo.hpp"


void cPointCloudInfo::clear()
{
	mCoordinateSystem.reset();
	mKinematicModel.reset();

	mSensorAngles.reset();

	mDollySpeeds.reset();

	mImuData.reset();

	mPointCloud.clear();
}

/**********************************************************
* Getters
***********************************************************/

const std::optional<pointcloud::eCOORDINATE_SYSTEM>& cPointCloudInfo::coordinateSystem() const
{
	return mCoordinateSystem;
}

const std::optional<pointcloud::eKINEMATIC_MODEL>& cPointCloudInfo::kinematicModel() const
{
	return mKinematicModel;
}

const std::optional<cPointCloudInfo::sSensorAngles_t>& cPointCloudInfo::sensorAngles() const
{
	return mSensorAngles;
}

const std::optional<cPointCloudInfo::sDollySpeeds_t>& cPointCloudInfo::dollySpeeds() const
{
	return mDollySpeeds;
}

const std::optional<pointcloud::imu_data_t>& cPointCloudInfo::imuData() const
{
	return mImuData;
}

const cRappPointCloud& cPointCloudInfo::pointCloud() const
{
	return mPointCloud;
}

/**********************************************************
* Setters
***********************************************************/

void cPointCloudInfo::setCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system)
{
	mCoordinateSystem = coordinate_system;
}

void cPointCloudInfo::setKinematicModel(pointcloud::eKINEMATIC_MODEL model)
{
	mKinematicModel = model;
}

void cPointCloudInfo::setSensorAngles(double pitch_deg, double roll_deg, double yaw_deg)
{
	sSensorAngles_t angles = { pitch_deg, roll_deg, yaw_deg };
	mSensorAngles = angles;
}

void cPointCloudInfo::setKinematicSpeed(double vx_mps, double vy_mps, double vz_mps)
{
	sDollySpeeds_t speeds = { vx_mps, vy_mps, vz_mps };
	mDollySpeeds = speeds;
}

void cPointCloudInfo::setImuData(pointcloud::imu_data_t data)
{
	mImuData = data;
}

void cPointCloudInfo::setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(cPointCloud pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(cPointCloud_FrameId pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}

void cPointCloudInfo::setPointCloudData(cPointCloud_SensorInfo pointCloud)
{
	mPointCloud = cRappPointCloud(pointCloud);
}



