
#include "LidarMapConfigScan.hpp"

#include "Constants.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

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

void cLidarMapConfigScan::clear()
{
	mDirty = false;
}

bool cLidarMapConfigScan::isDirty() const
{
	bool dirty = mDirty;

	return dirty;
}

const std::string& cLidarMapConfigScan::getExperimentName() const
{
	return mExperimentName;
}

const std::optional<eKinematicModel>& cLidarMapConfigScan::getKinematicModel() const { return mKinematicModel; }
const std::optional<eOrientationModel>& cLidarMapConfigScan::getOrientationModel() const { return mOrientationModel; }
const std::optional<eTranslateToGroundModel>& cLidarMapConfigScan::getTranslateToGroundModel() const { return mTranslateToGroundModel; }
const std::optional<eRotateToGroundModel>& cLidarMapConfigScan::getRotateToGroundModel() const { return mRotateToGroundModel; }

const std::optional<double>& cLidarMapConfigScan::getMinDistance_m() const { return mMinDistance_m; }
const std::optional<double>& cLidarMapConfigScan::getMaxDistance_m() const { return mMaxDistance_m; }
const std::optional<double>& cLidarMapConfigScan::getMinAzimuth_deg() const { return mMinAzimuth_deg; }
const std::optional<double>& cLidarMapConfigScan::getMaxAzimuth_deg() const { return mMaxAzimuth_deg; }
const std::optional<double>& cLidarMapConfigScan::getMinAltitude_deg() const { return mMinAltitude_deg; }
const std::optional<double>& cLidarMapConfigScan::getMaxAltitude_deg() const { return mMaxAltitude_deg; }

const std::optional<double>& cLidarMapConfigScan::getVx_mmps() const { return mVx_mmps; }
const std::optional<double>& cLidarMapConfigScan::getVy_mmps() const { return mVy_mmps; }
const std::optional<double>& cLidarMapConfigScan::getVz_mmps() const { return mVz_mmps; }

const std::optional<double>& cLidarMapConfigScan::getSensorMountPitch_deg() const { return mSensorMountPitch_deg; }
const std::optional<double>& cLidarMapConfigScan::getSensorMountRoll_deg() const { return mSensorMountRoll_deg; }
const std::optional<double>& cLidarMapConfigScan::getSensorMountYaw_deg() const { return mSensorMountYaw_deg; }

const std::optional<double>& cLidarMapConfigScan::getSensorPitchOffset_deg() const { return mSensorPitchOffset_deg; }
const std::optional<double>& cLidarMapConfigScan::getSensorRollOffset_deg() const { return mSensorRollOffset_deg; }
const std::optional<double>& cLidarMapConfigScan::getSensorYawOffset_deg() const { return mSensorYawOffset_deg; }

const std::optional<double>& cLidarMapConfigScan::getStartPitchOffset_deg() const { return mStartPitchOffset_deg; }
const std::optional<double>& cLidarMapConfigScan::getStartRollOffset_deg() const { return mStartRollOffset_deg; }
const std::optional<double>& cLidarMapConfigScan::getStartYawOffset_deg() const { return mStartYawOffset_deg; }

const std::optional<double>& cLidarMapConfigScan::getEndPitchOffset_deg() const { return mEndPitchOffset_deg; }
const std::optional<double>& cLidarMapConfigScan::getEndRollOffset_deg() const { return mEndRollOffset_deg; }
const std::optional<double>& cLidarMapConfigScan::getEndYawOffset_deg() const { return mEndYawOffset_deg; }

const std::vector<kdt::sDollyOrientationInterpPoint_t>& cLidarMapConfigScan::getOrientationTable() const { return mOrientationTable; }

const std::optional<bool>&   cLidarMapConfigScan::getTranslateToGround() const { return mTranslateToGround; }
const std::optional<double>& cLidarMapConfigScan::getTranslateDistance_m() const { return mTranslateDistance_m; }
const std::optional<double>& cLidarMapConfigScan::getTranslateThreshold_pct() const { return mTranslateThreshold_pct; }

const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& cLidarMapConfigScan::getTranslateTable() const { return mTranslateTable; }

