
#pragma once

#include "datatypes.hpp"
#include "PointCloudUtilTypes.hpp"
#include "KinematicDataTypes.hpp"
#include "LidarMapConfigKinematics.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <optional>


class cLidarMapConfigScanInfo
{
public:
	cLidarMapConfigScanInfo();
	explicit cLidarMapConfigScanInfo(const std::string& name);
	~cLidarMapConfigScanInfo() = default;

	void clear();
	bool empty() const;

	bool isDirty() const;

	const std::string& getMeasurementName() const;

	/*** Sensor Mounting Parameters ***/
	const std::optional<double>& getSensorMountPitch_deg() const;
	const std::optional<double>& getSensorMountRoll_deg() const;
	const std::optional<double>& getSensorMountYaw_deg() const;

	const std::optional<rfm::rappPoint_t>& getReferencePoint() const;

	/*** LiDAR Parameters ***/
	const std::optional<double>& getMinDistance_m() const;
	const std::optional<double>& getMaxDistance_m() const;
	const std::optional<double>& getMinAzimuth_deg() const;
	const std::optional<double>& getMaxAzimuth_deg() const;
	const std::optional<double>& getMinAltitude_deg() const;
	const std::optional<double>& getMaxAltitude_deg() const;


	void setMeasurementName(const std::string& name);

	void resetSensorMountPitch_deg();
	void resetSensorMountRoll_deg();
	void resetSensorMountYaw_deg();

	void setSensorMountPitch_deg(const std::optional<double>&);
	void setSensorMountRoll_deg(const std::optional<double>&);
	void setSensorMountYaw_deg(const std::optional<double>&);

	void resetReferencePoint();
	void setReferencePoint(const std::optional<rfm::rappPoint_t>& p);

	void resetMinDistance_m();
	void resetMaxDistance_m();
	void resetMinAzimuth_deg();
	void resetMaxAzimuth_deg();
	void resetMinAltitude_deg();
	void resetMaxAltitude_deg();

	void setMinDistance_m(const std::optional<double>& dist_m);
	void setMaxDistance_m(const std::optional<double>& dist_m);
	void setMinAzimuth_deg(const std::optional<double>& az_deg);
	void setMaxAzimuth_deg(const std::optional<double>& az_deg);
	void setMinAltitude_deg(const std::optional<double>& alt_deg);
	void setMaxAltitude_deg(const std::optional<double>& alt_deg);

	void clearDirtyFlag();
	void setDirtyFlag();

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& infoDoc);

private:
	template<typename T>
	void reset(std::optional<T>& var);

protected:
	std::string mMeasurementName;

	// Dolly - Sensor Mounting Parameters
	std::optional<double> mSensorMountPitch_deg;
	std::optional<double> mSensorMountRoll_deg;
	std::optional<double> mSensorMountYaw_deg;

	// Reference point to normalize scans
	std::optional<rfm::rappPoint_t> mReferencePoint;

	// LiDAR Sensor Limits
	std::optional<double> mMinDistance_m;
	std::optional<double> mMaxDistance_m;
	std::optional<double> mMinAzimuth_deg;
	std::optional<double> mMaxAzimuth_deg;
	std::optional<double> mMinAltitude_deg;
	std::optional<double> mMaxAltitude_deg;

private:
	bool mDirty = false;
};


class cLidarMapConfigScan : public cLidarMapConfigScanInfo
{
public:
	typedef cLidarMapConfigKinematics::iterator			iterator;
	typedef cLidarMapConfigKinematics::const_iterator	const_iterator;

public:
	cLidarMapConfigScan();
	explicit cLidarMapConfigScan(const std::string& name);
	~cLidarMapConfigScan() = default;

	void clear();
	bool empty() const;

	bool isDirty() const;

	/*** Dolly Kinematic Parameters ***/
	const std::optional<eKinematicModel>* getKinematicModel(int date) const;
	const std::optional<eKinematicModel>* getKinematicModel(int month, int day) const;

	const std::optional<eOrientationModel>* getOrientationModel(int date) const;
	const std::optional<eOrientationModel>* getOrientationModel(int month, int day) const;

	const std::optional<eTranslateToGroundModel>* getTranslateToGroundModel(int date) const;
	const std::optional<eTranslateToGroundModel>* getTranslateToGroundModel(int month, int day) const;

	const std::optional<eRotateToGroundModel>* getRotateToGroundModel(int date) const;
	const std::optional<eRotateToGroundModel>* getRotateToGroundModel(int month, int day) const;


