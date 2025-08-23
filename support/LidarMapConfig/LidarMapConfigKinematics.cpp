
#include "LidarMapConfigKinematics.hpp"

#include "StringUtils.hpp"
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

	bool operator!=(const std::optional<rfm::rappPoint_t>& lhs, const std::optional<rfm::rappPoint_t>& rhs)
	{
		if (!lhs.has_value() && !rhs.has_value())
			return false;

		if (lhs.has_value() != rhs.has_value())
			return true;

		return static_cast<rfm::rappPoint_t>(lhs.value()) != static_cast<rfm::rappPoint_t>(rhs.value());
	}
}

using namespace lidar_config;


/********************************************************************
 * Lidar Map Config Kinematic Parameters
********************************************************************/

cLidarMapConfigKinematicParameters::cLidarMapConfigKinematicParameters()
	: mEffectiveMonth(0), mEffectiveDay(0)
{
}

cLidarMapConfigKinematicParameters::cLidarMapConfigKinematicParameters(int month, int day)
	: mEffectiveMonth(month), mEffectiveDay(day)
{
}

cLidarMapConfigKinematicParameters& cLidarMapConfigKinematicParameters::operator=(const cLidarMapConfigKinematicParameters& other)
{
	mDirty = other.mDirty;

	mEffectiveMonth = other.mEffectiveMonth;
	mEffectiveDay = other.mEffectiveDay;

	setKinematicModel(other.mKinematicModel);
    setOrientationModel(other.mOrientationModel);
	setTranslateToGroundModel(other.mTranslateToGroundModel);
	setRotateToGroundModel(other.mRotateToGroundModel);

	setVx_mmps(other.mVx_mmps);
	setVy_mmps(other.mVy_mmps);
	setVz_mmps(other.mVz_mmps);

	setSensorPitchOffset_deg(other.mSensorPitchOffset_deg);
	setSensorRollOffset_deg(other.mSensorRollOffset_deg);
	setSensorYawOffset_deg(other.mSensorYawOffset_deg);

	setStartPitchOffset_deg(other.mStartPitchOffset_deg);
	setStartRollOffset_deg(other.mStartRollOffset_deg);
	setStartYawOffset_deg(other.mStartYawOffset_deg);

	setEndPitchOffset_deg(other.mEndPitchOffset_deg);
	setEndRollOffset_deg(other.mEndRollOffset_deg);
	setEndYawOffset_deg(other.mEndYawOffset_deg);

	setOrientationTable(other.mOrientationTable);

	setTranslateToGround(other.mTranslateToGround);
	setTranslateDistance_m(other.mTranslateDistance_m);
	setTranslateThreshold_pct(other.mTranslateThreshold_pct);

	setTranslateTable(other.mTranslateTable);

	setRotateToGround(other.mRotateToGround);
	setRotateAngles_deg(other.mRotatePitch_deg, other.mRotateRoll_deg);
	setRotateThreshold_pct(other.mRotateThreshold_pct);

	setRotateTable(other.mRotateTable);

	setStart_X_m(other.mStart_X_m);
	setStart_Y_m(other.mStart_Y_m);
	setStart_Z_m(other.mStart_Z_m);

	setEnd_X_m(other.mEnd_X_m);
	setEnd_Y_m(other.mEnd_Y_m);
	setEnd_Z_m(other.mEnd_Z_m);

	return *this;
}

bool cLidarMapConfigKinematicParameters::operator==(const cLidarMapConfigKinematicParameters& other) const
{
	bool equal = mEffectiveMonth == other.mEffectiveMonth;
	equal &= mEffectiveDay == other.mEffectiveDay;

	equal &= mKinematicModel == other.mKinematicModel;
	equal &= mOrientationModel == other.mOrientationModel;
	equal &= mTranslateToGroundModel == other.mTranslateToGroundModel;
	equal &= mRotateToGroundModel == other.mRotateToGroundModel;

	equal &= mVx_mmps == other.mVx_mmps;
	equal &= mVy_mmps == other.mVy_mmps;
	equal &= mVz_mmps == other.mVz_mmps;

	equal &= mSensorPitchOffset_deg == other.mSensorPitchOffset_deg;
	equal &= mSensorRollOffset_deg == other.mSensorRollOffset_deg;
	equal &= mSensorYawOffset_deg == other.mSensorYawOffset_deg;

	equal &= mStartPitchOffset_deg == other.mStartPitchOffset_deg;
	equal &= mStartRollOffset_deg == other.mStartRollOffset_deg;
	equal &= mStartYawOffset_deg == other.mStartYawOffset_deg;

	equal &= mEndPitchOffset_deg == other.mEndPitchOffset_deg;
	equal &= mEndRollOffset_deg == other.mEndRollOffset_deg;
	equal &= mEndYawOffset_deg == other.mEndYawOffset_deg;

	equal &= mOrientationTable == other.mOrientationTable;
	equal &= mTranslateToGround == other.mTranslateToGround;
	equal &= mTranslateDistance_m == other.mTranslateDistance_m;
	equal &= mTranslateThreshold_pct == other.mTranslateThreshold_pct;
	equal &= mTranslateTable == other.mTranslateTable;
	equal &= mRotateToGround == other.mRotateToGround;
	equal &= mRotatePitch_deg == other.mRotatePitch_deg;
	equal &= mRotateRoll_deg == other.mRotateRoll_deg;
	equal &= mRotateThreshold_pct == other.mRotateThreshold_pct;
	equal &= mRotateTable == other.mRotateTable;
	equal &= mStart_X_m == other.mStart_X_m;
	equal &= mStart_Y_m == other.mStart_Y_m;
	equal &= mStart_Z_m == other.mStart_Z_m;
	equal &= mEnd_X_m == other.mEnd_X_m;
	equal &= mEnd_Y_m == other.mEnd_Y_m;
	equal &= mEnd_Z_m == other.mEnd_Z_m;

	return equal;
}

