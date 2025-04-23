
#include "LidarMapConfigDefaults.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>

//
//  Lidar Map Config Defaults
//
cLidarMapConfigDefaults::cLidarMapConfigDefaults()
{}

void cLidarMapConfigDefaults::clear()
{
	mDirty = false;

	mDollySpeeds.clear();
	mDollyHeight.clear();
	mSensorOrientation.clear();
	mSensorLimits.clear();
	mOptions.clear();
}

bool cLidarMapConfigDefaults::isDirty() const
{
	bool dirty = mDirty;

	dirty |= mDollySpeeds.isDirty();
	dirty |= mDollyHeight.isDirty();
	dirty |= mSensorOrientation.isDirty();
	dirty |= mSensorLimits.isDirty();
	dirty |= mOptions.isDirty();

	return dirty;
}


const cLidarMapConfigDefaults_DollySpeeds& cLidarMapConfigDefaults::getDollySpeeds() const { return mDollySpeeds; }
cLidarMapConfigDefaults_DollySpeeds& cLidarMapConfigDefaults::getDollySpeeds() { return mDollySpeeds; }

const cLidarMapConfigDefaults_DollyHeight& cLidarMapConfigDefaults::getDollyHeight() const { return mDollyHeight; }
cLidarMapConfigDefaults_DollyHeight& cLidarMapConfigDefaults::getDollyHeight() { return mDollyHeight; }

const cLidarMapConfigDefaults_SensorOrientation& cLidarMapConfigDefaults::getSensorOrientation() const { return mSensorOrientation; }
cLidarMapConfigDefaults_SensorOrientation& cLidarMapConfigDefaults::getSensorOrientation() { return mSensorOrientation; }

const cLidarMapConfigDefaults_SensorLimits& cLidarMapConfigDefaults::getSensorLimits() const { return mSensorLimits; }
cLidarMapConfigDefaults_SensorLimits& cLidarMapConfigDefaults::getSensorLimits() { return mSensorLimits; }

const cLidarMapConfigDefaults_Options& cLidarMapConfigDefaults::getOptions() const { return mOptions; }
cLidarMapConfigDefaults_Options& cLidarMapConfigDefaults::getOptions() { return mOptions; }

const cLidarMapConfigDefaults_Flattening& cLidarMapConfigDefaults::getFlattening() const { return mFlattening; }
cLidarMapConfigDefaults_Flattening& cLidarMapConfigDefaults::getFlattening() { return mFlattening; }

void cLidarMapConfigDefaults::setDirty(bool dirty)
{
	mDirty = dirty;
}

void cLidarMapConfigDefaults::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("defaults"))
		return;
		
	auto defaults = jdoc["defaults"];

	mDollySpeeds.load(defaults);
	mDollyHeight.load(defaults);
	mSensorOrientation.load(defaults);
	mSensorLimits.load(defaults);
	mOptions.load(defaults);
	mFlattening.load(defaults);
}

nlohmann::json cLidarMapConfigDefaults::save()
{
	nlohmann::json defaults;

	defaults["dolly speeds"] = mDollySpeeds.save();
	defaults["dolly height"] = mDollyHeight.save();
	defaults["sensor orientation"] = mSensorOrientation.save();
	defaults["sensor limits"] = mSensorLimits.save();
	defaults["options"] = mOptions.save();
	defaults["flattening"] = mFlattening.save();

	mDirty = false;

	return defaults;
}


//
//  Lidar Map Config Defaults - Dolly Speeds
//
cLidarMapConfigDefaults_DollySpeeds::cLidarMapConfigDefaults_DollySpeeds()
{}

void cLidarMapConfigDefaults_DollySpeeds::clear()
{
	mDirty = false;

}

bool cLidarMapConfigDefaults_DollySpeeds::isDirty() const
{
	return mDirty;
}

int cLidarMapConfigDefaults_DollySpeeds::getVx_mmps() const { return mVx_mmps; }
int cLidarMapConfigDefaults_DollySpeeds::getVy_mmps() const { return mVy_mmps; }
int cLidarMapConfigDefaults_DollySpeeds::getVz_mmps() const { return mVz_mmps; }

void cLidarMapConfigDefaults_DollySpeeds::setVx_mmps(int vx)
{
	mDirty |= (mVx_mmps != vx);
	mVx_mmps = vx;
}

void cLidarMapConfigDefaults_DollySpeeds::setVy_mmps(int vy)
{
	mDirty |= (mVy_mmps != vy);
	mVy_mmps = vy;
}

