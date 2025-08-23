
#include "LidarMapConfigScan.hpp"

#include "Constants.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


namespace lidar_config
{
	inline int to_date(int month, int day)
	{
		return month * 100 + day;
	}
}

namespace
{
	bool operator!=(const std::optional<rfm::rappPoint_t>& lhs, const std::optional<rfm::rappPoint_t>& rhs)
	{
		if (!lhs.has_value() && !rhs.has_value())
			return false;

		if (lhs.has_value() != rhs.has_value())
			return true;

		return static_cast<rfm::rappPoint_t>(lhs.value()) != static_cast<rfm::rappPoint_t>(rhs.value());
	}
}

cLidarMapConfigScan::cLidarMapConfigScan()
{}

cLidarMapConfigScan::cLidarMapConfigScan(const std::string& name)
: mMeasurementName(name)
{}

void cLidarMapConfigScan::clear()
{
	mDirty = false;

	mMeasurementName.clear();
	mSensorMountPitch_deg.reset();
	mSensorMountRoll_deg.reset();
	mSensorMountYaw_deg.reset();
	mReferencePoint.reset();

	mMinDistance_m.reset();
	mMaxDistance_m.reset();
	mMinAzimuth_deg.reset();
	mMaxAzimuth_deg.reset();
	mMinAltitude_deg.reset();
	mMaxAltitude_deg.reset();

	mKinematics.clear();
}

bool cLidarMapConfigScan::empty() const
{
	bool has_value = false;

	has_value |= mMinDistance_m.has_value();
	has_value |= mMaxDistance_m.has_value();
	has_value |= mMinAzimuth_deg.has_value();
	has_value |= mMaxAzimuth_deg.has_value();
	has_value |= mMinAltitude_deg.has_value();
	has_value |= mMaxAltitude_deg.has_value();

	return mMeasurementName.empty();
}

bool cLidarMapConfigScan::isDirty() const
{
	bool dirty = mDirty;

	dirty |= mKinematics.isDirty();

	return dirty;
}

const std::string& cLidarMapConfigScan::getMeasurementName() const
{
	return mMeasurementName;
}

const std::optional<double>& cLidarMapConfigScan::getSensorMountPitch_deg() const { return mSensorMountPitch_deg; }
const std::optional<double>& cLidarMapConfigScan::getSensorMountRoll_deg() const { return mSensorMountRoll_deg; }
const std::optional<double>& cLidarMapConfigScan::getSensorMountYaw_deg() const { return mSensorMountYaw_deg; }

const std::optional<rfm::rappPoint_t>& cLidarMapConfigScan::getReferencePoint() const
{
	return mReferencePoint; 
}

const std::optional<double>& cLidarMapConfigScan::getMinDistance_m() const { return mMinDistance_m; }
const std::optional<double>& cLidarMapConfigScan::getMaxDistance_m() const { return mMaxDistance_m; }
const std::optional<double>& cLidarMapConfigScan::getMinAzimuth_deg() const { return mMinAzimuth_deg; }
const std::optional<double>& cLidarMapConfigScan::getMaxAzimuth_deg() const { return mMaxAzimuth_deg; }
const std::optional<double>& cLidarMapConfigScan::getMinAltitude_deg() const { return mMinAltitude_deg; }
const std::optional<double>& cLidarMapConfigScan::getMaxAltitude_deg() const { return mMaxAltitude_deg; }

const std::optional<eKinematicModel>* cLidarMapConfigScan::getKinematicModel(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getKinematicModel());

	return &(it->second.getKinematicModel());
}

const std::optional<eKinematicModel>* cLidarMapConfigScan::getKinematicModel(int month, int day) const
{
	return getKinematicModel(lidar_config::to_date(month, day));
}

const std::optional<eOrientationModel>* cLidarMapConfigScan::getOrientationModel(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getOrientationModel());

	return &(it->second.getOrientationModel());
}

const std::optional<eOrientationModel>* cLidarMapConfigScan::getOrientationModel(int month, int day) const
{
	return getOrientationModel(lidar_config::to_date(month, day));
}

const std::optional<eTranslateToGroundModel>* cLidarMapConfigScan::getTranslateToGroundModel(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getTranslateToGroundModel());

	return &(it->second.getTranslateToGroundModel());
}

const std::optional<eTranslateToGroundModel>* cLidarMapConfigScan::getTranslateToGroundModel(int month, int day) const
{
	return getTranslateToGroundModel(lidar_config::to_date(month, day));
}