const std::optional<bool>&   cLidarMapConfigScan::getRotateToGround() const { return mRotateToGround; }
const std::optional<double>& cLidarMapConfigScan::getRotatePitch_deg() const { return mRotatePitch_deg; }
const std::optional<double>& cLidarMapConfigScan::getRotateRoll_deg() const { return mRotateRoll_deg; }
const std::optional<double>& cLidarMapConfigScan::getRotateThreshold_pct() const { return mRotateThreshold_pct; }

const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& cLidarMapConfigScan::getRotateTable() const { return mRotateTable; }

const std::optional<double>& cLidarMapConfigScan::getStart_X_m() const { return mStart_X_m; }
const std::optional<double>& cLidarMapConfigScan::getStart_Y_m() const { return mStart_Y_m; }
const std::optional<double>& cLidarMapConfigScan::getStart_Z_m() const { return mStart_Z_m; }

const std::optional<double>& cLidarMapConfigScan::getEnd_X_m() const { return mEnd_X_m; }
const std::optional<double>& cLidarMapConfigScan::getEnd_Y_m() const { return mEnd_Y_m; }
const std::optional<double>& cLidarMapConfigScan::getEnd_Z_m() const { return mEnd_Z_m; }

const std::optional<rfm::rappPoint_t>& cLidarMapConfigScan::getReferencePoint() const { return mReferencePoint; }


void cLidarMapConfigScan::setExperimentName(const std::string& name)
{
	mDirty |= (mExperimentName != name);
	mExperimentName = name;
}

void cLidarMapConfigScan::resetKinematicModel()
{
	reset(mKinematicModel);
}

void cLidarMapConfigScan::setKinematicModel(const std::optional<eKinematicModel>& model)
{
	mDirty |= (mKinematicModel != model);
	mKinematicModel = model;
}

void cLidarMapConfigScan::resetOrientationModel()
{
	reset(mOrientationModel);
}

void cLidarMapConfigScan::setOrientationModel(const std::optional<eOrientationModel>& model)
{
	mDirty |= (mOrientationModel != model);
	mOrientationModel = model;
}

void cLidarMapConfigScan::resetTranslateToGroundModel()
{
	reset(mTranslateToGroundModel);
}

void cLidarMapConfigScan::setTranslateToGroundModel(const std::optional<eTranslateToGroundModel>& model)
{
	mDirty |= (mTranslateToGroundModel != model);
	mTranslateToGroundModel = model;
}

void cLidarMapConfigScan::resetRotateToGroundModel()
{
	reset(mRotateToGroundModel);
}

void cLidarMapConfigScan::setRotateToGroundModel(const std::optional<eRotateToGroundModel>& model)
{
	mDirty |= (mRotateToGroundModel != model);
	mRotateToGroundModel = model;
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

void cLidarMapConfigScan::setVx_mmps(const std::optional<double>& vx_mmps)
{
	mDirty |= (mVx_mmps != vx_mmps);
	mVx_mmps = vx_mmps;
}

void cLidarMapConfigScan::setVy_mmps(const std::optional<double>& vy_mmps)
{
	mDirty |= (mVy_mmps != vy_mmps);
	mVy_mmps = vy_mmps;
}

void cLidarMapConfigScan::setVz_mmps(const std::optional<double>& vz_mmps)
{
	mDirty |= (mVz_mmps != vz_mmps);
	mVz_mmps = vz_mmps;
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

void cLidarMapConfigScan::setSensorPitchOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mSensorPitchOffset_deg != offset_deg);
	mSensorPitchOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setSensorRollOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mSensorRollOffset_deg != offset_deg);
	mSensorRollOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setSensorYawOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mSensorYawOffset_deg != offset_deg);
	mSensorYawOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setStartPitchOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mStartPitchOffset_deg != offset_deg);
	mStartPitchOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setStartRollOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mStartRollOffset_deg != offset_deg);
	mStartRollOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setStartYawOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mStartYawOffset_deg != offset_deg);
	mStartYawOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setEndPitchOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mEndPitchOffset_deg != offset_deg);
	mEndPitchOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setEndRollOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mEndRollOffset_deg != offset_deg);
	mEndRollOffset_deg = offset_deg;
}

void cLidarMapConfigScan::setEndYawOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mEndYawOffset_deg != offset_deg);
	mEndYawOffset_deg = offset_deg;
}

void cLidarMapConfigScan::clearOrientationTable()
{
	mDirty |= !mOrientationTable.empty();
	mOrientationTable.clear();
}

