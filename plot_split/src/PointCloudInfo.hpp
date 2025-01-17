
#pragma once

#include "RappPointCloud.hpp"

#include <cbdf/PointCloudTypes.hpp>
#include <cbdf/PointCloud.hpp>

#include <string>
#include <optional>
#include <vector>
#include <deque>
#include <list>

// Forward Declarations

class cPointCloudInfo
{
public:
	struct sSensorAngles_t
	{
		double pitch_deg = 0.0;
		double roll_deg = 0.0;
		double yaw_deg = 0.0;
	};

	struct sSensorSpeeds_t
	{
		double vx_mps = 0.0;
		double vy_mps = 0.0;
		double vz_mps = 0.0;
	};

	struct sDistanceWindow_t
	{
		double min_distance_m = 0.0;
		double max_distance_m = 0.0;
	};

	struct sAzimuthWindow_t
	{
		double min_azimuth_deg = 0.0;
		double max_azimuth_deg = 0.0;
	};

	struct sAltitudeWindow_t
	{
		double min_altitude_deg = 0.0;
		double max_altitude_deg = 0.0;
	};

	struct sRappPoint_t
	{
		std::int32_t x_mm = 0;
		std::int32_t y_mm = 0;
		std::int32_t z_mm = 0;
	};


public:
	cPointCloudInfo() = default;
	virtual ~cPointCloudInfo() = default;

	const std::optional<pointcloud::eCOORDINATE_SYSTEM>& coordinateSystem() const;
	const std::optional<pointcloud::eKINEMATIC_MODEL>&	 kinematicModel() const;

	const std::optional<cPointCloudInfo::sSensorAngles_t>& sensorAngles() const;
	const std::optional<cPointCloudInfo::sSensorSpeeds_t>& sensorSpeeds() const;

	const std::optional<sDistanceWindow_t>& distanceWindow() const;
	const std::optional<sAzimuthWindow_t>&  azimuthWindow() const;
	const std::optional<sAltitudeWindow_t>& altitudeWindow() const;

	const std::optional<sRappPoint_t>& referencePoint() const;

	bool vegetationOnly() const;

	const std::optional<pointcloud::imu_data_t>& imuData() const;

	const std::deque<pointcloud::sSensorKinematicInfo_t>& getSensorKinematicData() const;

	std::size_t numPointClouds() const;
	const cRappPointCloud& getPointCloud(int index) const;

	const std::list<cRappPointCloud>& getPointClouds() const;

	void clear();

protected:

	void setCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system);
	void setKinematicModel(pointcloud::eKINEMATIC_MODEL model);
	void setSensorAngles(double pitch_deg, double roll_deg, double yaw_deg);
	void setKinematicSpeed(double vx_mps, double vy_mps, double vz_mps);

	void setDistanceWindow(double min_distance_m, double max_distance_m);
	void setAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg);
	void setAltitudeWindow(double min_altitude_deg, double max_altitude_deg);

	void setReferencePoint(std::int32_t x_mm, std::int32_t y_mm, std::int32_t z_mm);

	void setVegetationOnly(const bool vegetation_only);

	void setImuData(pointcloud::imu_data_t data);

	void clearPointCloudData();
	void addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud);
	void addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud);
	void addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud);
	void addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud);
	void addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud);
	void addPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud);
	void addPointCloudData(cPointCloud pointCloud);
	void addPointCloudData(cPointCloud_FrameId pointCloud);
	void addPointCloudData(cPointCloud_SensorInfo pointCloud);

	void clearSensorKinematics();
	void addSensorKinematicPoint(const pointcloud::sSensorKinematicInfo_t& point);

private:

	std::optional<pointcloud::eCOORDINATE_SYSTEM>	mCoordinateSystem;
	std::optional<pointcloud::eKINEMATIC_MODEL>		mKinematicModel;

	std::optional<sSensorAngles_t> mSensorAngles;

	std::optional<sSensorSpeeds_t> mSensorSpeeds;

	std::optional<sDistanceWindow_t> mDistanceWindow;
	std::optional<sAzimuthWindow_t>  mAzimuthWindow;
	std::optional<sAltitudeWindow_t> mAltitudeWindow;

	std::optional<sRappPoint_t> mReferencePoint;

	bool mVegetationOnly = false;

	std::optional<pointcloud::imu_data_t> mImuData;

	std::deque<pointcloud::sSensorKinematicInfo_t> mSensorKinematicData;

	std::list<cRappPointCloud> mPointClouds;

	friend class cPointCloudLoader;
};

