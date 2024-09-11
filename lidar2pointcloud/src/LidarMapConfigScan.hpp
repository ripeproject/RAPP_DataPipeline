
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

	const std::optional<eKinematicModel>&			getKinematicModel() const;
	const std::optional<eOrientationModel>&			getOrientationModel() const;
	const std::optional<eTranslateToGroundModel>&	getTranslateToGroundModel() const;
	const std::optional<eRotateToGroundModel>&		getRotateToGroundModel() const;

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

	const std::vector<rfm::sDollyOrientationInterpPoint_t>& getOrientationTable() const;

	const std::optional<bool>&   getTranslateToGround() const;
	const std::optional<double>& getTranslateDistance_m() const;
	const std::optional<double>& getTranslateThreshold_pct() const;

	const std::vector<rfm::sPointCloudTranslationInterpPoint_t>& getTranslateTable() const;

	const std::optional<bool>&   getRotateToGround() const;
	const std::optional<double>& getRotatePitch_deg() const;
	const std::optional<double>& getRotateRoll_deg() const;
	const std::optional<double>& getRotateThreshold_pct() const;

	const std::vector<rfm::sPointCloudRotationInterpPoint_t>& getRotateTable() const;


	const std::optional<double>& getStart_X_m() const;
	const std::optional<double>& getStart_Y_m() const;
	const std::optional<double>& getStart_Z_m() const;

	const std::optional<double>& getEnd_X_m() const;
	const std::optional<double>& getEnd_Y_m() const;
	const std::optional<double>& getEnd_Z_m() const;


	void setExperimentName(const std::string& name);

	void setKinematicModel(const std::optional<eKinematicModel>& model);
	void setOrientationModel(const std::optional<eOrientationModel>& model);
	void setTranslateToGroundModel(const std::optional<eTranslateToGroundModel>& model);
	void setRotateToGroundModel(const std::optional<eRotateToGroundModel>& model);

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

	void setVx_mmps(const std::optional<double>& );
	void setVy_mmps(const std::optional<double>& );
	void setVz_mmps(const std::optional<double>& );

	void resetSensorMountPitch_deg();
	void resetSensorMountRoll_deg();
	void resetSensorMountYaw_deg();

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

	void clearOrientationTable();
	void setOrientationTable(const std::vector<rfm::sDollyOrientationInterpPoint_t>& table);

	void setTranslateToGround(const std::optional<bool>&);
	void setTranslateDistance_m(const std::optional<double>& dist_m);
	void setTranslateThreshold_pct(const std::optional<double>& threshold_pct);

	void clearTranslateTable();
	void setTranslateTable(const std::vector<rfm::sPointCloudTranslationInterpPoint_t>& table);

	void setRotateToGround(const std::optional<bool>&);
	void setRotateAngles_deg(const std::optional<double>& pitch_deg, const std::optional<double>& roll_deg);
	void setRotateThreshold_pct(const std::optional<double>&);

	void clearRotateTable();
	void setRotateTable(const std::vector<rfm::sPointCloudRotationInterpPoint_t>& table);

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
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	void reset(std::optional<double>& var);

private:
	bool mDirty = false;

	std::string mExperimentName;

	std::optional<eKinematicModel>			mKinematicModel;
	std::optional<eOrientationModel>		mOrientationModel;
	std::optional<eTranslateToGroundModel>	mTranslateToGroundModel;
	std::optional<eRotateToGroundModel>		mRotateToGroundModel;

	// Sensor Limits
	std::optional<double> mMinDistance_m;
	std::optional<double> mMaxDistance_m;
	std::optional<double> mMinAzimuth_deg;
	std::optional<double> mMaxAzimuth_deg;
	std::optional<double> mMinAltitude_deg;
	std::optional<double> mMaxAltitude_deg;

	// Dolly - Sensor Mounting Parameters
	std::optional<double> mSensorMountPitch_deg;
	std::optional<double> mSensorMountRoll_deg;
	std::optional<double> mSensorMountYaw_deg;

	// Dolly Translation Model - Constant: Speeds
	std::optional<double> mVx_mmps;
	std::optional<double> mVy_mmps;
	std::optional<double> mVz_mmps;

	// Dolly Rotation Model - Constant: Angles
	std::optional<double> mSensorPitchOffset_deg;
	std::optional<double> mSensorRollOffset_deg;
	std::optional<double> mSensorYawOffset_deg;

	// Dolly Rotation Model - Linear: Start and Stop Angles
	std::optional<double> mStartPitchOffset_deg;
	std::optional<double> mStartRollOffset_deg;
	std::optional<double> mStartYawOffset_deg;

	std::optional<double> mEndPitchOffset_deg;
	std::optional<double> mEndRollOffset_deg;
	std::optional<double> mEndYawOffset_deg;

	// Dolly Rotation Model - Intrep Table: Data Points
	std::vector<rfm::sDollyOrientationInterpPoint_t> mOrientationTable;

	// Translate Model Parameters
	std::optional<bool>   mTranslateToGround;

	// Translate Model - Constants: Distance
	std::optional<double> mTranslateDistance_m;

	// Translate Model - Fit to Ground: Threshold Level
	std::optional<double> mTranslateThreshold_pct;

	// Translate Model - Intrep Table: Data Points
	std::vector<rfm::sPointCloudTranslationInterpPoint_t> mTranslateTable;

	// Rotate Model Parameters
	std::optional<bool>   mRotateToGround;

	// Rotate Model - Constants: Pitch and Roll
	std::optional<double> mRotatePitch_deg;
	std::optional<double> mRotateRoll_deg;

	// Rotate Model - Fit to Ground: Threshold Level
	std::optional<double> mRotateThreshold_pct;

	// Rotate Model - Intrep Table: Data Points
	std::vector<rfm::sPointCloudRotationInterpPoint_t> mRotateTable;

	std::optional<double> mStart_X_m;
	std::optional<double> mStart_Y_m;
	std::optional<double> mStart_Z_m;

	std::optional<double> mEnd_X_m;
	std::optional<double> mEnd_Y_m;
	std::optional<double> mEnd_Z_m;

	friend class cLidarMapConfigFile;
};


