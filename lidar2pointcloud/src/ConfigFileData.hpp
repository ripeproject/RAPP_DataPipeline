
#pragma once

#include <string>
#include <map>
#include <memory>

// Forward declarations
class cKinematics;


class cConfigFileData
{
public:
	explicit cConfigFileData(const std::string& filename);
	~cConfigFileData();

	bool empty() const;

	std::unique_ptr<cKinematics> getModel(const std::string& experiment_filename);

private:
	void load(const std::string& filename);

private:
	double mMinDistance_m = 1.0;
	double mMaxDistance_m = 40.0;
	double mMinAzimuth_deg = 135.0;
	double mMaxAzimuth_deg = 225.0;
	double mMinAltitude_deg = -25.0;
	double mMaxAltitude_deg = 25.0;

	bool mAggregatePointCloud = true;
	bool mSaveReducedPointCloud = false;

	std::map<std::string, std::unique_ptr<cKinematics>> mKinematicModels;
};