void cLidarMapConfigDefaults_DollySpeeds::setVz_mmps(int vz)
{
	mDirty |= (mVz_mmps != vz);
	mVz_mmps = vz;
}

bool cLidarMapConfigDefaults_DollySpeeds::operator==(const cLidarMapConfigDefaults_DollySpeeds& rhs) const
{
	return (mVx_mmps == rhs.mVx_mmps) && (mVy_mmps == rhs.mVy_mmps) && (mVz_mmps == rhs.mVz_mmps);
}

bool cLidarMapConfigDefaults_DollySpeeds::operator!=(const cLidarMapConfigDefaults_DollySpeeds& rhs) const
{
	return !operator==(rhs);
}

void cLidarMapConfigDefaults_DollySpeeds::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("dolly speeds"))
		return;

	auto dolly_speeds = jdoc["dolly speeds"];

	if (dolly_speeds.contains("Vx (mm/s)"))
		mVx_mmps = static_cast<int>(dolly_speeds["Vx (mm/s)"].get<double>());

	if (dolly_speeds.contains("Vy (mm/s)"))
		mVy_mmps = static_cast<int>(dolly_speeds["Vy (mm/s)"].get<double>());

	if (dolly_speeds.contains("Vz (mm/s)"))
		mVz_mmps = static_cast<int>(dolly_speeds["Vz (mm/s)"].get<double>());
}

nlohmann::json cLidarMapConfigDefaults_DollySpeeds::save()
{
	nlohmann::json dolly_speeds;

	dolly_speeds["Vx (mm/s)"] = mVx_mmps;
	dolly_speeds["Vy (mm/s)"] = mVy_mmps;
	dolly_speeds["Vz (mm/s)"] = mVz_mmps;

	mDirty = false;

	return dolly_speeds;
}


//
//  Lidar Map Config Defaults - Dolly Height
//
cLidarMapConfigDefaults_DollyHeight::cLidarMapConfigDefaults_DollyHeight()
{}

void cLidarMapConfigDefaults_DollyHeight::clear()
{
	mHeight_m = 5.0;
	mAxis = eHeightAxis::Z;

	mDirty = false;
}

bool cLidarMapConfigDefaults_DollyHeight::isDirty() const
{
	return mDirty;
}

double cLidarMapConfigDefaults_DollyHeight::getHeight_m() const { return mHeight_m; }
cLidarMapConfigDefaults_DollyHeight::eHeightAxis cLidarMapConfigDefaults_DollyHeight::getHeightAxis() const { return mAxis; }

void cLidarMapConfigDefaults_DollyHeight::setHeight_m(double height)
{
	mDirty |= (mHeight_m != height);
	mHeight_m = height;
}

void cLidarMapConfigDefaults_DollyHeight::setHeightAxis(eHeightAxis axis)
{
	mDirty |= (mAxis != axis);
	mAxis = axis;
}

bool cLidarMapConfigDefaults_DollyHeight::operator==(const cLidarMapConfigDefaults_DollyHeight& rhs) const
{
	return (mHeight_m == rhs.mHeight_m) && (mAxis == rhs.mAxis);
}

bool cLidarMapConfigDefaults_DollyHeight::operator!=(const cLidarMapConfigDefaults_DollyHeight& rhs) const
{
	return !operator==(rhs);
}

void cLidarMapConfigDefaults_DollyHeight::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("dolly height"))
		return;

	auto dolly_height = jdoc["dolly height"];

	if (dolly_height.contains("height (m)"))
		mHeight_m = dolly_height["height (m)"].get<double>();

	if (dolly_height.contains("height axis"))
	{
		std::string axis = dolly_height["height axis"];
		if (!axis.empty())
		{
			if ((axis[0] == 'x') && (axis[0] == 'X'))
				mAxis = eHeightAxis::X;
			if ((axis[0] == 'y') && (axis[0] == 'Y'))
				mAxis = eHeightAxis::Y;
			if ((axis[0] == 'z') && (axis[0] == 'Z'))
				mAxis = eHeightAxis::Z;
		}
	}
}

nlohmann::json cLidarMapConfigDefaults_DollyHeight::save()
{
	nlohmann::json dolly_height;

	dolly_height["height (m)"] = mHeight_m;

	switch (mAxis)
	{
	case eHeightAxis::X:
		dolly_height["height axis"] = "X";
		break;
	case eHeightAxis::Y:
		dolly_height["height axis"] = "Y";
		break;
	case eHeightAxis::Z:
		dolly_height["height axis"] = "Z";
		break;
	}

	mDirty = false;

	return dolly_height;
}


