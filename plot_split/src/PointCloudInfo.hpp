
#pragma once

#include "PointCloudTypes.hpp"
#include "RappPointCloud.hpp"

#include <string>
#include <optional>
#include <vector>

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

	struct sDollySpeeds_t
	{
		double vx_mps = 0.0;
		double vy_mps = 0.0;
		double vz_mp = 0.0;
	};

public:
	cPointCloudInfo() = default;
	virtual ~cPointCloudInfo() = default;

	const std::optional<pointcloud::eCOORDINATE_SYSTEM>& coordinateSystem() const;
	const std::optional<pointcloud::eKINEMATIC_MODEL>&	 kinematicModel() const;

	const std::optional<cPointCloudInfo::sSensorAngles_t>& sensorAngles() const;
	const std::optional<cPointCloudInfo::sDollySpeeds_t>&  dollySpeeds() const;

	const std::optional<pointcloud::imu_data_t>& imuData() const;

	const cRappPointCloud& pointCloud() const;

	void clear();

protected:

	void setCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system);
	void setKinematicModel(pointcloud::eKINEMATIC_MODEL model);
	void setSensorAngles(double pitch_deg, double roll_deg, double yaw_deg);
	void setKinematicSpeed(double vx_mps, double vy_mps, double vz_mps);

	void setImuData(pointcloud::imu_data_t data);

	void setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame pointCloud);
	void setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_FrameId pointCloud);
	void setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cReducedPointCloudByFrame_SensorInfo pointCloud);
	void setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame pointCloud);
	void setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_FrameId pointCloud);
	void setPointCloudData(uint16_t frameID, uint64_t timestamp_ns, cSensorPointCloudByFrame_SensorInfo pointCloud);
	void setPointCloudData(cPointCloud pointCloud);
	void setPointCloudData(cPointCloud_FrameId pointCloud);
	void setPointCloudData(cPointCloud_SensorInfo pointCloud);

private:

	std::optional<pointcloud::eCOORDINATE_SYSTEM>	mCoordinateSystem;
	std::optional<pointcloud::eKINEMATIC_MODEL>		mKinematicModel;

	std::optional<sSensorAngles_t> mSensorAngles;

	std::optional<sDollySpeeds_t> mDollySpeeds;

	std::optional<pointcloud::imu_data_t> mImuData;

	cRappPointCloud mPointCloud;

	friend class cPointCloudLoader;
};