	/*** Sensor Orientation Parameters - Constant: Angles ***/
	const std::optional<double>* getSensorPitchOffset_deg(int date) const;
	const std::optional<double>* getSensorRollOffset_deg(int date) const;
	const std::optional<double>* getSensorYawOffset_deg(int date) const;

	const std::optional<double>* getSensorPitchOffset_deg(int month, int day) const;
	const std::optional<double>* getSensorRollOffset_deg(int month, int day) const;
	const std::optional<double>* getSensorYawOffset_deg(int month, int day) const;

	/*** Sensor Orientation Parameters - Linear: Start and Stop Angles ***/
	const std::optional<double>* getStartPitchOffset_deg(int date) const;
	const std::optional<double>* getStartRollOffset_deg(int date) const;
	const std::optional<double>* getStartYawOffset_deg(int date) const;

	const std::optional<double>* getEndPitchOffset_deg(int date) const;
	const std::optional<double>* getEndRollOffset_deg(int date) const;
	const std::optional<double>* getEndYawOffset_deg(int date) const;

	const std::optional<double>* getStartPitchOffset_deg(int month, int day) const;
	const std::optional<double>* getStartRollOffset_deg(int month, int day) const;
	const std::optional<double>* getStartYawOffset_deg(int month, int day) const;

	const std::optional<double>* getEndPitchOffset_deg(int month, int day) const;
	const std::optional<double>* getEndRollOffset_deg(int month, int day) const;
	const std::optional<double>* getEndYawOffset_deg(int month, int day) const;

	/*** Sensor Orientation Parameters - Intrep Table: Data Points ***/
	const std::vector<kdt::sDollyOrientationInterpPoint_t>* getOrientationTable(int date) const;
	const std::vector<kdt::sDollyOrientationInterpPoint_t>* getOrientationTable(int month, int day) const;

	/*** Sensor Translation Parameters - Start/End Positions ***/
	const std::optional<double>* getStart_X_m(int date) const;
	const std::optional<double>* getStart_Y_m(int date) const;
	const std::optional<double>* getStart_Z_m(int date) const;

	const std::optional<double>* getEnd_X_m(int date) const;
	const std::optional<double>* getEnd_Y_m(int date) const;
	const std::optional<double>* getEnd_Z_m(int date) const;

	/*** Sensor Translation Parameters - Constant: Speeds ***/
	const std::optional<double>* getVx_mmps(int date) const;
	const std::optional<double>* getVy_mmps(int date) const;
	const std::optional<double>* getVz_mmps(int date) const;

	const std::optional<double>* getVx_mmps(int month, int day) const;
	const std::optional<double>* getVy_mmps(int month, int day) const;
	const std::optional<double>* getVz_mmps(int month, int day) const;

	/*** Translate Point Cloud Parameters ***/
	const std::optional<bool>* getTranslateToGround(int date) const;
	const std::optional<double>* getTranslateDistance_m(int date) const;
	const std::optional<double>* getTranslateThreshold_pct(int date) const;

	const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>* getTranslateTable(int date) const;

	/*** Rotate Point Cloud Parameters ***/
	const std::optional<bool>* getRotateToGround(int date) const;
	const std::optional<double>* getRotatePitch_deg(int date) const;
	const std::optional<double>* getRotateRoll_deg(int date) const;
	const std::optional<double>* getRotateThreshold_pct(int date) const;

	const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>* getRotateTable(int date) const;


	bool contains(const int date) const;
	bool contains(const int month, const int day) const;

	const cLidarMapConfigKinematicParameters& front() const;
	cLidarMapConfigKinematicParameters& front();

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const_iterator	find(const int date) const;
	iterator		find(const int date);

	const_iterator	find(const int month, const int day) const;
	iterator		find(const int month, const int day);

	const_iterator	find_exact(const int month, const int day) const;
	iterator		find_exact(const int month, const int day);

	cLidarMapConfigKinematicParameters& add(const int date);
	cLidarMapConfigKinematicParameters& add(const int month, const int day);

	void clearDirtyFlag();
	void setDirtyFlag();

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	template<typename T>
	void reset(std::optional<T>& var);

private:

	cLidarMapConfigKinematics mKinematics;

	friend class cLidarMapConfigFile;
	friend class cLidarMapConfigCatalog;
};

/***
 * Implementation Details
 ***/
template<typename T>
inline void cLidarMapConfigScanInfo::reset(std::optional<T>& var)
{
	if (var.has_value())
	{
		var.reset();
		mDirty = true;
	}
}

template<typename T>
inline void cLidarMapConfigScan::reset(std::optional<T>& var)
{
	if (var.has_value())
	{
		var.reset();
		mDirty = true;
	}
}