bool cLidarMapConfigKinematicParameters::operator!=(const cLidarMapConfigKinematicParameters& other) const
{
	return !operator==(other);
}


const int cLidarMapConfigKinematicParameters::date() const { return to_date(mEffectiveMonth, mEffectiveDay); }
const int cLidarMapConfigKinematicParameters::month() const { return mEffectiveMonth; }
const int cLidarMapConfigKinematicParameters::day() const { return mEffectiveDay; }

void cLidarMapConfigKinematicParameters::clear()
{
	mDirty = false;

	mKinematicModel.reset();
	mOrientationModel.reset();
	mTranslateToGroundModel.reset();
	mRotateToGroundModel.reset();

	mVx_mmps.reset();
	mVy_mmps.reset();
	mVz_mmps.reset();

	mSensorPitchOffset_deg.reset();
	mSensorRollOffset_deg.reset();
	mSensorYawOffset_deg.reset();

	mStartPitchOffset_deg.reset();
	mStartRollOffset_deg.reset();
	mStartYawOffset_deg.reset();

	mEndPitchOffset_deg.reset();
	mEndRollOffset_deg.reset();
	mEndYawOffset_deg.reset();

	mOrientationTable.clear();

	mTranslateToGround.reset();
	mTranslateDistance_m.reset();
	mTranslateThreshold_pct.reset();

	mTranslateTable.clear();

	mRotateToGround.reset();
	mRotatePitch_deg.reset();
	mRotateRoll_deg.reset();
	mRotateThreshold_pct.reset();

	mRotateTable.clear();

	mStart_X_m.reset();
	mStart_Y_m.reset();
	mStart_Z_m.reset();

	mEnd_X_m.reset();
	mEnd_Y_m.reset();
	mEnd_Z_m.reset();
}

bool cLidarMapConfigKinematicParameters::empty() const
{
	bool has_value = mKinematicModel.has_value();
	has_value |= mOrientationModel.has_value();
	has_value |= mTranslateToGroundModel.has_value();
	has_value |= mRotateToGroundModel.has_value();

	has_value |= mVx_mmps.has_value();
	has_value |= mVy_mmps.has_value();
	has_value |= mVz_mmps.has_value();

	has_value |= mSensorPitchOffset_deg.has_value();
	has_value |= mSensorRollOffset_deg.has_value();
	has_value |= mSensorYawOffset_deg.has_value();

	has_value |= mStartPitchOffset_deg.has_value();
	has_value |= mStartRollOffset_deg.has_value();
	has_value |= mStartYawOffset_deg.has_value();

	has_value |= mEndPitchOffset_deg.has_value();
	has_value |= mEndRollOffset_deg.has_value();
	has_value |= mEndYawOffset_deg.has_value();

	has_value |= mTranslateToGround.has_value();
	has_value |= mTranslateDistance_m.has_value();
	has_value |= mTranslateThreshold_pct.has_value();

	has_value |= mRotateToGround.has_value();
	has_value |= mRotatePitch_deg.has_value();
	has_value |= mRotateRoll_deg.has_value();
	has_value |= mRotateThreshold_pct.has_value();

	has_value |= mStart_X_m.has_value();
	has_value |= mStart_Y_m.has_value();
	has_value |= mStart_Z_m.has_value();

	has_value |= mEnd_X_m.has_value();
	has_value |= mEnd_Y_m.has_value();
	has_value |= mEnd_Z_m.has_value();

	return !has_value && mRotateTable.empty() && mTranslateTable.empty() && mOrientationTable.empty();
}


bool cLidarMapConfigKinematicParameters::isDirty() const
{
	return mDirty;
}