//
//  Lidar Map Config Defaults - Sensor Orientation
//
cLidarMapConfigDefaults_SensorOrientation::cLidarMapConfigDefaults_SensorOrientation()
{}

void cLidarMapConfigDefaults_SensorOrientation::clear()
{
	mDirty = false;

	mPitch_deg = -90.0;
	mRoll_deg = 0.0;
	mYaw_deg = 90.0;
}

bool cLidarMapConfigDefaults_SensorOrientation::isDirty() const
{
	return mDirty;
}

double cLidarMapConfigDefaults_SensorOrientation::getPitch_deg() const { return mPitch_deg; }
double cLidarMapConfigDefaults_SensorOrientation::getRoll_deg() const { return mRoll_deg; }
double cLidarMapConfigDefaults_SensorOrientation::getYaw_deg() const { return mYaw_deg; }

void cLidarMapConfigDefaults_SensorOrientation::setPitch_deg(double pitch)
{
	mDirty |= (mPitch_deg != pitch);
	mPitch_deg = pitch;
}

void cLidarMapConfigDefaults_SensorOrientation::setRoll_deg(double roll)
{
	mDirty |= (mRoll_deg != roll);
	mRoll_deg = roll;
}

void cLidarMapConfigDefaults_SensorOrientation::setYaw_deg(double yaw)
{
	mDirty |= (mYaw_deg != yaw);
	mYaw_deg = yaw;
}

bool cLidarMapConfigDefaults_SensorOrientation::operator==(const cLidarMapConfigDefaults_SensorOrientation& rhs) const
{
	return (mPitch_deg == rhs.mPitch_deg) && (mRoll_deg == rhs.mRoll_deg) && (mYaw_deg == rhs.mYaw_deg);
}

bool cLidarMapConfigDefaults_SensorOrientation::operator!=(const cLidarMapConfigDefaults_SensorOrientation& rhs) const
{
	return !operator==(rhs);
}

void cLidarMapConfigDefaults_SensorOrientation::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("sensor orientation"))
		return;

	auto sensor_orientation = jdoc["sensor orientation"];

	if (sensor_orientation.contains("pitch (deg)"))
		mPitch_deg = sensor_orientation["pitch (deg)"].get<double>();

	if (sensor_orientation.contains("roll (deg)"))
		mRoll_deg = sensor_orientation["roll (deg)"].get<double>();

	if (sensor_orientation.contains("yaw (deg)"))
		mYaw_deg = sensor_orientation["yaw (deg)"].get<double>();
}

nlohmann::json cLidarMapConfigDefaults_SensorOrientation::save()
{
	nlohmann::json sensor_orientation;

	sensor_orientation["pitch (deg)"] = mPitch_deg;
	sensor_orientation["roll (deg)"] = mRoll_deg;
	sensor_orientation["yaw (deg)"] = mYaw_deg;

	mDirty = false;

	return sensor_orientation;
}


//
//  Lidar Map Config Defaults - Sensor Limits
//
cLidarMapConfigDefaults_SensorLimits::cLidarMapConfigDefaults_SensorLimits()
{}

void cLidarMapConfigDefaults_SensorLimits::clear()
{
	mDirty = false;

	mMinDistance_m = 1.0;
	mMaxDistance_m = 40.0;
	mMinAzimuth_deg = 135.0;
	mMaxAzimuth_deg = 225.0;
	mMinAltitude_deg = -10.0;
	mMaxAltitude_deg = 10.0;
}

bool cLidarMapConfigDefaults_SensorLimits::isDirty() const
{
	return mDirty;
}

double cLidarMapConfigDefaults_SensorLimits::getMinDistance_m() const { return mMinDistance_m; }
double cLidarMapConfigDefaults_SensorLimits::getMaxDistance_m() const { return mMaxDistance_m; }
double cLidarMapConfigDefaults_SensorLimits::getMinAzimuth_deg() const { return mMinAzimuth_deg; }
double cLidarMapConfigDefaults_SensorLimits::getMaxAzimuth_deg() const { return mMaxAzimuth_deg; }
double cLidarMapConfigDefaults_SensorLimits::getMinAltitude_deg() const { return mMinAltitude_deg; }
double cLidarMapConfigDefaults_SensorLimits::getMaxAltitude_deg() const { return mMaxAltitude_deg; }

void cLidarMapConfigDefaults_SensorLimits::setMinDistance_m(double distance_m)
{
	mDirty |= (mMinDistance_m != distance_m);
	mMinDistance_m = distance_m;
}

