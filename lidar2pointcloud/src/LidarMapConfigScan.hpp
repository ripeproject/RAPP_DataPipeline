
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <optional>


class cLidarMapConfigScan
{
public:
	cLidarMapConfigScan();
	~cLidarMapConfigScan() = default;

	void clear();

	bool isDirty() const;

	const std::string& getExperimentName() const;

	const std::optional<eKinematicModel>&   getKinematicModel() const;
	const std::optional<eOrientationModel>& getOrientationModel() const;

	const std::optional<double>& getMinDistance_m() const;
	const std::optional<double>& getMaxDistance_m() const;
	const std::optional<double>& getMinAzimuth_deg() const;
	const std::optional<double>& getMaxAzimuth_deg() const;
	const std::optional<double>& getMinAltitude_deg() const;
	const std::optional<double>& getMaxAltitude_deg() const;

	const std::optional<double>& getVx_mmps() const;
	const std::optional<double>& getVy_mmps() const;
	const std::optional<double>& getVz_mmps() const;

	const std::optional<double>& getSensorMountPitch_deg() const;
	const std::optional<double>& getSensorMountRoll_deg() const;
	const std::optional<double>& getSensorMountYaw_deg() const;

	const std::optional<double>& getSensorPitchOffset_deg() const;
	const std::optional<double>& getSensorRollOffset_deg() const;
	const std::optional<double>& getSensorYawOffset_deg() const;

	const std::optional<double>& getStartPitchOffset_deg() const;
	const std::optional<double>& getStartRollOffset_deg() const;
	const std::optional<double>& getStartYawOffset_deg() const;

	const std::optional<double>& getEndPitchOffset_deg() const;
	const std::optional<double>& getEndRollOffset_deg() const;
	const std::optional<double>& getEndYawOffset_deg() const;

	const std::optional<bool>&   getTranslateToGround() const;
	const std::optional<double>& getTranslateThreshold_pct() const;
	const std::optional<bool>&   getRotateToGround() const;
	const std::optional<double>& getRotateThreshold_pct() const;

	const std::optional<double>& getStart_X_m() const;
	const std::optional<double>& getStart_Y_m() const;
	const std::optional<double>& getStart_Z_m() const;

	const std::optional<double>& getEnd_X_m() const;
	const std::optional<double>& getEnd_Y_m() const;
	const std::optional<double>& getEnd_Z_m() const;


	void setExperimentName(const std::string& name);

	void setKinematicModel(const std::optional<eKinematicModel>& model);
	void setOrientationModel(const std::optional<eOrientationModel>& model);

	void setMinDistance_m(const std::optional<double>& );
	void setMaxDistance_m(const std::optional<double>&);
	void setMinAzimuth_deg(const std::optional<double>&);
	void setMaxAzimuth_deg(const std::optional<double>&);
	void setMinAltitude_deg(const std::optional<double>&);
	void setMaxAltitude_deg(const std::optional<double>&);

	void setVx_mmps(const std::optional<double>& );
	void setVy_mmps(const std::optional<double>& );
	void setVz_mmps(const std::optional<double>& );

	void setSensorMountPitch_deg(const std::optional<double>&);
	void setSensorMountRoll_deg(const std::optional<double>&);
	void setSensorMountYaw_deg(const std::optional<double>&);

	void setSensorPitchOffset_deg(const std::optional<double>&);
	void setSensorRollOffset_deg(const std::optional<double>&);
	void setSensorYawOffset_deg(const std::optional<double>&);

	void setStartPitchOffset_deg(const std::optional<double>&);
	void setStartRollOffset_deg(const std::optional<double>&);
	void setStartYawOffset_deg(const std::optional<double>&);

	void setEndPitchOffset_deg(const std::optional<double>&);
	void setEndRollOffset_deg(const std::optional<double>&);
	void setEndYawOffset_deg(const std::optional<double>&);

	void setTranslateToGround(const std::optional<bool>&);
	void setTranslateThreshold_pct(const std::optional<double>&);
	void setRotateToGround(const std::optional<bool>&);
	void setRotateThreshold_pct(const std::optional<double>&);

	void setStart_X_m(const std::optional<double>&);
	void setStart_Y_m(const std::optional<double>&);
	void setStart_Z_m(const std::optional<double>&);

	void setStart_X_mm(const std::optional<double>& x_mm);
	void setStart_Y_mm(const std::optional<double>& y_mm);
	void setStart_Z_mm(const std::optional<double>& z_mm);

	void setEnd_X_m(const std::optional<double>&);
	void setEnd_Y_m(const std::optional<double>&);
	void setEnd_Z_m(const std::optional<double>&);

	void setEnd_X_mm(const std::optional<double>& x_mm);
	void setEnd_Y_mm(const std::optional<double>& y_mm);
	void setEnd_Z_mm(const std::optional<double>& z_mm);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	std::string mExperimentName;

	std::optional<eKinematicModel>   mKinematicModel;
	std::optional<eOrientationModel> mOrientationModel;

	std::optional<double> mMinDistance_m;
	std::optional<double> mMaxDistance_m;
	std::optional<double> mMinAzimuth_deg;
	std::optional<double> mMaxAzimuth_deg;
	std::optional<double> mMinAltitude_deg;
	std::optional<double> mMaxAltitude_deg;

	std::optional<double> mVx_mmps;
	std::optional<double> mVy_mmps;
	std::optional<double> mVz_mmps;

	std::optional<double> mSensorMountPitch_deg;
	std::optional<double> mSensorMountRoll_deg;
	std::optional<double> mSensorMountYaw_deg;

	std::optional<double> mSensorPitchOffset_deg;
	std::optional<double> mSensorRollOffset_deg;
	std::optional<double> mSensorYawOffset_deg;

	std::optional<double> mStartPitchOffset_deg;
	std::optional<double> mStartRollOffset_deg;
	std::optional<double> mStartYawOffset_deg;

	std::optional<double> mEndPitchOffset_deg;
	std::optional<double> mEndRollOffset_deg;
	std::optional<double> mEndYawOffset_deg;

	std::optional<bool>   mTranslateToGround;
	std::optional<double> mTranslateThreshold_pct;
	std::optional<bool>   mRotateToGround;
	std::optional<double> mRotateThreshold_pct;

	std::optional<double> mStart_X_m;
	std::optional<double> mStart_Y_m;
	std::optional<double> mStart_Z_m;

	std::optional<double> mEnd_X_m;
	std::optional<double> mEnd_Y_m;
	std::optional<double> mEnd_Z_m;

	std::vector<rfm::sOrientationInterpPoint_t> mOrientationTable;

	friend class cLidarMapConfigFile;
};


