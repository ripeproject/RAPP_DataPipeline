
#include "lidar2pointcloud.hpp"
#include "Constants.hpp"
#include "MathUtils.hpp"
#include "KinematicUtils.hpp"
#include "PointCloudGenerator.hpp"

#include <cbdf/PointCloudTypes.hpp>

#include <ouster/simple_blas.h>
#include <ouster/ouster_utils.h>

#include <eigen3/Eigen/Eigen>

#include <cassert>
#include <algorithm>
#include <numbers>
#include <cmath>
#include <valarray>
#include <iostream>

using namespace ouster;
using namespace nMathUtils;


extern void console_message(const std::string& msg);
extern void update_prefix_progress(const int id, std::string prefix, const int progress_pct);


namespace
{
	/** Unit of range from sensor packet, in meters. */
	constexpr double range_unit_m = 0.001;
	constexpr double range_unit_mm = 1.0;

	constexpr double g_mps2 = 9.80665;

	pointcloud::eKINEMATIC_MODEL add_sensor_rotation(pointcloud::eKINEMATIC_MODEL in)
	{
		switch (in)
		{
		case pointcloud::eKINEMATIC_MODEL::CONSTANT:
			return pointcloud::eKINEMATIC_MODEL::CONSTANT_SENSOR_ROTATION;
		case pointcloud::eKINEMATIC_MODEL::DOLLY:
			return pointcloud::eKINEMATIC_MODEL::DOLLY_SENSOR_ROTATION;
		case pointcloud::eKINEMATIC_MODEL::GPS:
			return pointcloud::eKINEMATIC_MODEL::GPS_SENSOR_ROTATION;
		case pointcloud::eKINEMATIC_MODEL::GPS_SPEEDS:
			return pointcloud::eKINEMATIC_MODEL::GPS_SPEEDS_SENSOR_ROTATION;
		}

		return in;
	}
}


cLidar2PointCloud::cLidar2PointCloud(int id)
:
	mID(id), cFieldScanDataModel(id)
{}

cLidar2PointCloud::~cLidar2PointCloud()
{}

void cLidar2PointCloud::setValidRange_m(double min_dist_m, double max_dist_m)
{
	mMinDistance_m = std::max(min_dist_m, 0.001);
	mMaxDistance_m = std::min(max_dist_m, 1000.0);

	if (mMaxDistance_m < mMinDistance_m)
		std::swap(mMinDistance_m, mMaxDistance_m);
}

void cLidar2PointCloud::setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg)
{
	mMinAzimuth_deg = wrap_0_to_360(min_azimuth_deg);
	mMaxAzimuth_deg = wrap_0_to_360(max_azimuth_deg);

	if (mMaxAzimuth_deg < mMinAzimuth_deg)
		std::swap(mMinAzimuth_deg, mMaxAzimuth_deg);
}

void cLidar2PointCloud::setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg)
{
	mMinAltitude_deg = min_altitude_deg;
	mMaxAltitude_deg = max_altitude_deg;

	if (mMaxAltitude_deg < mMinAltitude_deg)
		std::swap(mMinAltitude_deg, mMaxAltitude_deg);
}

void cLidar2PointCloud::setReferencePosition_mm(int32_t x_mm, int32_t y_mm, int32_t z_mm)
{
	mHasReferencePoint = true;
	mRefX_mm = x_mm;
	mRefY_mm = y_mm;
	mRefZ_mm = z_mm;
}

void cLidar2PointCloud::setInitialPosition_m(double x_m, double y_m, double z_m)
{
	mStartX_mm = static_cast<int32_t>(x_m * nConstants::M_TO_MM);
	mStartY_mm = static_cast<int32_t>(y_m * nConstants::M_TO_MM);
	mStartZ_mm = static_cast<int32_t>(z_m * nConstants::M_TO_MM);
}