void cLidarMapConfigScan::setOrientationTable(const std::vector<kdt::sDollyOrientationInterpPoint_t>& table)
{
	if (!mDirty)
	{
		if (mOrientationTable.size() != table.size())
		{
			mDirty = true;
		}
		else
		{
			auto n = mOrientationTable.size();
			for (size_t i = 0; i < n; ++i)
			{
				if (mOrientationTable[i] != table[i])
				{
					mDirty = true;
					break;
				}
			}
		}
	}

	mOrientationTable = table;
}

void cLidarMapConfigScan::setTranslateToGround(const std::optional<bool>& enable)
{
	mDirty |= (mTranslateToGround != enable);
	mTranslateToGround = enable;
}

void cLidarMapConfigScan::setTranslateDistance_m(const std::optional<double>& dist_m)
{
	mDirty |= (mTranslateDistance_m != dist_m);
	mTranslateDistance_m = dist_m;
}

void cLidarMapConfigScan::setTranslateThreshold_pct(const std::optional<double>& threshold_pct)
{
	mDirty |= (mTranslateThreshold_pct != threshold_pct);
	mTranslateThreshold_pct = threshold_pct;
}

void cLidarMapConfigScan::clearTranslateTable()
{
	mDirty |= !mTranslateTable.empty();
	mTranslateTable.clear();
}

void cLidarMapConfigScan::setTranslateTable(const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& table)
{
	if (!mDirty)
	{
		if (mTranslateTable.size() != table.size())
		{
			mDirty = true;
		}
		else
		{
			auto n = mTranslateTable.size();
			for (size_t i = 0; i < n; ++i)
			{
				if (mTranslateTable[i] != table[i])
				{
					mDirty = true;
					break;
				}
			}
		}
	}

	mTranslateTable = table;
}

void cLidarMapConfigScan::setRotateToGround(const std::optional<bool>& enable)
{
	mDirty |= (mRotateToGround != enable);
	mRotateToGround = enable;
}

void cLidarMapConfigScan::setRotateAngles_deg(const std::optional<double>& pitch_deg, const std::optional<double>& roll_deg)
{
	mDirty |= (mRotatePitch_deg != pitch_deg) || (mRotateRoll_deg != roll_deg);
	mRotatePitch_deg = pitch_deg;
	mRotateRoll_deg = roll_deg;
}

void cLidarMapConfigScan::setRotateThreshold_pct(const std::optional<double>& threshold_pct)
{
	mDirty |= (mRotateThreshold_pct != threshold_pct);
	mRotateThreshold_pct = threshold_pct;
}

void cLidarMapConfigScan::clearRotateTable()
{
	mDirty |= !mRotateTable.empty();
	mRotateTable.clear();
}

void cLidarMapConfigScan::setRotateTable(const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& table)
{
	if (!mDirty)
	{
		if (mRotateTable.size() != table.size())
		{
			mDirty = true;
		}
		else
		{
			auto n = mRotateTable.size();
			for (size_t i = 0; i < n; ++i)
			{
				if (mRotateTable[i] != table[i])
				{
					mDirty = true;
					break;
				}
			}
		}
	}

	mRotateTable = table;
}

void cLidarMapConfigScan::setStart_X_m(const std::optional<double>& x_m)
{
	mDirty |= (mStart_X_m != x_m);
	mStart_X_m = x_m;
}

void cLidarMapConfigScan::setStart_Y_m(const std::optional<double>& y_m)
{
	mDirty |= (mStart_Y_m != y_m);
	mStart_Y_m = y_m;
}

void cLidarMapConfigScan::setStart_Z_m(const std::optional<double>& z_m)
{
	mDirty |= (mStart_Z_m != z_m);
	mStart_Z_m = z_m;
}

void cLidarMapConfigScan::setStart_X_mm(const std::optional<double>& x_mm)
{
	std::optional<double> x_m = x_mm;

	if (x_m.has_value())
		x_m = x_mm.value() * nConstants::MM_TO_M;

	setStart_X_m(x_m);
}

void cLidarMapConfigScan::setStart_Y_mm(const std::optional<double>& y_mm)
{
	std::optional<double> y_m = y_mm;

	if (y_m.has_value())
		y_m = y_mm.value() * nConstants::MM_TO_M;

	setStart_Y_m(y_m);
}