const std::optional<eRotateToGroundModel>* cLidarMapConfigScan::getRotateToGroundModel(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getRotateToGroundModel());

	return &(it->second.getRotateToGroundModel());
}

const std::optional<eRotateToGroundModel>* cLidarMapConfigScan::getRotateToGroundModel(int month, int day) const
{
	return getRotateToGroundModel(lidar_config::to_date(month, day));
}


/*** Sensor Orientation Parameters - Constant: Angles ***/
const std::optional<double>* cLidarMapConfigScan::getSensorPitchOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getSensorPitchOffset_deg());

	return &(it->second.getSensorPitchOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getSensorRollOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getSensorRollOffset_deg());

	return &(it->second.getSensorRollOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getSensorYawOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getSensorYawOffset_deg());

	return &(it->second.getSensorYawOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getSensorPitchOffset_deg(int month, int day) const { return getSensorPitchOffset_deg(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getSensorRollOffset_deg(int month, int day) const { return getSensorRollOffset_deg(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getSensorYawOffset_deg(int month, int day) const { return getSensorYawOffset_deg(lidar_config::to_date(month, day)); }

/*** Sensor Orientation Parameters - Linear: Start and Stop Angles ***/
const std::optional<double>* cLidarMapConfigScan::getStartPitchOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getStartPitchOffset_deg());

	return &(it->second.getStartPitchOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getStartRollOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getStartRollOffset_deg());

	return &(it->second.getStartRollOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getStartYawOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getStartYawOffset_deg());

	return &(it->second.getStartYawOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getEndPitchOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getEndPitchOffset_deg());

	return &(it->second.getEndPitchOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getEndRollOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getEndRollOffset_deg());

	return &(it->second.getEndRollOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getEndYawOffset_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getEndYawOffset_deg());

	return &(it->second.getEndYawOffset_deg());
}

const std::optional<double>* cLidarMapConfigScan::getStartPitchOffset_deg(int month, int day) const { return getStartPitchOffset_deg(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getStartRollOffset_deg(int month, int day) const { return getStartRollOffset_deg(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getStartYawOffset_deg(int month, int day) const { return getStartYawOffset_deg(lidar_config::to_date(month, day)); }

const std::optional<double>* cLidarMapConfigScan::getEndPitchOffset_deg(int month, int day) const { return getEndPitchOffset_deg(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getEndRollOffset_deg(int month, int day) const { return getEndRollOffset_deg(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getEndYawOffset_deg(int month, int day) const { return getEndYawOffset_deg(lidar_config::to_date(month, day)); }

/*** Sensor Orientation Parameters - Intrep Table: Data Points ***/
const std::vector<kdt::sDollyOrientationInterpPoint_t>* cLidarMapConfigScan::getOrientationTable(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getOrientationTable());

	return &(it->second.getOrientationTable());
}

const std::vector<kdt::sDollyOrientationInterpPoint_t>* cLidarMapConfigScan::getOrientationTable(int month, int day) const
{
	return getOrientationTable(lidar_config::to_date(month, day));
}

/*** Sensor Translation Parameters - Start/End Positions ***/
const std::optional<double>* cLidarMapConfigScan::getStart_X_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getStart_X_m());

	return &(it->second.getStart_X_m());
}

const std::optional<double>* cLidarMapConfigScan::getStart_Y_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getStart_Y_m());

	return &(it->second.getStart_Y_m());
}

const std::optional<double>* cLidarMapConfigScan::getStart_Z_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getStart_Z_m());

	return &(it->second.getStart_Z_m());
}

const std::optional<double>* cLidarMapConfigScan::getEnd_X_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getEnd_X_m());

	return &(it->second.getEnd_X_m());
}

const std::optional<double>* cLidarMapConfigScan::getEnd_Y_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getEnd_Y_m());

	return &(it->second.getEnd_Y_m());
}

const std::optional<double>* cLidarMapConfigScan::getEnd_Z_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getEnd_Z_m());

	return &(it->second.getEnd_Z_m());
}

/*** Sensor Translation Parameters - Constant: Speeds ***/
const std::optional<double>* cLidarMapConfigScan::getVx_mmps(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getVx_mmps());

	return &(it->second.getVx_mmps());
}

const std::optional<double>* cLidarMapConfigScan::getVy_mmps(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getVy_mmps());

	return &(it->second.getVy_mmps());
}

const std::optional<double>* cLidarMapConfigScan::getVz_mmps(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getVz_mmps());

	return &(it->second.getVz_mmps());
}