void cLidar2PointCloud::setFinalPosition_m(double x_m, double y_m, double z_m)
{
	mEndX_mm = static_cast<int32_t>(x_m * nConstants::M_TO_MM);
	mEndY_mm = static_cast<int32_t>(y_m * nConstants::M_TO_MM);
	mEndZ_mm = static_cast<int32_t>(z_m * nConstants::M_TO_MM);
}

void cLidar2PointCloud::setDollySpeed(double Vx_mmps, double Vy_mmps, double Vz_mmps)
{
	mVx_mmps = Vx_mmps;
	mVy_mmps = Vy_mmps;
	mVz_mmps = Vz_mmps;
}

void cLidar2PointCloud::setSensorMountOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
	mSensorMountPitch_deg = pitch_deg;
	mSensorMountRoll_deg = roll_deg;
	mSensorMountYaw_deg = yaw_deg;
}

void cLidar2PointCloud::setOrientationOffset_deg(double yaw_deg, double pitch_deg, double roll_deg)
{
	mEndPitchOffset_deg = mStartPitchOffset_deg = pitch_deg;
	mEndRollOffset_deg = mStartRollOffset_deg = roll_deg;
	mEndYawOffset_deg = mStartYawOffset_deg = yaw_deg;
}

void cLidar2PointCloud::setInitialOffset_deg(double yaw_deg, double pitch_deg, double roll_deg)
{
	mStartPitchOffset_deg = pitch_deg;
	mStartRollOffset_deg = roll_deg;
	mStartYawOffset_deg = yaw_deg;
}

void cLidar2PointCloud::setFinalOffset_deg(double yaw_deg, double pitch_deg, double roll_deg)
{
	mEndPitchOffset_deg = pitch_deg;
	mEndRollOffset_deg = roll_deg;
	mEndYawOffset_deg = yaw_deg;
}

void cLidar2PointCloud::setOrientationInterpTable(const InterpTable_t& table)
{
	mOrientationInterpTable = table;
}


void cLidar2PointCloud::setTranslateToGroundModel(eTranslateToGroundModel model)
{
	mTranslateToGroundModel = model;
}

void cLidar2PointCloud::setTranslateDistance_m(double distance_m)
{
	mTranslationDistance_m = distance_m;
}

void cLidar2PointCloud::setTranslateThreshold_pct(double threshold_pct)
{
	if (threshold_pct < 1.0)
		mTranslationThreshold_pct = 1.0;
	else if (threshold_pct > 100.0)
		mTranslationThreshold_pct = 100.0;
	else
		mTranslationThreshold_pct = threshold_pct;
}

void cLidar2PointCloud::setTranslateInterpTable(const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& table)
{
	mTranslateInterpTable = table;
}

void cLidar2PointCloud::setRotationToGroundModel(eRotateToGroundModel model)
{
	mRotateToGroundModel = model;
}

void cLidar2PointCloud::setRotationAngles_deg(double pitch_deg, double roll_deg)
{
	mRotationPitch_deg = pitch_deg;
	mRotationRoll_deg = roll_deg;
}

void cLidar2PointCloud::setRotationThreshold_pct(double threshold_pct)
{
	if (threshold_pct < 1.0)
		mRotationThreshold_pct = 1.0;
	else if (threshold_pct > 100.0)
		mRotationThreshold_pct = 100.0;
	else
		mRotationThreshold_pct = threshold_pct;
}

void cLidar2PointCloud::setRotateInterpTable(const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& table)
{
	mRotateInterpTable = table;
}

pointcloud::eKINEMATIC_MODEL cLidar2PointCloud::getKinematicModel() const
{
	return mKinematicModel;
}

const std::vector<kdt::sDollyInfo_t>& cLidar2PointCloud::getComputedDollyPath() const
{
	auto pPointCloudGenerator = getOusterInfo()->getPointCloudGenerator().lock();
	return pPointCloudGenerator->getComputedDollyPath();
}

const cRappPointCloud& cLidar2PointCloud::getPointCloud() const
{
	auto pPointCloudGenerator = getOusterInfo()->getPointCloudGenerator().lock();
	return pPointCloudGenerator->getPointCloud();
}

