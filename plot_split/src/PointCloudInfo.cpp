
#include "PointCloudInfo.hpp"


void cPointCloudInfo::clear()
{
	mCoordinateSystem.reset();
	mKinematicModel.reset();

	mSensorAngles.reset();
	mSensorSpeeds.reset();

	mDistanceWindow.reset();
	mAzimuthWindow.reset();
	mAltitudeWindow.reset();

	mReferencePoint.reset();

	mVegetationOnly = false;

	mImuData.reset();

	mSensorKinematicData.clear();
	mPointClouds.clear();
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

const std::optional<cPointCloudInfo::sSensorSpeeds_t>& cPointCloudInfo::sensorSpeeds() const
{
	return mSensorSpeeds;
}

const std::optional<cPointCloudInfo::sDistanceWindow_t>& cPointCloudInfo::distanceWindow() const
{
	return mDistanceWindow;
}

const std::optional<cPointCloudInfo::sAzimuthWindow_t>& cPointCloudInfo::azimuthWindow() const
{
	return mAzimuthWindow;
}

const std::optional<cPointCloudInfo::sAltitudeWindow_t>& cPointCloudInfo::altitudeWindow() const
{
	return mAltitudeWindow;
}

const std::optional<cPointCloudInfo::sRappPoint_t>& cPointCloudInfo::referencePoint() const
{
	return mReferencePoint;
}

bool cPointCloudInfo::vegetationOnly() const
{
	return mVegetationOnly;
}

const std::optional<pointcloud::imu_data_t>& cPointCloudInfo::imuData() const
{
	return mImuData;
}

const std::deque<pointcloud::sSensorKinematicInfo_t>& cPointCloudInfo::getSensorKinematicData() const
{
	return mSensorKinematicData;
}

std::size_t cPointCloudInfo::numPointClouds() const
{
	return mPointClouds.size();
}

const cRappPointCloud& cPointCloudInfo::getPointCloud(int index) const
{
	auto it = mPointClouds.begin();
	std::advance(it, index);
	return *it;
}

const std::list<cRappPointCloud>& cPointCloudInfo::getPointClouds() const
{
	return mPointClouds;
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
	sSensorSpeeds_t speeds = { vx_mps, vy_mps, vz_mps };
	mSensorSpeeds = speeds;
}

void cPointCloudInfo::setDistanceWindow(double min_distance_m, double max_distance_m)
{
	sDistanceWindow_t window = { min_distance_m, max_distance_m };
	mDistanceWindow = window;
}

void cPointCloudInfo::setAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg)
{
	sAzimuthWindow_t window = { min_azimuth_deg, max_azimuth_deg };
	mAzimuthWindow = window;
}

void cPointCloudInfo::setAltitudeWindow(double min_altitude_deg, double max_altitude_deg)
{
	sAltitudeWindow_t window = { min_altitude_deg, max_altitude_deg };
	mAltitudeWindow = window;
}

void cPointCloudInfo::setReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm)
{
	sRappPoint_t ref_point = { x_mm, y_mm, z_mm };
	mReferencePoint = ref_point;
}

void cPointCloudInfo::setVegetationOnly(const bool vegetation_only)
{
	mVegetationOnly = vegetation_only;
}

void cPointCloudInfo::setImuData(pointcloud::imu_data_t data)
{
	mImuData = data;
}

void cPointCloudInfo::clearPointCloudData()
{
	mPointClouds.clear();
}

void cPointCloudInfo::addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(cPointCloud pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(cPointCloud_FrameId pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::addPointCloudData(cPointCloud_SensorInfo pointCloud)
{
	mPointClouds.push_back(cRappPointCloud(pointCloud.data()));
}

void cPointCloudInfo::clearSensorKinematics()
{
	mSensorKinematicData.clear();
}

void cPointCloudInfo::addSensorKinematicPoint(const pointcloud::sSensorKinematicInfo_t& point)
{
	mSensorKinematicData.push_back(point);
}