const std::optional<double>* cLidarMapConfigScan::getVx_mmps(int month, int day) const { return getVx_mmps(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getVy_mmps(int month, int day) const { return getVy_mmps(lidar_config::to_date(month, day)); }
const std::optional<double>* cLidarMapConfigScan::getVz_mmps(int month, int day) const { return getVz_mmps(lidar_config::to_date(month, day)); }

/*** Translate Point Cloud Parameters ***/
const std::optional<bool>* cLidarMapConfigScan::getTranslateToGround(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getTranslateToGround());

	return &(it->second.getTranslateToGround());
}

const std::optional<double>* cLidarMapConfigScan::getTranslateDistance_m(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getTranslateDistance_m());

	return &(it->second.getTranslateDistance_m());
}

const std::optional<double>* cLidarMapConfigScan::getTranslateThreshold_pct(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getTranslateThreshold_pct());

	return &(it->second.getTranslateThreshold_pct());
}

const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>* cLidarMapConfigScan::getTranslateTable(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getTranslateTable());

	return &(it->second.getTranslateTable());
}

/*** Rotate Point Cloud Parameters ***/
const std::optional<bool>* cLidarMapConfigScan::getRotateToGround(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getRotateToGround());

	return &(it->second.getRotateToGround());
}

const std::optional<double>* cLidarMapConfigScan::getRotatePitch_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getRotatePitch_deg());

	return &(it->second.getRotatePitch_deg());
}

const std::optional<double>* cLidarMapConfigScan::getRotateRoll_deg(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getRotateRoll_deg());

	return &(it->second.getRotateRoll_deg());
}

const std::optional<double>* cLidarMapConfigScan::getRotateThreshold_pct(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getRotateThreshold_pct());

	return &(it->second.getRotateThreshold_pct());
}

const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>* cLidarMapConfigScan::getRotateTable(int date) const
{
	if (mKinematics.empty())
		return nullptr;

	auto it = find(date);
	if (it == mKinematics.end())
		return &(mKinematics.begin()->second.getRotateTable());

	return &(it->second.getRotateTable());
}


bool cLidarMapConfigScan::contains(const int date) const
{
	return mKinematics.contains(date);
}

bool cLidarMapConfigScan::contains(const int month, const int day) const
{
	return contains(lidar_config::to_date(month, day));
}

const cLidarMapConfigKinematicParameters& cLidarMapConfigScan::front() const
{
	if (mKinematics.empty())
		throw std::logic_error("oops");

	return mKinematics.front();
}

cLidarMapConfigKinematicParameters& cLidarMapConfigScan::front()
{
	if (mKinematics.empty())
		throw std::logic_error("oops");

	return mKinematics.front();
}

cLidarMapConfigScan::iterator cLidarMapConfigScan::begin()
{
	return mKinematics.begin();
}

cLidarMapConfigScan::iterator cLidarMapConfigScan::end()
{
	return mKinematics.end();
}

cLidarMapConfigScan::const_iterator	cLidarMapConfigScan::begin() const
{
	return mKinematics.begin();
}

cLidarMapConfigScan::const_iterator	cLidarMapConfigScan::end() const
{
	return mKinematics.end();
}

cLidarMapConfigScan::const_iterator	cLidarMapConfigScan::find(const int date) const
{
	return mKinematics.find(date);
}

cLidarMapConfigScan::iterator cLidarMapConfigScan::find(const int date)
{
	return mKinematics.find(date);
}

cLidarMapConfigScan::const_iterator	cLidarMapConfigScan::find(const int month, const int day) const
{
	return mKinematics.find(month, day);
}

cLidarMapConfigScan::iterator cLidarMapConfigScan::find(const int month, const int day)
{
	return mKinematics.find(month, day);
}

cLidarMapConfigScan::const_iterator	cLidarMapConfigScan::find_exact(const int month, const int day) const
{
	return mKinematics.find_exact(month, day);
}

cLidarMapConfigScan::iterator cLidarMapConfigScan::find_exact(const int month, const int day)
{
	return mKinematics.find_exact(month, day);
}


void cLidarMapConfigScan::setMeasurementName(const std::string& name)
{
	mDirty |= (mMeasurementName != name);
	mMeasurementName = name;
}

void cLidarMapConfigScan::resetSensorMountPitch_deg()
{
	reset(mSensorMountPitch_deg);
}