cRappPointCloud& cLidar2PointCloud::getPointCloudRef() const
{
	auto pPointCloudGenerator = getOusterInfo()->getPointCloudGenerator().lock();
	return pPointCloudGenerator->getPointCloud();
}

bool cLidar2PointCloud::computeDollyMovement()
{
	update_prefix_progress(mID, "Computing Dolly Movement...", 0);

	validateStartPosition(mStartX_mm, mStartY_mm, mStartZ_mm);
	validateEndPosition(mEndX_mm, mEndY_mm, mEndZ_mm);

	if (!hasGpsData() && !hasSpiderData())
	{
		computeDollyMovement_ConstantSpeed();
	}
	else if (!hasGpsData())
	{
		computeDollyMovement_SpiderCam();
	}
	else
	{
		computeDollyMovement_GpsSpeeds();
	}

	return mDollyMovement.size() > 0;
}

bool cLidar2PointCloud::computeDollyOrientation()
{
	update_prefix_progress(mID, "Computing Dolly Orientation...", 0);

	if (mOrientationInterpTable.empty())
	{
		bool orientationConstant = ((mStartPitchOffset_deg == mEndPitchOffset_deg)
			&& (mStartRollOffset_deg == mEndRollOffset_deg) && (mStartYawOffset_deg == mEndYawOffset_deg));

		if (orientationConstant)
		{
			computeDollyOrientation_Constant();
		}
		else
		{
			computeDollyOrientation_ConstantSpeed();
		}
	}
	else
	{
		computeDollyOrientation_InterpTable();
	}

	return mDollyOrientation.size() > 0;
}

bool cLidar2PointCloud::computePointCloud()
{
	update_prefix_progress(mID, "Merging Dolly Movement...", 0);
	mergeDollyOrientation(mID, mDollyMovement, mDollyOrientation);

	auto pPointCloudGenerator = getOusterInfo()->getPointCloudGenerator().lock();

	pPointCloudGenerator->setValidRange_m(mMinDistance_m, mMaxDistance_m);
	pPointCloudGenerator->setAzimuthWindow_deg(mMinAzimuth_deg, mMaxAzimuth_deg);
	pPointCloudGenerator->setAltitudeWindow_deg(mMinAltitude_deg, mMaxAltitude_deg);

	auto ssnx = getSsnxInfo();
	if (ssnx)
	{
		if (ssnx->hasReferencePoint())
		{
			auto point = ssnx->getReferencePoint().value();

			auto ref_point = rfb::fromGPS(point.avgLat_rad, point.avgLng_rad, point.avgHeight_m);

			pPointCloudGenerator->setReferencePoint(ref_point, true);
		}
		else
		{
			const auto& gps = ssnx->getGeodeticPositions();

			if (gps.size() > 0)
			{
				auto point = gps[0];

				if (point.dataValid)
				{
					double height_m = point.Height_m - point.Undulation_m;
					auto ref_point = rfb::fromGPS(point.Lat_rad, point.Lon_rad, height_m);
					bool valid = point.HeightComputed && ((point.Mode == nSsnxTypes::eSolutionType::RTK_FIXED)
						|| (point.Mode == nSsnxTypes::eSolutionType::RTK_FLOAT));

					pPointCloudGenerator->setReferencePoint(ref_point, valid);
				}
			}
		}
	}


	if (!pPointCloudGenerator->referencePointValid())
	{
		std::string msg = "File \"";
		msg += getFileName();
		msg += "\" does not contain a valid reference point.";
		console_message(msg);
	}

	pPointCloudGenerator->setTranslateToGroundModel(mTranslateToGroundModel);

	switch (mTranslateToGroundModel)
	{
	case eTranslateToGroundModel::CONSTANT:
		pPointCloudGenerator->setTranslateDistance_m(mTranslationDistance_m);
		break;
	case eTranslateToGroundModel::FIT:
		pPointCloudGenerator->setTranslateThreshold_pct(mTranslationThreshold_pct);
		break;
	case eTranslateToGroundModel::INTREP_CURVE:
		pPointCloudGenerator->setTranslateInterpTable(mTranslateInterpTable);
		break;
	}


	pPointCloudGenerator->setRotationToGroundModel(mRotateToGroundModel);

	switch (mRotateToGroundModel)
	{
	case eRotateToGroundModel::CONSTANT:
		pPointCloudGenerator->setRotationAngles_deg(mRotationPitch_deg, mRotationRoll_deg);
		break;
	case eRotateToGroundModel::FIT:
		pPointCloudGenerator->setRotationThreshold_pct(mRotationThreshold_pct);
		break;
	case eRotateToGroundModel::INTREP_CURVE:
		pPointCloudGenerator->setRotateInterpTable(mRotateInterpTable);
		break;
	}

	pPointCloudGenerator->setDollyPath(mDollyMovement);

	update_prefix_progress(mID, "Generating Point Cloud...", 0);

	return pPointCloudGenerator->computePointCloud(mID);
}

