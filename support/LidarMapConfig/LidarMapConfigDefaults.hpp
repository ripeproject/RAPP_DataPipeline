
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>



class cLidarMapConfigDefaults_DollySpeeds
{
public:
	cLidarMapConfigDefaults_DollySpeeds();
	~cLidarMapConfigDefaults_DollySpeeds() = default;

	void clear();

	bool isDirty() const;

	int getVx_mmps() const;
	int getVy_mmps() const;
	int getVz_mmps() const;

	void setVx_mmps(int vx);
	void setVy_mmps(int vy);
	void setVz_mmps(int vz);

	bool operator==(const cLidarMapConfigDefaults_DollySpeeds& rhs) const;
	bool operator!=(const cLidarMapConfigDefaults_DollySpeeds& rhs) const;

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	int mVx_mmps = 0;
	int mVy_mmps = 450;
	int mVz_mmps = 0;

	friend class cLidarMapConfigDefaults;
};


class cLidarMapConfigDefaults_DollyHeight
{
public:
	enum class eHeightAxis {X, Y, Z};

public:
	cLidarMapConfigDefaults_DollyHeight();
	~cLidarMapConfigDefaults_DollyHeight() = default;

	void clear();

	bool isDirty() const;

	double		getHeight_m() const;
	eHeightAxis getHeightAxis() const;

	void setHeight_m(double height);
	void setHeightAxis(eHeightAxis axis);

	bool operator==(const cLidarMapConfigDefaults_DollyHeight& rhs) const;
	bool operator!=(const cLidarMapConfigDefaults_DollyHeight& rhs) const;

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	double mHeight_m = 5.0;
	eHeightAxis mAxis = eHeightAxis::Z;

	friend class cLidarMapConfigDefaults;
};


class cLidarMapConfigDefaults_SensorOrientation
{
public:
	cLidarMapConfigDefaults_SensorOrientation();
	~cLidarMapConfigDefaults_SensorOrientation() = default;

	void clear();

	bool isDirty() const;

	double getPitch_deg() const;
	double getRoll_deg() const;
	double getYaw_deg() const;

	void setPitch_deg(double pitch);
	void setRoll_deg(double roll);
	void setYaw_deg(double yaw);

	bool operator==(const cLidarMapConfigDefaults_SensorOrientation& rhs) const;
	bool operator!=(const cLidarMapConfigDefaults_SensorOrientation& rhs) const;

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	double mPitch_deg = -90.0;
	double mRoll_deg = 0.0;
	double mYaw_deg = 90.0;

	friend class cLidarMapConfigDefaults;
};


class cLidarMapConfigDefaults_SensorLimits
{
public:
	cLidarMapConfigDefaults_SensorLimits();
	~cLidarMapConfigDefaults_SensorLimits() = default;

	void clear();

	bool isDirty() const;

	double getMinDistance_m() const;
	double getMaxDistance_m() const;
	double getMinAzimuth_deg() const;
	double getMaxAzimuth_deg() const;
	double getMinAltitude_deg() const;
	double getMaxAltitude_deg() const;


	void setMinDistance_m(double distance_m);
	void setMaxDistance_m(double distance_m);
	void setMinAzimuth_deg(double azimuth_deg);
	void setMaxAzimuth_deg(double azimuth_deg);
	void setMinAltitude_deg(double altitude_deg);
	void setMaxAltitude_deg(double altitude_deg);

	bool operator==(const cLidarMapConfigDefaults_SensorLimits& rhs) const;
	bool operator!=(const cLidarMapConfigDefaults_SensorLimits& rhs) const;

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	double mMinDistance_m = 1.0;
	double mMaxDistance_m = 40.0;
	double mMinAzimuth_deg = 135.0;
	double mMaxAzimuth_deg = 225.0;
	double mMinAltitude_deg = -10.0;
	double mMaxAltitude_deg = 10.0;

	friend class cLidarMapConfigDefaults;
};


class cLidarMapConfigDefaults_Options
{
public:
	cLidarMapConfigDefaults_Options();
	~cLidarMapConfigDefaults_Options() = default;

	void clear();

	bool isDirty() const;

	bool getTranslateToGround() const;
	double getTranslationThreshold_pct() const;

	bool getRotateToGround() const;
	double getRotationThreshold_pct() const;


	void setTranslateToGround(bool enable);
	void setTranslationThreshold_pct(double threshold_pct);
	void setRotateToGround(bool enable);
	void setRotationThreshold_pct(double threshold_pct);

	bool operator==(const cLidarMapConfigDefaults_Options& rhs) const;
	bool operator!=(const cLidarMapConfigDefaults_Options& rhs) const;

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	bool mTranslateToGround = true;
	double mTranslationThreshold_pct = 1.0;

	bool mRotateToGround = true;
	double mRotationThreshold_pct = 1.0;

	friend class cLidarMapConfigDefaults;
};


class cLidarMapConfigDefaults_Flattening
{
public:
	cLidarMapConfigDefaults_Flattening();
	~cLidarMapConfigDefaults_Flattening() = default;

	void clear();

	bool isDirty() const;

	bool getFlatteningPointCloud() const;
	double getThreshold_pct() const;
	double getMaxAngle_deg() const;


	void setFlattenPointCloud(bool enable);
	void setThreshold_pct(double threshold_pct);
	void setMaxAngle_deg(double max_angle_deg);

	bool operator==(const cLidarMapConfigDefaults_Flattening& rhs) const;
	bool operator!=(const cLidarMapConfigDefaults_Flattening& rhs) const;

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	bool	mFlattenPointCloud = true;
	double	mMaxAngle_deg = 10.0;
	double	mThreshold_pct = 0.1;

	friend class cLidarMapConfigDefaults;
};


class cLidarMapConfigDefaults
{
public:
	cLidarMapConfigDefaults();
	~cLidarMapConfigDefaults() = default;

	void clear();

	bool isDirty() const;

	const cLidarMapConfigDefaults_DollySpeeds& getDollySpeeds() const;
	cLidarMapConfigDefaults_DollySpeeds& getDollySpeeds();

	const cLidarMapConfigDefaults_DollyHeight& getDollyHeight() const;
	cLidarMapConfigDefaults_DollyHeight& getDollyHeight();

	const cLidarMapConfigDefaults_SensorOrientation& getSensorOrientation() const;
	cLidarMapConfigDefaults_SensorOrientation& getSensorOrientation();

	const cLidarMapConfigDefaults_SensorLimits& getSensorLimits() const;
	cLidarMapConfigDefaults_SensorLimits& getSensorLimits();

	const cLidarMapConfigDefaults_Options& getOptions() const;
	cLidarMapConfigDefaults_Options& getOptions();

	const cLidarMapConfigDefaults_Flattening& getFlattening() const;
	cLidarMapConfigDefaults_Flattening& getFlattening();


protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	cLidarMapConfigDefaults_DollySpeeds			mDollySpeeds;
	cLidarMapConfigDefaults_DollyHeight			mDollyHeight;
	cLidarMapConfigDefaults_SensorOrientation	mSensorOrientation;
	cLidarMapConfigDefaults_SensorLimits		mSensorLimits;
	cLidarMapConfigDefaults_Options				mOptions;
	cLidarMapConfigDefaults_Flattening			mFlattening;

	friend class cLidarMapConfigFile;
};