void cLidarMapConfigDefaults_SensorLimits::setMaxDistance_m(double distance_m)
{
	mDirty |= (mMaxDistance_m != distance_m);
	mMaxDistance_m = distance_m;
}

void cLidarMapConfigDefaults_SensorLimits::setMinAzimuth_deg(double azimuth_deg)
{
	mDirty |= (mMinAzimuth_deg != azimuth_deg);
	mMinAzimuth_deg = azimuth_deg;
}

void cLidarMapConfigDefaults_SensorLimits::setMaxAzimuth_deg(double azimuth_deg)
{
	mDirty |= (mMaxAzimuth_deg != azimuth_deg);
	mMaxAzimuth_deg = azimuth_deg;
}

void cLidarMapConfigDefaults_SensorLimits::setMinAltitude_deg(double altitude_deg)
{
	mDirty |= (mMinAltitude_deg != altitude_deg);
	mMinAltitude_deg = altitude_deg;
}

void cLidarMapConfigDefaults_SensorLimits::setMaxAltitude_deg(double altitude_deg)
{
	mDirty |= (mMaxAltitude_deg != altitude_deg);
	mMaxAltitude_deg = altitude_deg;
}

bool cLidarMapConfigDefaults_SensorLimits::operator==(const cLidarMapConfigDefaults_SensorLimits& rhs) const
{
	return (mMinDistance_m == rhs.mMinDistance_m) && (mMaxDistance_m == rhs.mMaxDistance_m)
		&& (mMinAzimuth_deg == rhs.mMinAzimuth_deg) && (mMaxAzimuth_deg == rhs.mMaxAzimuth_deg)
		&& (mMinAltitude_deg == rhs.mMinAltitude_deg) && (mMaxAltitude_deg == rhs.mMaxAltitude_deg);
}

bool cLidarMapConfigDefaults_SensorLimits::operator!=(const cLidarMapConfigDefaults_SensorLimits& rhs) const
{
	return !operator==(rhs);
}

void cLidarMapConfigDefaults_SensorLimits::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("sensor limits"))
		return;

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

nlohmann::json cLidarMapConfigDefaults_SensorLimits::save()
{
	nlohmann::json sensor_limits;

	sensor_limits["min distance (m)"] = mMinDistance_m;
	sensor_limits["max distance (m)"] = mMaxDistance_m;
	sensor_limits["min azimuth (deg)"] = mMinAzimuth_deg;
	sensor_limits["max azimuth (deg)"] = mMaxAzimuth_deg;
	sensor_limits["min altitude (deg)"] = mMinAltitude_deg;
	sensor_limits["max altitude (deg)"] = mMaxAltitude_deg;

	mDirty = false;

	return sensor_limits;
}


//
//  Lidar Map Config Defaults - Options
//
cLidarMapConfigDefaults_Options::cLidarMapConfigDefaults_Options()
{}

void cLidarMapConfigDefaults_Options::clear()
{
	mDirty = false;

	mTranslateToGround = true;
	mTranslationThreshold_pct = 1.0;

	mRotateToGround = true;
	mRotationThreshold_pct = 1.0;
}

bool cLidarMapConfigDefaults_Options::isDirty() const
{
	return mDirty;
}

bool cLidarMapConfigDefaults_Options::getTranslateToGround() const { return mTranslateToGround; }
double cLidarMapConfigDefaults_Options::getTranslationThreshold_pct() const { return mTranslationThreshold_pct; }

bool cLidarMapConfigDefaults_Options::getRotateToGround() const { return mRotateToGround; }
double cLidarMapConfigDefaults_Options::getRotationThreshold_pct() const { return mRotationThreshold_pct; }


void cLidarMapConfigDefaults_Options::setTranslateToGround(bool enable)
{
	mDirty |= (mTranslateToGround != enable);
	mTranslateToGround = enable;
}

void cLidarMapConfigDefaults_Options::setTranslationThreshold_pct(double threshold_pct)
{
	mDirty |= (mTranslationThreshold_pct != threshold_pct);
	mTranslationThreshold_pct = threshold_pct;
}

void cLidarMapConfigDefaults_Options::setRotateToGround(bool enable)
{
	mDirty |= (mRotateToGround != enable);
	mRotateToGround = enable;
}

void cLidarMapConfigDefaults_Options::setRotationThreshold_pct(double threshold_pct)
{
	mDirty |= (mRotationThreshold_pct != threshold_pct);
	mRotationThreshold_pct = threshold_pct;
}