void cLidarMapConfigScan::setStart_Z_mm(const std::optional<double>& z_mm)
{
	std::optional<double> z_m = z_mm;

	if (z_m.has_value())
		z_m = z_mm.value() * nConstants::MM_TO_M;

	setStart_Z_m(z_m);
}

void cLidarMapConfigScan::setEnd_X_m(const std::optional<double>& x_m)
{
	mDirty |= (mEnd_X_m != x_m);
	mEnd_X_m = x_m;
}

void cLidarMapConfigScan::setEnd_Y_m(const std::optional<double>& y_m)
{
	mDirty |= (mEnd_Y_m != y_m);
	mEnd_Y_m = y_m;
}

void cLidarMapConfigScan::setEnd_Z_m(const std::optional<double>& z_m)
{
	mDirty |= (mEnd_Z_m != z_m);
	mEnd_Z_m = z_m;
}

void cLidarMapConfigScan::setEnd_X_mm(const std::optional<double>& x_mm)
{
	std::optional<double> x_m = x_mm;

	if (x_m.has_value())
		x_m = x_mm.value() * nConstants::MM_TO_M;

	setEnd_X_m(x_m);
}

void cLidarMapConfigScan::setEnd_Y_mm(const std::optional<double>& y_mm)
{
	std::optional<double> y_m = y_mm;

	if (y_m.has_value())
		y_m = y_mm.value() * nConstants::MM_TO_M;

	setEnd_Y_m(y_m);
}