void cLidar2PointCloud::computeDollyMovement_ConstantSpeed()
{
	double scanTime_sec = getScanTime_sec();
	rfm::rappPoint_t start_pos = { mStartX_mm, mStartY_mm, mStartZ_mm };
	rfm::rappPoint_t end_pos = { mEndX_mm, mEndY_mm, mEndZ_mm };

	rfm::rappSpeeds_t speeds = { mVx_mmps, mVy_mmps, mVz_mmps };

	double speed = sqrt(speeds.vx_mmps * speeds.vx_mmps + speeds.vy_mmps * speeds.vy_mmps + speeds.vz_mmps * speeds.vz_mmps);

	if (speed == 0)
	{
		console_message("Invalid Data: The constant speed model requires a non-zero speed!");
		return;
	}

	double dx = end_pos.x_mm - start_pos.x_mm;
	double dy = end_pos.y_mm - start_pos.y_mm;

	double posGroundTrack = nMathUtils::wrap_0_to_360(atan2(dy, dx) * nConstants::RAD_TO_DEG);
	double velGroundTrack = nMathUtils::wrap_0_to_360(atan2(speeds.vy_mmps, speeds.vx_mmps) * nConstants::RAD_TO_DEG);

	double deltaTrack = nMathUtils::wrap_0_to_360(std::abs(velGroundTrack - posGroundTrack));

	if ((178.0 < deltaTrack) && (deltaTrack < 182.0))
	{
		speeds.vx_mmps *= -1.0;
		speeds.vy_mmps *= -1.0;
		speeds.vz_mmps *= -1.0;
	}
	else if (deltaTrack > 2.0)
	{
		std::string msg = "Invalid Data: ";
		msg += "The ground track computed from the start/end positions does not match the ground track computed from the speed data. ";
		msg += "Please fix the speed numbers or the start/end positions.";
		console_message(msg);

		return;
	}

	mKinematicModel = pointcloud::eKINEMATIC_MODEL::CONSTANT;

	mDollyMovement = computeDollyKinematics(mID, start_pos, end_pos, speeds, &scanTime_sec);

	setGroundTrack_deg(posGroundTrack);
}

void cLidar2PointCloud::computeDollyMovement_SpiderCam()
{
	auto spiderCam = getSpiderCamInfo();
	if (!spiderCam)
	{
		console_message("Error: No SpiderCam Information!");
		return;
	}

	if (spiderCam->empty())
	{
		console_message("Invalid Data: No SpiderCam data has been loaded!");
		return;
	}

	auto start_index = spiderCam->getStartIndex();
	auto end_index = spiderCam->getEndIndex();

	mKinematicModel = pointcloud::eKINEMATIC_MODEL::DOLLY;

	mDollyMovement = computeDollyKinematics(mID, spiderCam->getPositionData(), start_index, end_index);
}