void cLidarMapConfigScan::resetSensorMountRoll_deg()
{
	reset(mSensorMountRoll_deg);
}

void cLidarMapConfigScan::resetSensorMountYaw_deg()
{
	reset(mSensorMountYaw_deg);
}

void cLidarMapConfigScan::setSensorMountPitch_deg(const std::optional<double>& pitch_deg)
{
	mDirty |= (mSensorMountPitch_deg != pitch_deg);
	mSensorMountPitch_deg = pitch_deg;
}

void cLidarMapConfigScan::setSensorMountRoll_deg(const std::optional<double>& roll_deg)
{
	mDirty |= (mSensorMountRoll_deg != roll_deg);
	mSensorMountRoll_deg = roll_deg;
}

void cLidarMapConfigScan::setSensorMountYaw_deg(const std::optional<double>& yaw_deg)
{
	mDirty |= (mSensorMountYaw_deg != yaw_deg);
	mSensorMountYaw_deg = yaw_deg;
}

void cLidarMapConfigScan::resetReferencePoint()
{
	reset(mReferencePoint);
}

void cLidarMapConfigScan::setReferencePoint(const std::optional<rfm::rappPoint_t>& rp)
{
	mDirty |= (mReferencePoint != rp);
	mReferencePoint = rp;
}

void cLidarMapConfigScan::resetMinDistance_m()
{
	reset(mMinDistance_m);
}

void cLidarMapConfigScan::resetMaxDistance_m()
{
	reset(mMaxDistance_m);
}

void cLidarMapConfigScan::resetMinAzimuth_deg()
{
	reset(mMinAzimuth_deg);
}

void cLidarMapConfigScan::resetMaxAzimuth_deg()
{
	reset(mMaxAzimuth_deg);
}

void cLidarMapConfigScan::resetMinAltitude_deg()
{
	reset(mMinAltitude_deg);
}

void cLidarMapConfigScan::resetMaxAltitude_deg()
{
	reset(mMaxAltitude_deg);
}

void cLidarMapConfigScan::setMinDistance_m(const std::optional<double>& dist_m)
{
	mDirty |= (mMinDistance_m != dist_m);
	mMinDistance_m = dist_m;
}

void cLidarMapConfigScan::setMaxDistance_m(const std::optional<double>& dist_m)
{
	mDirty |= (mMaxDistance_m != dist_m);
	mMaxDistance_m = dist_m;
}

void cLidarMapConfigScan::setMinAzimuth_deg(const std::optional<double>& azimuth_deg)
{
	mDirty |= (mMinAzimuth_deg != azimuth_deg);
	mMinAzimuth_deg = azimuth_deg;
}

void cLidarMapConfigScan::setMaxAzimuth_deg(const std::optional<double>& azimuth_deg)
{
	mDirty |= (mMaxAzimuth_deg != azimuth_deg);
	mMaxAzimuth_deg = azimuth_deg;
}

void cLidarMapConfigScan::setMinAltitude_deg(const std::optional<double>& altitude_deg)
{
	mDirty |= (mMinAltitude_deg != altitude_deg);
	mMinAltitude_deg = altitude_deg;
}

void cLidarMapConfigScan::setMaxAltitude_deg(const std::optional<double>& altitude_deg)
{
	mDirty |= (mMaxAltitude_deg != altitude_deg);
	mMaxAltitude_deg = altitude_deg;
}

cLidarMapConfigKinematicParameters& cLidarMapConfigScan::add(const int date)
{
	mDirty = true;
	return mKinematics.add(date);
}

cLidarMapConfigKinematicParameters& cLidarMapConfigScan::add(const int month, const int day)
{
	mDirty = true;
	return mKinematics.add(month, day);
}

void cLidarMapConfigScan::clearDirtyFlag()
{
	setDirty(false);
}

void cLidarMapConfigScan::setDirtyFlag()
{
	setDirty(true);
}

void cLidarMapConfigScan::setDirty(bool dirty)
{
	mDirty = dirty;
	mKinematics.setDirtyFlag(dirty);
}


