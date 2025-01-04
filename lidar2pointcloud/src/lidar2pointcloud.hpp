
#pragma once


#include "FieldScanDataModel.hpp"

#include <cbdf/PointCloudTypes.hpp>
#include <cbdf/PointCloud.hpp>

#include <filesystem>
#include <string>
#include <numbers>
#include <optional>
#include <fstream>
#include <memory>


struct sPoint_t
{
    double x;
    double y;
    double z;

    sPoint_t() : x(0), y(0), z(0) {}
    sPoint_t(double i, double j, double k) : x(i), y(j), z(k) {}
};


typedef std::vector<rfm::sDollyOrientationInterpPoint_t> InterpTable_t;

class cLidar2PointCloud : public cFieldScanDataModel
{
public:
	explicit cLidar2PointCloud(int id);
	~cLidar2PointCloud();

	/**
	 * Set the valid range in meters
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_dist_m	: The minimum distance to be considered valid
	 * @param	max_dist_m	: The maximum distance to be considered valid
	 */
	void setValidRange_m(double min_dist_m, double max_dist_m);

	/**
	 * Set the azimuth bounds to consider valid data
	 *
	 * The azimuth angle (zero) starts at the connector and increases in the
	 * counter-clock-wise direction.
	 * 		min_azimuth_deg < azimuth_deg < max_azimuth_deg
	 *
	 * @param	min_azimuth_deg	: The minimum azimuth to be considered valid
	 * @param	max_azimuth_deg	: The maximum azimuth to be considered valid
	 */
	void setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg);

	/**
	 * Set the altitude bounds to consider valid data
	 *
	 * The lidar return range is considered valid if:
	 * 		min_dist_m < range_m < max_dist_m
	 *
	 * @param	min_altitude_deg	: The minimum altitude to be considered valid
	 * @param	max_altitude_deg	: The maximum altitude to be considered valid
	 */
	void setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg);

	void setInitialPosition_m(double x_m, double y_m, double z_m);
	void setFinalPosition_m(double x_m, double y_m, double z_m);

	void setDollySpeed(double Vx_mmps, double Vy_mmps, double Vz_mmps);

	void setSensorMountOrientation(double yaw_deg, double pitch_deg, double roll_deg);

	void setOrientationOffset_deg(double yaw_deg, double pitch_deg, double roll_deg);

	void setInitialOffset_deg(double yaw_deg, double pitch_deg, double roll_deg);
	void setFinalOffset_deg(double yaw_deg, double pitch_deg, double roll_deg);

	void setInterpolationTable(const InterpTable_t& table);

	void setTranslateToGroundModel(eTranslateToGroundModel model);
	void setTranslateDistance_m(double distance_m);
	void setTranslateThreshold_pct(double threshold_pct);
	void setTranslateInterpTable(const std::vector<rfm::sPointCloudTranslationInterpPoint_t>& table);

	void setRotationToGroundModel(eRotateToGroundModel model);
	void setRotationAngles_deg(double pitch_deg, double roll_deg);
	void setRotationThreshold_pct(double threshold_pct);
	void setRotateInterpTable(const std::vector<rfm::sPointCloudRotationInterpPoint_t>& table);

	pointcloud::eKINEMATIC_MODEL getKinematicModel() const;

	const std::vector<rfm::sDollyInfo_t>& getComputedDollyPath() const;

	const cRappPointCloud& getPointCloud() const;

public:
	bool computeDollyMovement();
	bool computeDollyOrientation();
	bool computePointCloud();

private:
	void computeDollyMovement_ConstantSpeed();
	void computeDollyMovement_SpiderCam();
	void computeDollyMovement_GpsSpeeds();

	void computeDollyOrientation_Constant();
	void computeDollyOrientation_ConstantSpeed();
	void computeDollyOrientation_InterpTable();
	void computeDollyOrientation_IMU();

private:
	const int mID;

	pointcloud::eKINEMATIC_MODEL mKinematicModel = pointcloud::eKINEMATIC_MODEL::UNKNOWN;

	double mMinDistance_m = 0.001;
	double mMaxDistance_m = 1000.0;

	double mMinAzimuth_deg = 0.0;
	double mMaxAzimuth_deg = 360;

	double mMinAltitude_deg = -25.0;
	double mMaxAltitude_deg = 25.0;

	int32_t mStartX_mm = 0;
	int32_t mStartY_mm = 0;
	int32_t mStartZ_mm = 0;

	int32_t mEndX_mm = 0;
	int32_t mEndY_mm = 0;
	int32_t mEndZ_mm = 0;

	double mVx_mmps = 0.0;
	double mVy_mmps = 0.0;
	double mVz_mmps = 0.0;

	double mSensorMountPitch_deg = -90.0;
	double mSensorMountRoll_deg = 0.0;
	double mSensorMountYaw_deg = 90.0;

	double mStartPitchOffset_deg = 0.0;
	double mStartRollOffset_deg = 0.0;
	double mStartYawOffset_deg = 0.0;

	double mEndPitchOffset_deg = 0.0;
	double mEndRollOffset_deg = 0.0;
	double mEndYawOffset_deg = 0.0;

	InterpTable_t mOrientationInterpTable;

	eTranslateToGroundModel	mTranslateToGroundModel = eTranslateToGroundModel::NONE;
	double	mTranslationDistance_m = 0.0;
	double	mTranslationThreshold_pct = 1.0;
	std::vector<rfm::sPointCloudTranslationInterpPoint_t> mTranslateInterpTable;

	eRotateToGroundModel	mRotateToGroundModel = eRotateToGroundModel::NONE;
	double	mRotationPitch_deg = 0.0;
	double	mRotationRoll_deg = 0.0;
	double	mRotationThreshold_pct = 1.0;
	std::vector<rfm::sPointCloudRotationInterpPoint_t>    mRotateInterpTable;

private:
	std::vector<rfm::sDollyInfo_t>			mDollyMovement;
	std::vector<rfm::sDollyOrientation_t>	mDollyOrientation;
};
