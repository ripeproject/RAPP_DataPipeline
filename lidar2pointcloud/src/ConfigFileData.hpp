
#pragma once

#include <string>
#include <map>
#include <memory>

// Forward declarations
class cKinematics;

struct sOptions_t
{
	double minDistance_m = 1.0;
	double maxDistance_m = 40.0;
	double minAzimuth_deg = 0.0;
	double maxAzimuth_deg = 360.0;
	double minAltitude_deg = -25.0;
	double maxAltitude_deg = 25.0;

	bool aggregatePointCloud = true;
	bool saveReducedPointCloud = false;
};


class cConfigFileData
{
public:
	explicit cConfigFileData(const std::string& filename);
	~cConfigFileData();

	/**
	 * Load any default parameters from the configuration file.
	 *
	 * The defaults can be overridden using the setDefault<parameters> 
	 * methods.  The defaults can be set by using the set defaults methods
	 * instead of call loadDefaults.
	 */
	bool loadDefaults();

	/**
	 * Load the kinematic models from the configuration file.
	 *
	 * This method should be call after setting all default values.
	 */
	bool loadKinematicModels();

	bool empty() const;

	void setDefaultValidRange_m(double minimumDistance_m, double maximumDistance_m);
	void setDefaultAzimuthWindow_deg(double minimumAzimuth_deg, double maximumAzimuth_deg);
	void setDefaultAltitudeWindow_deg(double minimumAltitude_deg, double maximumAltitude_deg);

	void setDefaultAggregatePointCloud(bool aggregatedPointCloud);
	void setDefaultReducedPointCloud(bool reducedPointCloud);

	void setDefaultInitialPosition_m(double x_m, double y_m, double z_m);
	void setDefaultSpeeds_mmmps(double vx_mmps, double vy_mmps, double vz_mmps);
	void setDefaultHeight_m(double height_m, char axis);

	sOptions_t getOptions(const std::string& experiment_filename);
	std::unique_ptr<cKinematics> getModel(const std::string& experiment_filename);

private:
	void load(const std::string& filename);

private:
	std::string mConfigFilename;

	sOptions_t mDefaultOptions;

	double mDefault_X_m = 0.0;
	double mDefault_Y_m = 0.0;
	double mDefault_Z_m = 0.0;

	double mDefault_Vx_mmps = 0.0;
	double mDefault_Vy_mmps = 0.0;
	double mDefault_Vz_mmps = 0.0;

	double mDefault_Height_m = 0.0;
	char mDefault_HeightAxis = 'Z';

	double mDefault_SensorPitch_deg = -90.0;
	double mDefault_SensorRoll_deg = 0.0;
	double mDefault_SensorYaw_deg = 270.0;
	bool mDefault_RotateToSEU = true;

	std::map<std::string, sOptions_t> mOptions;

	std::map<std::string, std::unique_ptr<cKinematics>> mKinematicModels;
};