void cLidarMapConfigScan::setEnd_Z_mm(const std::optional<double>& z_mm)
{
	std::optional<double> z_m = z_mm;

	if (z_m.has_value())
		z_m = z_mm.value() * nConstants::MM_TO_M;

	setEnd_Z_m(z_m);
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

void cLidarMapConfigScan::setDirty(bool dirty)
{
	mDirty = dirty;
}


void cLidarMapConfigScan::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("experiment_name"))
		mExperimentName = jdoc["experiment_name"];

	if (jdoc.contains("experiment name"))
		mExperimentName = jdoc["experiment name"];


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


	if (jdoc.contains("start position"))
	{
		auto pos = jdoc["start position"];

		if (pos.contains("x (m)"))
			mStart_X_m = pos["x (m)"];

		if (pos.contains("y (m)"))
			mStart_Y_m = pos["y (m)"];

		if (pos.contains("z (m)"))
			mStart_Z_m = pos["z (m)"];
	}

	if (jdoc.contains("end position"))
	{
		auto pos = jdoc["end position"];

		if (pos.contains("x (m)"))
			mEnd_X_m = pos["x (m)"];

		if (pos.contains("y (m)"))
			mEnd_Y_m = pos["y (m)"];

		if (pos.contains("z (m)"))
			mEnd_Z_m = pos["z (m)"];
	}

	if (jdoc.contains("Vx (mm/s)"))
	{
		mVx_mmps = jdoc["Vx (mm/s)"];
		mKinematicModel = eKinematicModel::CONSTANT_SPEED;
	}

	if (jdoc.contains("Vy (mm/s)"))
	{
		mVy_mmps = jdoc["Vy (mm/s)"];
		mKinematicModel = eKinematicModel::CONSTANT_SPEED;
	}

	if (jdoc.contains("Vz (mm/s)"))
	{
		mVz_mmps = jdoc["Vz (mm/s)"];
		mKinematicModel = eKinematicModel::CONSTANT_SPEED;
	}

	if (jdoc.contains("sensor orientation table"))
	{
		mOrientationModel = eOrientationModel::INTREP_CURVE;

		auto points = jdoc["sensor orientation table"];

		if (points.empty())
		{
			kdt::sDollyOrientationInterpPoint_t point;

			mOrientationTable.push_back(point);
			point.distance_pct = 100.0;
			mOrientationTable.push_back(point);
		}
		else
		{
			for (const auto& point : points)
			{
				kdt::sDollyOrientationInterpPoint_t p;

				if (!point.contains("distance (%)"))
					continue;

				p.distance_pct = point["distance (%)"];

				if (point.contains("pitch (deg)"))
					p.pitch_deg = point["pitch (deg)"];

				if (point.contains("roll (deg)"))
					p.roll_deg = point["roll (deg)"];

				if (point.contains("yaw (deg)"))
					p.yaw_deg = point["yaw (deg)"];

				mOrientationTable.push_back(p);
			}

			std::sort(mOrientationTable.begin(), mOrientationTable.end(), [](const auto& a, const auto& b)
				{
					return a.distance_pct < b.distance_pct;
				});

			if (mOrientationTable.empty())
			{
				kdt::sDollyOrientationInterpPoint_t point;

				mOrientationTable.push_back(point);
				point.distance_pct = 100.0;
				mOrientationTable.push_back(point);
			}
			else
			{
				auto point = mOrientationTable.front();
				if (point.distance_pct > 0.0)
				{
					point.distance_pct = 0.0;
					mOrientationTable.insert(mOrientationTable.begin(), point);
				}

				point = mOrientationTable.back();
				if (point.distance_pct < 100.0)
				{
					point.distance_pct = 100.0;
					mOrientationTable.push_back(point);
				}
			}
		}
	}
	else if (jdoc.contains("sensor start orientation offset"))
	{
		mOrientationModel = eOrientationModel::LINEAR;

		auto orientation = jdoc["sensor start orientation offset"];

		if (orientation.contains("pitch (deg)"))
			mStartPitchOffset_deg = orientation["pitch (deg)"];

		if (orientation.contains("roll (deg)"))
			mStartRollOffset_deg = orientation["roll (deg)"];

		if (orientation.contains("yaw (deg)"))
			mStartYawOffset_deg = orientation["yaw (deg)"];

		if (jdoc.contains("sensor final orientation offset"))
		{
			auto orientation = jdoc["sensor final orientation offset"];

			if (orientation.contains("pitch (deg)"))
				mEndPitchOffset_deg = orientation["pitch (deg)"];

			if (orientation.contains("roll (deg)"))
				mEndRollOffset_deg = orientation["roll (deg)"];

			if (orientation.contains("yaw (deg)"))
				mEndYawOffset_deg = orientation["yaw (deg)"];
		}
	}
	else if (jdoc.contains("sensor orientation offset"))
	{
		mOrientationModel = eOrientationModel::CONSTANT;

		auto orientation = jdoc["sensor orientation offset"];

		if (orientation.contains("pitch (deg)"))
			mSensorPitchOffset_deg = orientation["pitch (deg)"];

		if (orientation.contains("roll (deg)"))
			mSensorRollOffset_deg = orientation["roll (deg)"];

		if (orientation.contains("yaw (deg)"))
			mSensorYawOffset_deg = orientation["yaw (deg)"];
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

	if (jdoc.contains("options"))
	{
		auto options = jdoc["options"];

		if (options.contains("translation distance (m)"))
		{
			mTranslateToGroundModel = eTranslateToGroundModel::CONSTANT;
			mTranslateDistance_m = options["translation distance (m)"].get<double>();
		}

		if (options.contains("translation threshold (%)"))
		{
			mTranslateToGroundModel = eTranslateToGroundModel::FIT;
			mTranslateThreshold_pct = options["translation threshold (%)"].get<double>();
		}

		if (options.contains("translation table"))
		{
			mTranslateToGroundModel = eTranslateToGroundModel::INTREP_CURVE;

			auto points = options["translation table"];

			if (points.empty())
			{
				pointcloud::sPointCloudTranslationInterpPoint_t point;

				mTranslateTable.push_back(point);
				point.displacement_m = 0.0;
				mTranslateTable.push_back(point);
			}
			else
			{
				for (const auto& point : points)
				{
					pointcloud::sPointCloudTranslationInterpPoint_t p;

					if (!point.contains("displacement (m)"))
						continue;

					p.displacement_m = point["displacement (m)"];

					if (point.contains("height (m)"))
						p.height_m = point["height (m)"];

					mTranslateTable.push_back(p);
				}

				std::sort(mTranslateTable.begin(), mTranslateTable.end(), [](const auto& a, const auto& b)
					{
						return a.displacement_m < b.displacement_m;
					});

				if (mTranslateTable.empty())
				{
					pointcloud::sPointCloudTranslationInterpPoint_t point;

					mTranslateTable.push_back(point);
					point.displacement_m = 0.0;
					mTranslateTable.push_back(point);
				}
				else
				{
					auto point = mTranslateTable.front();
					if (point.displacement_m > 0.0)
					{
						point.displacement_m = 0.0;
						mTranslateTable.insert(mTranslateTable.begin(), point);
					}
				}
			}
		}

		if (options.contains("rotation pitch (deg)") || options.contains("rotation roll (deg)"))
		{
			mRotateToGroundModel = eRotateToGroundModel::CONSTANT;

			if (options.contains("rotation pitch (deg)"))
				mRotatePitch_deg = options["rotation pitch (deg)"];

			if (options.contains("rotation roll (deg)"))
				mRotateRoll_deg = options["rotation roll (deg)"];
		}

		if (options.contains("rotation threshold (%)"))
		{
			mRotateToGroundModel = eRotateToGroundModel::FIT;
			mRotateThreshold_pct = options["rotation threshold (%)"].get<double>();
		}

		if (options.contains("rotation table"))
		{
			mRotateToGroundModel = eRotateToGroundModel::INTREP_CURVE;

			auto points = options["rotation table"];

			if (points.empty())
			{
				pointcloud::sPointCloudRotationInterpPoint_t point;

				mRotateTable.push_back(point);
				point.displacement_m = 0.0;
				mRotateTable.push_back(point);
			}
			else
			{
				for (const auto& point : points)
				{
					pointcloud::sPointCloudRotationInterpPoint_t p;

					if (!point.contains("displacement (m)"))
						continue;

					p.displacement_m = point["displacement (m)"];

					if (point.contains("pitch (deg)"))
						p.pitch_deg = point["pitch (deg)"];

					if (point.contains("roll (deg)"))
						p.roll_deg = point["roll (deg)"];

					mRotateTable.push_back(p);
				}

				std::sort(mRotateTable.begin(), mRotateTable.end(), [](const auto& a, const auto& b)
					{
						return a.displacement_m < b.displacement_m;
					});

				if (mRotateTable.empty())
				{
					pointcloud::sPointCloudRotationInterpPoint_t point;

					mRotateTable.push_back(point);
					point.displacement_m = 0.0;
					mRotateTable.push_back(point);
				}
				else
				{
					auto point = mRotateTable.front();
					if (point.displacement_m > 0.0)
					{
						point.displacement_m = 0.0;
						mRotateTable.insert(mRotateTable.begin(), point);
					}
				}
			}
		}
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
}

nlohmann::json cLidarMapConfigScan::save()
{
	nlohmann::json scanDoc;

	scanDoc["experiment name"] = mExperimentName;

	if (mStart_X_m.has_value() || mStart_Y_m.has_value() || mStart_Z_m.has_value())
	{
		nlohmann::json pos;

		if (mStart_X_m.has_value())
			pos["x (m)"] = mStart_X_m.value();
			
		if (mStart_Y_m.has_value())
			pos["y (m)"] = mStart_Y_m.value();

		if (mStart_Z_m.has_value())
			pos["z (m)"] = mStart_Z_m.value();

		scanDoc["start position"] = pos;
	}


	if (mEnd_X_m.has_value() || mEnd_Y_m.has_value() || mEnd_Z_m.has_value())
	{
		nlohmann::json pos;

		if (mEnd_X_m.has_value())
			pos["x (m)"] = mEnd_X_m.value();

		if (mEnd_Y_m.has_value())
			pos["y (m)"] = mEnd_Y_m.value();

		if (mEnd_Z_m.has_value())
			pos["z (m)"] = mEnd_Z_m.value();

		scanDoc["end position"] = pos;
	}

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

	if (mKinematicModel.has_value() && (mKinematicModel.value() == eKinematicModel::CONSTANT_SPEED))
	{
		if (mVx_mmps.has_value())
			scanDoc["Vx (mm/s)"] = mVx_mmps.value();

		if (mVy_mmps.has_value())
			scanDoc["Vy (mm/s)"] = mVy_mmps.value();

		if (mVz_mmps.has_value())
			scanDoc["Vz (mm/s)"] = mVz_mmps.value();
	}

	if (mOrientationModel.has_value())
	{
		if (mOrientationModel == eOrientationModel::CONSTANT)
		{
			if (mSensorPitchOffset_deg.has_value() || mSensorRollOffset_deg.has_value() || mSensorYawOffset_deg.has_value())
			{
				nlohmann::json orientation;

				if (mSensorPitchOffset_deg.has_value())
					orientation["pitch (deg)"] = mSensorPitchOffset_deg.value();

				if (mSensorRollOffset_deg.has_value())
					orientation["roll (deg)"] = mSensorRollOffset_deg.value();

				if (mSensorYawOffset_deg.has_value())
					orientation["yaw (deg)"] = mSensorYawOffset_deg.value();

				scanDoc["sensor orientation offset"] = orientation;
			}
		}
		else if (mOrientationModel == eOrientationModel::LINEAR)
		{
			if (mStartPitchOffset_deg.has_value() || mStartRollOffset_deg.has_value() || mStartYawOffset_deg.has_value())
			{
				nlohmann::json orientation;

				if (mStartPitchOffset_deg.has_value())
					orientation["pitch (deg)"] = mStartPitchOffset_deg.value();

				if (mStartRollOffset_deg.has_value())
					orientation["roll (deg)"] = mStartRollOffset_deg.value();

				if (mStartYawOffset_deg.has_value())
					orientation["yaw (deg)"] = mStartYawOffset_deg.value();

				scanDoc["sensor start orientation offset"] = orientation;
			}

			if (mEndPitchOffset_deg.has_value() || mEndRollOffset_deg.has_value() || mEndYawOffset_deg.has_value())
			{
				nlohmann::json orientation;

				if (mEndPitchOffset_deg.has_value())
					orientation["pitch (deg)"] = mEndPitchOffset_deg.value();

				if (mEndRollOffset_deg.has_value())
					orientation["roll (deg)"] = mEndRollOffset_deg.value();

				if (mEndYawOffset_deg.has_value())
					orientation["yaw (deg)"] = mEndYawOffset_deg.value();

				scanDoc["sensor final orientation offset"] = orientation;
			}
		}
		else if (mOrientationModel == eOrientationModel::INTREP_CURVE)
		{
			nlohmann::json points;

			for (const auto& point : mOrientationTable)
			{
				nlohmann::json orientation;

				orientation["distance (%)"] = point.distance_pct;
				orientation["pitch (deg)"] = point.pitch_deg;
				orientation["roll (deg)"] = point.roll_deg;
				orientation["yaw (deg)"] = point.yaw_deg;

				points.push_back(orientation);
			}

			scanDoc["sensor orientation table"] = points;
		}
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

	if (mTranslateToGroundModel.has_value() || mRotateToGroundModel.has_value())
	{
		nlohmann::json options;

		if (mTranslateToGroundModel.has_value())
		{
			auto model = mTranslateToGroundModel.value();

			switch (model)
			{
			case eTranslateToGroundModel::NONE:
				break;
			case eTranslateToGroundModel::CONSTANT:
				if (mTranslateDistance_m.has_value())
					options["translation distance (m)"] = mTranslateDistance_m.value();
				break;
			case eTranslateToGroundModel::FIT:
				if (mTranslateThreshold_pct.has_value())
					options["translation threshold (%)"] = mTranslateThreshold_pct.value();
				break;
			case eTranslateToGroundModel::INTREP_CURVE:

				nlohmann::json points;

				for (const auto& point : mTranslateTable)
				{
					nlohmann::json table;

					table["displacement (m)"] = point.displacement_m;
					table["height (m)"]		  = point.height_m;

					points.push_back(table);
				}

				options["translation table"] = points;
				break;
			}
		}

		if (mRotateToGroundModel.has_value())
		{
			auto model = mRotateToGroundModel.value();

			switch (model)
			{
			case eRotateToGroundModel::NONE:
				break;
			case eRotateToGroundModel::CONSTANT:
				if (mRotatePitch_deg.has_value())
					options["rotation pitch (deg)"] = mRotatePitch_deg.value();
				if (mRotateRoll_deg.has_value())
					options["rotation roll (deg)"] = mRotateRoll_deg.value();
				break;
			case eRotateToGroundModel::FIT:
				if (mRotateThreshold_pct.has_value())
					options["rotation threshold (%)"] = mRotateThreshold_pct.value();
				break;
			case eRotateToGroundModel::INTREP_CURVE:

				nlohmann::json points;

				for (const auto& point : mRotateTable)
				{
					nlohmann::json table;

					table["displacement (m)"] = point.displacement_m;
					table["pitch (deg)"] = point.pitch_deg;
					table["roll (deg)"] = point.roll_deg;

					points.push_back(table);
				}

				options["rotation table"] = points;
				break;
			}
		}

		scanDoc["options"] = options;
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

	mDirty = false;

	return scanDoc;
}