void cLidarMapConfigScan::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("experiment_name"))
		mMeasurementName = jdoc["experiment_name"];

	if (jdoc.contains("experiment name"))
		mMeasurementName = jdoc["experiment name"];

	if (jdoc.contains("measurement_name"))
		mMeasurementName = jdoc["measurement_name"];

	if (jdoc.contains("measurement name"))
		mMeasurementName = jdoc["measurement name"];


	if (jdoc.contains("sensor mount orientation"))
	{
		auto orientation = jdoc["sensor mount orientation"];

		if (orientation.contains("pitch (deg)"))
			mSensorMountPitch_deg = orientation["pitch (deg)"];

		if (orientation.contains("roll (deg)"))
			mSensorMountRoll_deg = orientation["roll (deg)"];

		if (orientation.contains("yaw (deg)"))
			mSensorMountYaw_deg = orientation["yaw (deg)"];
	}

	if (jdoc.contains("sensor limits"))
	{
		auto sensor_limits = jdoc["sensor limits"];

		if (sensor_limits.contains("min distance (m)"))
			mMinDistance_m = sensor_limits["min distance (m)"].get<double>();

		if (sensor_limits.contains("max distance (m)"))
			mMaxDistance_m = sensor_limits["max distance (m)"].get<double>();

		if (sensor_limits.contains("min azimuth (deg)"))
			mMinAzimuth_deg = sensor_limits["min azimuth (deg)"].get<double>();

		if (sensor_limits.contains("max azimuth (deg)"))
			mMaxAzimuth_deg = sensor_limits["max azimuth (deg)"].get<double>();

		if (sensor_limits.contains("min altitude (deg)"))
			mMinAltitude_deg = sensor_limits["min altitude (deg)"].get<double>();

		if (sensor_limits.contains("max altitude (deg)"))
			mMaxAltitude_deg = sensor_limits["max altitude (deg)"].get<double>();
	}

	if (jdoc.contains("reference_point"))
	{
		auto reference_point = jdoc["reference_point"];

		rfm::rappPoint_t p;
		p.x_mm = reference_point["x (mm)"];
		p.y_mm = reference_point["y (mm)"];
		p.z_mm = reference_point["z (mm)"];

		mReferencePoint = p;
	}

	if (jdoc.contains("kinematics"))
	{
		const auto& kinematics = jdoc["kinematics"];
		mKinematics.load(kinematics);
	}
}

nlohmann::json cLidarMapConfigScan::save()
{
	nlohmann::json scanDoc;

	scanDoc["measurement name"] = mMeasurementName;

	if (mSensorMountPitch_deg.has_value() || mSensorMountRoll_deg.has_value() || mSensorMountYaw_deg.has_value())
	{
		nlohmann::json orientation;

		if (mSensorMountPitch_deg.has_value())
			orientation["pitch (deg)"] = mSensorMountPitch_deg.value();

		if (mSensorMountRoll_deg.has_value())
			orientation["roll (deg)"] = mSensorMountRoll_deg.value();

		if (mSensorMountYaw_deg.has_value())
			orientation["yaw (deg)"] = mSensorMountYaw_deg.value();

		scanDoc["sensor mount orientation"] = orientation;
	}

	if (mReferencePoint.has_value())
	{
		nlohmann::json reference_point;

		rfm::rappPoint_t p = mReferencePoint.value();

		reference_point["x (mm)"] = p.x_mm;
		reference_point["y (mm)"] = p.y_mm;
		reference_point["z (mm)"] = p.z_mm;

		scanDoc["reference_point"] = reference_point;
	}

	if (mMinDistance_m.has_value() || mMaxDistance_m.has_value() || mMinAzimuth_deg.has_value()
		|| mMaxAzimuth_deg.has_value() || mMinAltitude_deg.has_value() || mMaxAltitude_deg.has_value())
	{
		nlohmann::json sensor_limits;

		if (mMinDistance_m.has_value())
			sensor_limits["min distance (m)"] = mMinDistance_m.value();

		if (mMaxDistance_m.has_value())
			sensor_limits["max distance (m)"] = mMaxDistance_m.value();

		if (mMinAzimuth_deg.has_value())
			sensor_limits["min azimuth (deg)"] = mMinAzimuth_deg.value();

		if (mMaxAzimuth_deg.has_value())
			sensor_limits["max azimuth (deg)"] = mMaxAzimuth_deg.value();

		if (mMinAltitude_deg.has_value())
			sensor_limits["min altitude (deg)"] = mMinAltitude_deg.value();

		if (mMaxAltitude_deg.has_value())
			sensor_limits["max altitude (deg)"] = mMaxAltitude_deg.value();

		scanDoc["sensor limits"] = sensor_limits;
	}

	auto kinematicDoc = mKinematics.save();

	if (!kinematicDoc.is_null())
		scanDoc["kinematics"] = kinematicDoc;

	mDirty = false;

	return scanDoc;
}