void cLidar2PointCloud::computeDollyMovement_GpsSpeeds()
{
	double scanTime_sec = getScanTime_sec();

	auto ssnx = getSsnxInfo();
	if (!ssnx)
	{
		console_message("Error: No GPS Information!");
		return;
	}

	auto gps = ssnx->getGeodeticPositions();

	if (gps.empty())
	{
		console_message("Invalid Data: No GPS velocity data has been loaded!");
		return;
	}

	rfm::rappPoint_t start = { mStartX_mm, mStartY_mm, mStartZ_mm };
	rfm::rappPoint_t end = { mEndX_mm, mEndY_mm, mEndZ_mm };

	auto spiderCam = getSpiderCamInfo();
	if (spiderCam && !spiderCam->empty())
	{
		auto start_pos = spiderCam->startPosition();
		auto end_pos = spiderCam->endPosition();

		start.x_mm = static_cast<int32_t>(start_pos.X_mm);
		start.y_mm = static_cast<int32_t>(start_pos.Y_mm);
		start.z_mm = static_cast<int32_t>(start_pos.Z_mm);

		end.x_mm = static_cast<int32_t>(end_pos.X_mm);
		end.y_mm = static_cast<int32_t>(end_pos.Y_mm);
		end.z_mm = static_cast<int32_t>(end_pos.Z_mm);
	}

	bool ignore = false;

	mKinematicModel = pointcloud::eKINEMATIC_MODEL::GPS_SPEEDS;

	mDollyMovement = computeDollyKinematics(mID, start, end, gps, ignore, &scanTime_sec);
}

void cLidar2PointCloud::computeDollyOrientation_Constant()
{
	double scanTime_sec = getScanTime_sec();

	mDollyOrientation = computeDollyOrientationKinematics(mID, mSensorMountPitch_deg,
		mSensorMountRoll_deg, mSensorMountYaw_deg,
		mStartPitchOffset_deg, mStartRollOffset_deg, mStartYawOffset_deg,
		scanTime_sec);
}

void cLidar2PointCloud::computeDollyOrientation_ConstantSpeed()
{
	mKinematicModel = add_sensor_rotation(mKinematicModel);

	double scanTime_sec = getScanTime_sec();

	kdt::sDollyAtitude_t start = { mStartPitchOffset_deg, mStartRollOffset_deg, mStartYawOffset_deg };
	kdt::sDollyAtitude_t end = { mEndPitchOffset_deg, mEndRollOffset_deg, mEndYawOffset_deg };

	mDollyOrientation = computeDollyOrientationKinematics(mID, mSensorMountPitch_deg,
		mSensorMountRoll_deg, mSensorMountYaw_deg,
		start, end, scanTime_sec);
}

void cLidar2PointCloud::computeDollyOrientation_InterpTable()
{
	mKinematicModel = add_sensor_rotation(mKinematicModel);

	double scanTime_sec = getScanTime_sec();

	mDollyOrientation = computeDollyOrientationKinematics(mID, mSensorMountPitch_deg,
		mSensorMountRoll_deg, mSensorMountYaw_deg,
		mOrientationInterpTable, scanTime_sec);
}

void cLidar2PointCloud::computeDollyOrientation_IMU()
{
	auto ouster = getOusterInfo();
	if (!ouster)
		return;

	auto imu = ouster->getImuData();

	if (imu.empty())
		return;

	auto intrinsics = ouster->getImuIntrinsics();

	if (!intrinsics.has_value())
		return;

	auto transform = intrinsics.value();

	mDollyOrientation = computeDollyOrientationKinematics(mID, mSensorMountPitch_deg,
		mSensorMountRoll_deg, mSensorMountYaw_deg, imu, transform);
}

void cLidar2PointCloud::clearLidarData()
{
	auto pPointCloudGenerator = getOusterInfo()->getPointCloudGenerator().lock();
	pPointCloudGenerator->clearLidarData();
}