const std::optional<eKinematicModel>& cLidarMapConfigKinematicParameters::getKinematicModel() const { return mKinematicModel; }
const std::optional<eOrientationModel>& cLidarMapConfigKinematicParameters::getOrientationModel() const { return mOrientationModel; }
const std::optional<eTranslateToGroundModel>& cLidarMapConfigKinematicParameters::getTranslateToGroundModel() const { return mTranslateToGroundModel; }
const std::optional<eRotateToGroundModel>& cLidarMapConfigKinematicParameters::getRotateToGroundModel() const { return mRotateToGroundModel; }

const std::optional<double>& cLidarMapConfigKinematicParameters::getVx_mmps() const { return mVx_mmps; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getVy_mmps() const { return mVy_mmps; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getVz_mmps() const { return mVz_mmps; }

const std::optional<double>& cLidarMapConfigKinematicParameters::getSensorPitchOffset_deg() const { return mSensorPitchOffset_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getSensorRollOffset_deg() const { return mSensorRollOffset_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getSensorYawOffset_deg() const { return mSensorYawOffset_deg; }

const std::optional<double>& cLidarMapConfigKinematicParameters::getStartPitchOffset_deg() const { return mStartPitchOffset_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getStartRollOffset_deg() const { return mStartRollOffset_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getStartYawOffset_deg() const { return mStartYawOffset_deg; }

const std::optional<double>& cLidarMapConfigKinematicParameters::getEndPitchOffset_deg() const { return mEndPitchOffset_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getEndRollOffset_deg() const { return mEndRollOffset_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getEndYawOffset_deg() const { return mEndYawOffset_deg; }

const std::vector<kdt::sDollyOrientationInterpPoint_t>& cLidarMapConfigKinematicParameters::getOrientationTable() const { return mOrientationTable; }

const std::optional<bool>& cLidarMapConfigKinematicParameters::getTranslateToGround() const { return mTranslateToGround; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getTranslateDistance_m() const { return mTranslateDistance_m; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getTranslateThreshold_pct() const { return mTranslateThreshold_pct; }

const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& cLidarMapConfigKinematicParameters::getTranslateTable() const { return mTranslateTable; }

const std::optional<bool>& cLidarMapConfigKinematicParameters::getRotateToGround() const { return mRotateToGround; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getRotatePitch_deg() const { return mRotatePitch_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getRotateRoll_deg() const { return mRotateRoll_deg; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getRotateThreshold_pct() const { return mRotateThreshold_pct; }

const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& cLidarMapConfigKinematicParameters::getRotateTable() const { return mRotateTable; }

const std::optional<double>& cLidarMapConfigKinematicParameters::getStart_X_m() const { return mStart_X_m; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getStart_Y_m() const { return mStart_Y_m; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getStart_Z_m() const { return mStart_Z_m; }

const std::optional<double>& cLidarMapConfigKinematicParameters::getEnd_X_m() const { return mEnd_X_m; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getEnd_Y_m() const { return mEnd_Y_m; }
const std::optional<double>& cLidarMapConfigKinematicParameters::getEnd_Z_m() const { return mEnd_Z_m; }

void cLidarMapConfigKinematicParameters::resetKinematicModel()
{
	reset(mKinematicModel);
}

void cLidarMapConfigKinematicParameters::setKinematicModel(const std::optional<eKinematicModel>& model)
{
	mDirty |= (mKinematicModel != model);
	mKinematicModel = model;
}

void cLidarMapConfigKinematicParameters::resetOrientationModel()
{
	reset(mOrientationModel);
}

void cLidarMapConfigKinematicParameters::setOrientationModel(const std::optional<eOrientationModel>& model)
{
	mDirty |= (mOrientationModel != model);
	mOrientationModel = model;
}

void cLidarMapConfigKinematicParameters::resetTranslateToGroundModel()
{
	reset(mTranslateToGroundModel);
}

void cLidarMapConfigKinematicParameters::setTranslateToGroundModel(const std::optional<eTranslateToGroundModel>& model)
{
	mDirty |= (mTranslateToGroundModel != model);
	mTranslateToGroundModel = model;
}

void cLidarMapConfigKinematicParameters::resetRotateToGroundModel()
{
	reset(mRotateToGroundModel);
}

void cLidarMapConfigKinematicParameters::setRotateToGroundModel(const std::optional<eRotateToGroundModel>& model)
{
	mDirty |= (mRotateToGroundModel != model);
	mRotateToGroundModel = model;
}

void cLidarMapConfigKinematicParameters::resetSpeeds()
{
	setVx_mmps(std::optional<double>());
	setVy_mmps(std::optional<double>());
	setVz_mmps(std::optional<double>());
}

void cLidarMapConfigKinematicParameters::setVx_mmps(const std::optional<double>& vx_mmps)
{
	mDirty |= (mVx_mmps != vx_mmps);
	mVx_mmps = vx_mmps;
}

void cLidarMapConfigKinematicParameters::setVy_mmps(const std::optional<double>& vy_mmps)
{
	mDirty |= (mVy_mmps != vy_mmps);
	mVy_mmps = vy_mmps;
}

void cLidarMapConfigKinematicParameters::setVz_mmps(const std::optional<double>& vz_mmps)
{
	mDirty |= (mVz_mmps != vz_mmps);
	mVz_mmps = vz_mmps;
}

void cLidarMapConfigKinematicParameters::setSensorPitchOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mSensorPitchOffset_deg != offset_deg);
	mSensorPitchOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setSensorRollOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mSensorRollOffset_deg != offset_deg);
	mSensorRollOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setSensorYawOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mSensorYawOffset_deg != offset_deg);
	mSensorYawOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setStartPitchOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mStartPitchOffset_deg != offset_deg);
	mStartPitchOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setStartRollOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mStartRollOffset_deg != offset_deg);
	mStartRollOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setStartYawOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mStartYawOffset_deg != offset_deg);
	mStartYawOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setEndPitchOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mEndPitchOffset_deg != offset_deg);
	mEndPitchOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setEndRollOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mEndRollOffset_deg != offset_deg);
	mEndRollOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::setEndYawOffset_deg(const std::optional<double>& offset_deg)
{
	mDirty |= (mEndYawOffset_deg != offset_deg);
	mEndYawOffset_deg = offset_deg;
}

void cLidarMapConfigKinematicParameters::clearOrientationTable()
{
	mDirty |= !mOrientationTable.empty();
	mOrientationTable.clear();
}

void cLidarMapConfigKinematicParameters::setOrientationTable(const std::vector<kdt::sDollyOrientationInterpPoint_t>& table)
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

void cLidarMapConfigKinematicParameters::setTranslateToGround(const std::optional<bool>& enable)
{
	mDirty |= (mTranslateToGround != enable);
	mTranslateToGround = enable;
}

void cLidarMapConfigKinematicParameters::setTranslateDistance_m(const std::optional<double>& dist_m)
{
	mDirty |= (mTranslateDistance_m != dist_m);
	mTranslateDistance_m = dist_m;
}

void cLidarMapConfigKinematicParameters::setTranslateThreshold_pct(const std::optional<double>& threshold_pct)
{
	mDirty |= (mTranslateThreshold_pct != threshold_pct);
	mTranslateThreshold_pct = threshold_pct;
}

void cLidarMapConfigKinematicParameters::clearTranslateTable()
{
	mDirty |= !mTranslateTable.empty();
	mTranslateTable.clear();
}

void cLidarMapConfigKinematicParameters::setTranslateTable(const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& table)
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

void cLidarMapConfigKinematicParameters::setRotateToGround(const std::optional<bool>& enable)
{
	mDirty |= (mRotateToGround != enable);
	mRotateToGround = enable;
}

void cLidarMapConfigKinematicParameters::setRotateAngles_deg(const std::optional<double>& pitch_deg, const std::optional<double>& roll_deg)
{
	mDirty |= (mRotatePitch_deg != pitch_deg) || (mRotateRoll_deg != roll_deg);
	mRotatePitch_deg = pitch_deg;
	mRotateRoll_deg = roll_deg;
}

void cLidarMapConfigKinematicParameters::setRotateThreshold_pct(const std::optional<double>& threshold_pct)
{
	mDirty |= (mRotateThreshold_pct != threshold_pct);
	mRotateThreshold_pct = threshold_pct;
}

void cLidarMapConfigKinematicParameters::clearRotateTable()
{
	mDirty |= !mRotateTable.empty();
	mRotateTable.clear();
}

void cLidarMapConfigKinematicParameters::setRotateTable(const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& table)
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

void cLidarMapConfigKinematicParameters::resetStartPosition()
{
	setStart_X_m(std::optional<double>());
	setStart_Y_m(std::optional<double>());
	setStart_Z_m(std::optional<double>());
}

void cLidarMapConfigKinematicParameters::setStart_X_m(const std::optional<double>& x_m)
{
	mDirty |= (mStart_X_m != x_m);
	mStart_X_m = x_m;
}

void cLidarMapConfigKinematicParameters::setStart_Y_m(const std::optional<double>& y_m)
{
	mDirty |= (mStart_Y_m != y_m);
	mStart_Y_m = y_m;
}

void cLidarMapConfigKinematicParameters::setStart_Z_m(const std::optional<double>& z_m)
{
	mDirty |= (mStart_Z_m != z_m);
	mStart_Z_m = z_m;
}

void cLidarMapConfigKinematicParameters::setStart_X_mm(const std::optional<double>& x_mm)
{
	std::optional<double> x_m = x_mm;

	if (x_m.has_value())
		x_m = x_mm.value() * nConstants::MM_TO_M;

	setStart_X_m(x_m);
}

void cLidarMapConfigKinematicParameters::setStart_Y_mm(const std::optional<double>& y_mm)
{
	std::optional<double> y_m = y_mm;

	if (y_m.has_value())
		y_m = y_mm.value() * nConstants::MM_TO_M;

	setStart_Y_m(y_m);
}

void cLidarMapConfigKinematicParameters::setStart_Z_mm(const std::optional<double>& z_mm)
{
	std::optional<double> z_m = z_mm;

	if (z_m.has_value())
		z_m = z_mm.value() * nConstants::MM_TO_M;

	setStart_Z_m(z_m);
}

void cLidarMapConfigKinematicParameters::resetEndPosition()
{
	setEnd_X_m(std::optional<double>());
	setEnd_Y_m(std::optional<double>());
	setEnd_Z_m(std::optional<double>());
}

void cLidarMapConfigKinematicParameters::setEnd_X_m(const std::optional<double>& x_m)
{
	mDirty |= (mEnd_X_m != x_m);
	mEnd_X_m = x_m;
}

void cLidarMapConfigKinematicParameters::setEnd_Y_m(const std::optional<double>& y_m)
{
	mDirty |= (mEnd_Y_m != y_m);
	mEnd_Y_m = y_m;
}

void cLidarMapConfigKinematicParameters::setEnd_Z_m(const std::optional<double>& z_m)
{
	mDirty |= (mEnd_Z_m != z_m);
	mEnd_Z_m = z_m;
}

void cLidarMapConfigKinematicParameters::setEnd_X_mm(const std::optional<double>& x_mm)
{
	std::optional<double> x_m = x_mm;

	if (x_m.has_value())
		x_m = x_mm.value() * nConstants::MM_TO_M;

	setEnd_X_m(x_m);
}

void cLidarMapConfigKinematicParameters::setEnd_Y_mm(const std::optional<double>& y_mm)
{
	std::optional<double> y_m = y_mm;

	if (y_m.has_value())
		y_m = y_mm.value() * nConstants::MM_TO_M;

	setEnd_Y_m(y_m);
}

void cLidarMapConfigKinematicParameters::setEnd_Z_mm(const std::optional<double>& z_mm)
{
	std::optional<double> z_m = z_mm;

	if (z_m.has_value())
		z_m = z_mm.value() * nConstants::MM_TO_M;

	setEnd_Z_m(z_m);
}

void cLidarMapConfigKinematicParameters::setDate(int month, int day)
{
	mDirty |= (mEffectiveMonth != month);
	mEffectiveMonth = month;

	mDirty |= (mEffectiveDay != day);
	mEffectiveDay = day;
}

void cLidarMapConfigKinematicParameters::clearDirtyFlag()
{
	setDirty(false);
}

void cLidarMapConfigKinematicParameters::setDirtyFlag()
{
	setDirty(true);
}

void cLidarMapConfigKinematicParameters::setDirty(bool dirty)
{
	mDirty = dirty;
}


void cLidarMapConfigKinematicParameters::load(const nlohmann::json& jdoc)
{
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
}

nlohmann::json cLidarMapConfigKinematicParameters::save()
{
	nlohmann::json scanDoc;

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
					table["height (m)"] = point.height_m;

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

	mDirty = false;

	if (scanDoc.is_null())
		return scanDoc;

	std::string date = std::to_string(mEffectiveMonth);
	date += "/";
	date += std::to_string(mEffectiveDay);

	scanDoc["date"] = date;

	return scanDoc;
}



/********************************************************************
 * Lidar Map Config Kinematics
********************************************************************/

cLidarMapConfigKinematics::cLidarMapConfigKinematics()
{}

cLidarMapConfigKinematics::~cLidarMapConfigKinematics()
{}

void cLidarMapConfigKinematics::clear()
{
	mKinematics.clear();
}

std::size_t cLidarMapConfigKinematics::size() const
{
	return mKinematics.size();
}

bool cLidarMapConfigKinematics::empty() const
{
	return mKinematics.empty();
}

bool cLidarMapConfigKinematics::isDirty() const
{
	for (const auto& kinematic : mKinematics)
	{
		if (kinematic.second.isDirty())
			return true;
	}

	return mDirty;
}


bool cLidarMapConfigKinematics::contains(const int date) const
{
	return mKinematics.contains(date);
}

bool cLidarMapConfigKinematics::contains(const int month, const int day) const
{
	return contains(to_date(month, day));
}

const cLidarMapConfigKinematicParameters& cLidarMapConfigKinematics::front() const { return mKinematics.begin()->second; }
cLidarMapConfigKinematicParameters& cLidarMapConfigKinematics::front() { return mKinematics.begin()->second; }

cLidarMapConfigKinematics::iterator	cLidarMapConfigKinematics::begin() { return mKinematics.begin(); }
cLidarMapConfigKinematics::iterator	cLidarMapConfigKinematics::end() { return mKinematics.end(); }

cLidarMapConfigKinematics::const_iterator cLidarMapConfigKinematics::begin() const { return mKinematics.begin(); }
cLidarMapConfigKinematics::const_iterator cLidarMapConfigKinematics::end() const { return mKinematics.end(); }

cLidarMapConfigKinematics::const_iterator cLidarMapConfigKinematics::find(const int date) const
{
	if (mKinematics.empty())
		return mKinematics.end();

	if (date < mKinematics.begin()->first)
		return mKinematics.begin();

	KinematicParameters_t::const_iterator result = mKinematics.end();

	for (auto it = mKinematics.begin(); it != mKinematics.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cLidarMapConfigKinematics::iterator	cLidarMapConfigKinematics::find(const int date)
{
	if (mKinematics.empty())
		return mKinematics.end();

	if (date < mKinematics.begin()->first)
		return mKinematics.begin();

	KinematicParameters_t::iterator result = mKinematics.end();

	for (auto it = mKinematics.begin(); it != mKinematics.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cLidarMapConfigKinematics::const_iterator cLidarMapConfigKinematics::find(const int month, const int day) const { return find(to_date(month, day)); }
cLidarMapConfigKinematics::iterator	cLidarMapConfigKinematics::find(const int month, const int day) { return find(to_date(month, day)); }

cLidarMapConfigKinematics::const_iterator cLidarMapConfigKinematics::find_exact(const int month, const int day) const
{
	int date = to_date(month, day);

	return mKinematics.find(date);
}

cLidarMapConfigKinematics::iterator	cLidarMapConfigKinematics::find_exact(const int month, const int day)
{
	int date = to_date(month, day);

	return mKinematics.find(date);
}


const std::optional<eKinematicModel>& cLidarMapConfigKinematics::getKinematicModel(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getKinematicModel();
}

const std::optional<eKinematicModel>& cLidarMapConfigKinematics::getKinematicModel(int month, int day) const { return getKinematicModel(to_date(month, day)); }

const std::optional<eOrientationModel>& cLidarMapConfigKinematics::getOrientationModel(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getOrientationModel();
}

const std::optional<eOrientationModel>& cLidarMapConfigKinematics::getOrientationModel(int month, int day) const { return getOrientationModel(to_date(month, day)); }


const std::optional<eTranslateToGroundModel>& cLidarMapConfigKinematics::getTranslateToGroundModel(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getTranslateToGroundModel();
}

const std::optional<eTranslateToGroundModel>& cLidarMapConfigKinematics::getTranslateToGroundModel(int month, int day) const { return getTranslateToGroundModel(to_date(month, day)); }


const std::optional<eRotateToGroundModel>& cLidarMapConfigKinematics::getRotateToGroundModel(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getRotateToGroundModel();
}

const std::optional<eRotateToGroundModel>& cLidarMapConfigKinematics::getRotateToGroundModel(int month, int day) const { return getRotateToGroundModel(to_date(month, day)); }


/*** Sensor Orientation Parameters - Constant: Angles ***/
const std::optional<double>& cLidarMapConfigKinematics::getSensorPitchOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getSensorPitchOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getSensorRollOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getSensorRollOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getSensorYawOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getSensorYawOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getSensorPitchOffset_deg(int month, int day) const { return getSensorPitchOffset_deg(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getSensorRollOffset_deg(int month, int day) const { return getSensorRollOffset_deg(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getSensorYawOffset_deg(int month, int day) const { return getSensorYawOffset_deg(to_date(month, day)); }

/*** Sensor Orientation Parameters - Linear: Start and Stop Angles ***/
const std::optional<double>& cLidarMapConfigKinematics::getStartPitchOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getStartPitchOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getStartRollOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getStartRollOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getStartYawOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getStartYawOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getEndPitchOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getEndPitchOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getEndRollOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getEndRollOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getEndYawOffset_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getEndYawOffset_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getStartPitchOffset_deg(int month, int day) const { return getStartPitchOffset_deg(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getStartRollOffset_deg(int month, int day) const { return getStartRollOffset_deg(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getStartYawOffset_deg(int month, int day) const { return getStartYawOffset_deg(to_date(month, day)); }

const std::optional<double>& cLidarMapConfigKinematics::getEndPitchOffset_deg(int month, int day) const { return getEndPitchOffset_deg(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getEndRollOffset_deg(int month, int day) const { return getEndRollOffset_deg(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getEndYawOffset_deg(int month, int day) const { return getEndYawOffset_deg(to_date(month, day)); }

/*** Sensor Orientation Parameters - Intrep Table: Data Points ***/
const std::vector<kdt::sDollyOrientationInterpPoint_t>& cLidarMapConfigKinematics::getOrientationTable(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getOrientationTable();
}

const std::vector<kdt::sDollyOrientationInterpPoint_t>& cLidarMapConfigKinematics::getOrientationTable(int month, int day) const { return getOrientationTable(to_date(month, day)); }

/*** Sensor Translation Parameters - Start/End Positions ***/
const std::optional<double>& cLidarMapConfigKinematics::getStart_X_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getStart_X_m();
}

const std::optional<double>& cLidarMapConfigKinematics::getStart_Y_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getStart_Y_m();
}

const std::optional<double>& cLidarMapConfigKinematics::getStart_Z_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getStart_Z_m();
}

const std::optional<double>& cLidarMapConfigKinematics::getEnd_X_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getEnd_X_m();
}

const std::optional<double>& cLidarMapConfigKinematics::getEnd_Y_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getEnd_Y_m();
}

const std::optional<double>& cLidarMapConfigKinematics::getEnd_Z_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getEnd_Z_m();
}

/*** Sensor Translation Parameters - Constant: Speeds ***/
const std::optional<double>& cLidarMapConfigKinematics::getVx_mmps(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getVx_mmps();
}

const std::optional<double>& cLidarMapConfigKinematics::getVy_mmps(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getVy_mmps();
}

const std::optional<double>& cLidarMapConfigKinematics::getVz_mmps(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getVz_mmps();
}

const std::optional<double>& cLidarMapConfigKinematics::getVx_mmps(int month, int day) const { return getVx_mmps(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getVy_mmps(int month, int day) const { return getVy_mmps(to_date(month, day)); }
const std::optional<double>& cLidarMapConfigKinematics::getVz_mmps(int month, int day) const { return getVz_mmps(to_date(month, day)); }

/*** Translate Point Cloud Parameters ***/
const std::optional<bool>& cLidarMapConfigKinematics::getTranslateToGround(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getTranslateToGround();
}

const std::optional<double>& cLidarMapConfigKinematics::getTranslateDistance_m(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getTranslateDistance_m();
}

const std::optional<double>& cLidarMapConfigKinematics::getTranslateThreshold_pct(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getTranslateThreshold_pct();
}

const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& cLidarMapConfigKinematics::getTranslateTable(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getTranslateTable();
}


/*** Rotate Point Cloud Parameters ***/
const std::optional<bool>& cLidarMapConfigKinematics::getRotateToGround(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getRotateToGround();
}

const std::optional<double>& cLidarMapConfigKinematics::getRotatePitch_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getRotatePitch_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getRotateRoll_deg(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getRotateRoll_deg();
}

const std::optional<double>& cLidarMapConfigKinematics::getRotateThreshold_pct(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getRotateThreshold_pct();
}

const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& cLidarMapConfigKinematics::getRotateTable(int date) const
{
	auto it = find(date);
	if (it == mKinematics.end())
		throw std::logic_error("oops");

	return it->second.getRotateTable();
}


void cLidarMapConfigKinematics::clearDirtyFlag()
{
	for (auto& parameters : mKinematics)
	{
		parameters.second.clearDirtyFlag();
	}

	mDirty = false;
}

void cLidarMapConfigKinematics::setDirtyFlag(bool dirty)
{
	for (auto& parameters : mKinematics)
	{
		parameters.second.setDirty(dirty);
	}

	mDirty = dirty;
}

cLidarMapConfigKinematicParameters& cLidarMapConfigKinematics::add(const int date)
{
	if (mKinematics.contains(date))
	{
		return mKinematics.find(date)->second;
	}

	int day = date % 100;
	int month = date / 100;

	cLidarMapConfigKinematicParameters parameters = { month, day };

	mKinematics.insert(std::make_pair(date, std::move(parameters)));

	cLidarMapConfigKinematicParameters& result = mKinematics.find(date)->second;
	result.setDirtyFlag();

	mDirty = true;

	return result;
}

cLidarMapConfigKinematicParameters& cLidarMapConfigKinematics::add(const int month, const int day)
{
	return add(lidar_config::to_date(month, day));
}

void cLidarMapConfigKinematics::load(const nlohmann::json& jdoc)
{
	for (const auto& entry : jdoc)
	{
		if (entry.contains("date"))
		{
			std::string date = entry["date"];
			auto pos = date.find('/');
			int month = std::stoi(date.substr(0, pos));
			int day = std::stoi(date.substr(pos + 1));
			cLidarMapConfigKinematicParameters parameters(month, day);
			parameters.load(entry);

			mKinematics.insert(std::make_pair(parameters.date(), std::move(parameters)));
		}
	}
}

nlohmann::json cLidarMapConfigKinematics::save()
{
	nlohmann::json kinematicDoc;

	for (auto& kinematics : mKinematics)
	{
		auto entryDoc = kinematics.second.save();
		
		if (entryDoc.is_null())
			continue;

		kinematicDoc.push_back(kinematics.second.save());
	}

	return kinematicDoc;
}






































#if 0
cLidarMapConfigCatalog::cLidarMapConfigCatalog(int month, int day)
	: mEffectiveMonth(month), mEffectiveDay(day)

{}

const int cLidarMapConfigCatalog::date() const
{
	return (mEffectiveMonth * 100) + mEffectiveDay;
}

const int cLidarMapConfigCatalog::month() const
{
	return mEffectiveMonth;
}

const int cLidarMapConfigCatalog::day() const
{
	return mEffectiveDay;
}

void cLidarMapConfigCatalog::clear()
{
	mDirty = false;
	mScans.clear();
}

bool cLidarMapConfigCatalog::isDirty() const
{
	for (const auto& scan : mScans)
	{
		if (scan.isDirty())
			return true;
	}

	return mDirty;
}

void cLidarMapConfigCatalog::setDirty(bool dirty)
{
	mDirty = dirty;
}

bool cLidarMapConfigCatalog::empty() const
{
	return mScans.empty();
}

std::size_t cLidarMapConfigCatalog::size() const
{
	return mScans.size();
}

bool cLidarMapConfigCatalog::contains(const std::string& name) const
{
	for (const auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return true;
	}

	return false;
}

const cLidarMapConfigKinematicParameters& cLidarMapConfigCatalog::front() const { return mScans.front(); }
cLidarMapConfigScan& cLidarMapConfigCatalog::front() { return mScans.front(); }


cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::begin() { return mScans.begin(); }
cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::end() { return mScans.end(); }

cLidarMapConfigCatalog::const_iterator	cLidarMapConfigCatalog::begin() const { return mScans.cbegin(); }
cLidarMapConfigCatalog::const_iterator	cLidarMapConfigCatalog::end() const { return mScans.cend(); }

cLidarMapConfigCatalog::const_iterator	cLidarMapConfigCatalog::find_by_filename(const std::string& measurement_filename) const
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);
	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		auto name = safeFilename(it->getMeasurementName());
		if (filename == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::find_by_filename(const std::string& measurement_filename)
{
	using namespace nStringUtils;

	auto scan_file = removeProcessedTimestamp(measurement_filename);
	auto in = removeMeasurementTimestamp(scan_file.filename);
	auto filename = safeFilename(in.filename);

	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		auto name = safeFilename(it->getMeasurementName());
		if (filename == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigCatalog::const_iterator cLidarMapConfigCatalog::find(const std::string& name) const
{
	for (auto it = mScans.cbegin(); it != mScans.cend(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.cend();
}

cLidarMapConfigCatalog::iterator cLidarMapConfigCatalog::find(const std::string& name)
{
	for (auto it = mScans.begin(); it != mScans.end(); ++it)
	{
		if (it->getMeasurementName() == name)
			return it;
	}

	return mScans.end();
}

cLidarMapConfigScan& cLidarMapConfigCatalog::add(const std::string& name)
{
	for (auto& scan : mScans)
	{
		if (scan.getMeasurementName() == name)
			return scan;
	}

	cLidarMapConfigScan scan;

	scan.setMeasurementName(name);

	mScans.push_back(std::move(scan));

	return mScans.back();
}

void cLidarMapConfigCatalog::remove(const std::string& name)
{

}

const cLidarMapConfigScan& cLidarMapConfigCatalog::operator[](int index) const
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}

cLidarMapConfigScan& cLidarMapConfigCatalog::operator[](int index)
{
	if (index < 0)
		return mScans.front();

	if (index >= mScans.size())
		return mScans.back();

	return mScans[index];
}



void cLidarMapConfigCatalog::load(const nlohmann::json& jdoc)
{
	if (jdoc.contains("scans"))
	{
		const auto& scans = jdoc["scans"];

		for (const auto& entry : scans)
		{

			cLidarMapConfigScan scan;

			scan.setEffectiveDate(mEffectiveMonth, mEffectiveDay);

			scan.load(entry);

			mScans.push_back(std::move(scan));
		}
	}
}

nlohmann::json cLidarMapConfigCatalog::save()
{
	nlohmann::json catalogDoc;

	std::string date = std::to_string(mEffectiveMonth);
	date += "/";
	date += std::to_string(mEffectiveDay);

	catalogDoc["date"] = date;

	nlohmann::json scansDoc;

	for (auto& scan : mScans)
	{
		scansDoc.push_back(scan.save());
	}

	if (!scansDoc.is_null())
		catalogDoc["scans"] = scansDoc;

	mDirty = false;

	return catalogDoc;
}
#endif

