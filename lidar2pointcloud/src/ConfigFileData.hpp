
#pragma once

#include <string>
#include <map>
#include <memory>
#include <optional>

// Forward declarations
class cKinematics;

namespace nConfigFileData
{
	struct sParameters_t
	{
		double minDistance_m = 1.0;
		double maxDistance_m = 40.0;
		double minAzimuth_deg = 135.0;
		double maxAzimuth_deg = 225.0;
		double minAltitude_deg = -10.0;
		double maxAltitude_deg = 10.0;

		double startX_m = 0.0;
		double startY_m = 0.0;
		double startZ_m = 0.0;

		double endX_m = 0.0;
		double endY_m = 0.0;
		double endZ_m = 0.0;

		double Vx_mmps = 0.0;
		double Vy_mmps = 0.0;
		double Vz_mmps = 0.0;

		double sensorMountPitch_deg = -90.0;
		double sensorMountRoll_deg  = 0.0;
		double sensorMountYaw_deg   = 90.0;

		double sensorPitchOffset_deg = 0.0;
		double sensorRollOffset_deg  = 0.0;
		double sensorYawOffset_deg   = 0.0;

		double startPitchOffset_deg = 0.0;
		double startRollOffset_deg = 0.0;
		double startYawOffset_deg = 0.0;

		double endPitchOffset_deg = 0.0;
		double endRollOffset_deg = 0.0;
		double endYawOffset_deg = 0.0;

		bool   translateToGround = true;
		double translateThreshold_pct = 1.0;
		bool   rotateToGround = true;
		double rotateThreshold_pct = 1.0;
	};
}


class cConfigFileData_
{
public:
	explicit cConfigFileData_(const std::string& filename);
	~cConfigFileData_();

	/**
	 * Load any parameters from the configuration file.
	 */
	bool load();

	bool empty() const;

	bool saveCompactPointCloud() const;
	bool saveFrameIds() const;
	bool savePixelInfo() const;

	bool savePlyFiles() const;
	bool plyUseBinaryFormat() const;

	std::optional<nConfigFileData::sParameters_t> getParameters(const std::string& experiment_filename);

private:
	std::string mConfigFilename;

	bool mSaveCompactPointCloud = false;
	bool mSaveFrameIDs = false;
	bool mSavePixelInfo = false;

	bool mSavePlyFiles = false;
	bool mPlyUseBinaryFormat = false;

	std::map<std::string, nConfigFileData::sParameters_t>	mParameters;
};