bool cLidarMapConfigDefaults_Options::operator==(const cLidarMapConfigDefaults_Options& rhs) const
{
	return (mTranslateToGround == rhs.mTranslateToGround) && (mTranslationThreshold_pct == rhs.mTranslationThreshold_pct)
		&& (mRotateToGround == rhs.mRotateToGround) && (mRotationThreshold_pct == rhs.mRotationThreshold_pct);
}

bool cLidarMapConfigDefaults_Options::operator!=(const cLidarMapConfigDefaults_Options& rhs) const
{
	return !operator==(rhs);
}

void cLidarMapConfigDefaults_Options::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("options"))
		return;

	auto options = jdoc["options"];

	if (options.contains("translate to ground"))
		mTranslateToGround = options["translate to ground"];

	if (options.contains("translation threshold (%)"))
		mTranslationThreshold_pct = options["translation threshold (%)"].get<double>();

	if (options.contains("rotation to ground"))
		mRotateToGround = options["rotation to ground"];

	if (options.contains("rotation threshold (%)"))
		mRotationThreshold_pct = options["rotation threshold (%)"].get<double>();
}

nlohmann::json cLidarMapConfigDefaults_Options::save()
{
	nlohmann::json options;

	options["translate to ground"] = mTranslateToGround;
	options["translation threshold (%)"] = mTranslationThreshold_pct;
	options["rotate to ground"] = mRotateToGround;
	options["rotation threshold (%)"] = mRotationThreshold_pct;

	mDirty = false;

	return options;
}


//
//  Lidar Map Config Defaults - Flattening
//
cLidarMapConfigDefaults_Flattening::cLidarMapConfigDefaults_Flattening()
{
}

void cLidarMapConfigDefaults_Flattening::clear()
{
	mDirty = false;

	mFlattenPointCloud = true;
	mMaxAngle_deg = 10.0;
	mThreshold_pct = 0.1;
}

bool cLidarMapConfigDefaults_Flattening::isDirty() const
{
	return mDirty;
}

bool cLidarMapConfigDefaults_Flattening::getFlatteningPointCloud() const { return mFlattenPointCloud; }
double cLidarMapConfigDefaults_Flattening::getThreshold_pct() const { return mThreshold_pct; }
double cLidarMapConfigDefaults_Flattening::getMaxAngle_deg() const { return mMaxAngle_deg; }


void cLidarMapConfigDefaults_Flattening::setFlattenPointCloud(bool enable)
{
	mDirty |= (mFlattenPointCloud != enable);
	mFlattenPointCloud = enable;
}

void cLidarMapConfigDefaults_Flattening::setThreshold_pct(double threshold_pct)
{
	mDirty |= (mThreshold_pct != threshold_pct);
	mThreshold_pct = threshold_pct;
}

void cLidarMapConfigDefaults_Flattening::setMaxAngle_deg(double max_angle_deg)
{
	mDirty |= (mMaxAngle_deg != max_angle_deg);
	mMaxAngle_deg = max_angle_deg;
}

bool cLidarMapConfigDefaults_Flattening::operator==(const cLidarMapConfigDefaults_Flattening& rhs) const
{
	return (mFlattenPointCloud == rhs.mFlattenPointCloud) && (mThreshold_pct == rhs.mThreshold_pct)
		&& (mMaxAngle_deg == rhs.mMaxAngle_deg);
}

bool cLidarMapConfigDefaults_Flattening::operator!=(const cLidarMapConfigDefaults_Flattening& rhs) const
{
	return !operator==(rhs);
}

void cLidarMapConfigDefaults_Flattening::load(const nlohmann::json& jdoc)
{
	if (!jdoc.contains("flattening"))
		return;

	auto flattening = jdoc["flattening"];

	if (flattening.contains("flatten point cloud"))
		mFlattenPointCloud = flattening["flatten point cloud"];

	if (flattening.contains("threshold (%)"))
		mThreshold_pct = flattening["threshold (%)"].get<double>();

	if (flattening.contains("maximum angle (deg)"))
		mMaxAngle_deg = flattening["maximum angle (deg)"].get<double>();
}

nlohmann::json cLidarMapConfigDefaults_Flattening::save()
{
	nlohmann::json flattening;

	flattening["flatten point cloud"] = mFlattenPointCloud;
	flattening["threshold (%)"] = mThreshold_pct;
	flattening["maximum angle (deg)"] = mMaxAngle_deg;

	mDirty = false;

	return flattening;
}



