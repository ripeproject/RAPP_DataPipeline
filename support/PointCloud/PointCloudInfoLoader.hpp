#pragma once

#include "PointCloudTypes.hpp"
#include "PointCloudInfo.hpp"
#include "PointCloudParser.hpp"

#include <memory>


class cPointCloudInfoLoader : public cPointCloudParser
{
public:
	explicit cPointCloudInfoLoader(std::weak_ptr<cPointCloudInfo> info);
	virtual ~cPointCloudInfoLoader();

protected:
	void onBeginPointCloudBlock() override;
	void onEndPointCloudBlock() override;

	void onCoordinateSystem(pointcloud::eCOORDINATE_SYSTEM coordinate_system) override;
	void onKinematicModel(pointcloud::eKINEMATIC_MODEL model) override;
	void onDistanceWindow(double min_dist_m, double max_dist_m) override;
	void onAzimuthWindow(double min_azimuth_deg, double max_azimuth_deg) override;
	void onAltitudeWindow(double min_altitude_deg, double max_altitude_deg) override;

	void onDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m,
		double z_min_m, double z_max_m) override;

	void onBeginSensorKinematics() override;
	void onEndSensorKinematics() override;
	void onSensorKinematicInfo(pointcloud::sSensorKinematicInfo_t point) override;

	void onPointCloudData(cPointCloud pointCloud) override;
	void onPointCloudData(cPointCloud_FrameId pointCloud) override;
	void onPointCloudData(cPointCloud_SensorInfo pointCloud) override;

	void onBeginPointCloudList() override;
	void onEndPointCloudList() override;

	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
		cReducedPointCloudByFrame pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
		cReducedPointCloudByFrame_FrameId pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
		cReducedPointCloudByFrame_SensorInfo pointCloud) override;

	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
		cSensorPointCloudByFrame pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
		cSensorPointCloudByFrame_FrameId pointCloud) override;
	void onPointCloudData(uint16_t frameID, uint64_t timestamp_ns,
		cSensorPointCloudByFrame_SensorInfo pointCloud) override;

private:
	std::shared_ptr<cPointCloudInfo> mInfo;

	sPointCloudBlock mBlock;

	bool mHasListTag = false;
	bool mHasBlockTag = false;
};

