
#include "ConfigFileData.hpp"
#include "StringUtils.hpp"

#include "Kinematics_Constant.hpp"
#include "Kinematics_Dolly.hpp"
#include "Kinematics_GPS.hpp"
#include "Kinematics_SLAM.hpp"

#include <nlohmann/json.hpp>

#include <fstream>


cConfigFileData::cConfigFileData(const std::string& filename)
	: mConfigFilename(filename)
{
	load(filename);
}

cConfigFileData::~cConfigFileData()
{}

bool cConfigFileData::empty() const
{
	return mKinematicModels.empty();
}

void cConfigFileData::setDefaultValidRange_m(double minimumDistance_m, double maximumDistance_m)
{
	mDefaultOptions.minDistance_m = minimumDistance_m;
	mDefaultOptions.maxDistance_m = maximumDistance_m;
}

void cConfigFileData::setDefaultAzimuthWindow_deg(double minimumAzimuth_deg, double maximumAzimuth_deg)
{
	mDefaultOptions.minAzimuth_deg = minimumAzimuth_deg;
	mDefaultOptions.maxAzimuth_deg = maximumAzimuth_deg;
}

void cConfigFileData::setDefaultAltitudeWindow_deg(double minimumAltitude_deg, double maximumAltitude_deg)
{
	mDefaultOptions.minAltitude_deg = minimumAltitude_deg;
	mDefaultOptions.maxAltitude_deg = maximumAltitude_deg;
}

void cConfigFileData::setDefaultAggregatePointCloud(bool aggregatedPointCloud)
{
	mDefaultOptions.aggregatePointCloud = aggregatedPointCloud;
}

void cConfigFileData::setDefaultReducedPointCloud(bool reducedPointCloud)
{
	mDefaultOptions.saveReducedPointCloud = reducedPointCloud;
}

sOptions_t cConfigFileData::getOptions(const std::string& experiment_filename)
{
	using namespace nStringUtils;

	auto in = removeMeasurementTimestamp(experiment_filename);
	auto filename = safeFilename(in.filename);

	auto it = mOptions.find(filename);

	if (it == mOptions.end())
		return mDefaultOptions;

	return it->second;
}


std::unique_ptr<cKinematics> cConfigFileData::getModel(const std::string& experiment_filename)
{
	using namespace nStringUtils;

	auto in = removeMeasurementTimestamp(experiment_filename);
	auto filename = safeFilename(in.filename);

	auto it = mKinematicModels.find(filename);

	if (it == mKinematicModels.end())
		return std::unique_ptr<cKinematics>();

	return std::move(it->second);
}


bool cConfigFileData::loadDefaults()
{
	using namespace nStringUtils;

	if (mConfigFilename.empty()) return false;

	std::ifstream in;
	in.open(mConfigFilename);
	if (!in.is_open())
		return false;

	nlohmann::json configDoc;
	try
	{
		in >> configDoc;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		auto msg = e.what();
		return false;
	}
	catch (const std::exception& e)
	{
		return false;
	}

	double default_X_m = 0.0;
	double default_Y_m = 0.0;
	double default_Z_m = 0.0;

	double default_Vx_mmps = 0.0;
	double default_Vy_mmps = 0.0;
	double default_Vz_mmps = 0.0;

	double default_Height_m = 0.0;
	char default_HeightAxis = 'Z';

	double default_SensorPitch_deg = -90.0;
	double default_SensorRoll_deg = 0.0;
	double default_SensorYaw_deg = 270.0;
	bool default_RotateToSEU = true;

	/*** Load Defaults ***/

	if (configDoc.contains("defaults"))
	{
		auto defaults = configDoc["defaults"];

		if (defaults.contains("dolly speeds"))
		{
			auto speeds = defaults["dolly speeds"];

			if (speeds.contains("Vx (mm/s)"))
				default_Vx_mmps = speeds["Vx (mm/s)"];

			if (speeds.contains("Vy (mm/s)"))
				default_Vy_mmps = speeds["Vy (mm/s)"];

			if (speeds.contains("Vz (mm/s)"))
				default_Vz_mmps = speeds["Vz (mm/s)"];
		}

		if (defaults.contains("dolly height"))
		{
			auto height = defaults["dolly height"];

			default_Height_m = height["height (m)"];
			std::string axis = height["height axis"];
			default_HeightAxis = toupper(axis[0]);
		}

		if (defaults.contains("sensor orientation"))
		{
			auto orientation = defaults["sensor orientation"];

			if (orientation.contains("pitch (deg)"))
				default_SensorPitch_deg = orientation["pitch (deg)"];

			if (orientation.contains("roll (deg)"))
				default_SensorRoll_deg = orientation["roll (deg)"];

			if (orientation.contains("yaw (deg)"))
				default_SensorYaw_deg = orientation["yaw (deg)"];

			if (orientation.contains("rotate to SEU"))
				default_RotateToSEU = orientation["rotate to SEU"];
		}

		if (defaults.contains("sensor limits"))
		{
			auto limits = defaults["sensor limits"];

			if (limits.contains("min distance (m)"))
				mDefaultOptions.maxDistance_m = limits["min distance (m)"];

			if (limits.contains("max distance (m)"))
				mDefaultOptions.maxDistance_m = limits["max distance (m)"];

			if (limits.contains("min azimuth (deg)"))
				mDefaultOptions.minAzimuth_deg = limits["min azimuth (deg)"];

			if (limits.contains("max azimuth (deg)"))
				mDefaultOptions.maxAzimuth_deg = limits["max azimuth (deg)"];

			if (limits.contains("min altitude (deg)"))
				mDefaultOptions.minAltitude_deg = limits["min altitude (deg)"];

			if (limits.contains("max altitude (deg)"))
				mDefaultOptions.maxAltitude_deg = limits["max altitude (deg)"];
		}

		/*
				"options":
				{
					"aggregatePointCloud" : true,
					"saveReducedPointCloud" : false
				}
		*/
	}

	return true;
}


void cConfigFileData::load(const std::string& filename)
{
	using namespace nStringUtils;

	std::ifstream in;
	in.open(filename);
	if (!in.is_open())
		return;

	nlohmann::json configDoc;
	try
	{
		in >> configDoc;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		auto msg = e.what();
		return;
	}
	catch (const std::exception& e)
	{
		return;
	}

	double default_X_m = 0.0;
	double default_Y_m = 0.0;
	double default_Z_m = 0.0;

	double default_Vx_mmps = 0.0;
	double default_Vy_mmps = 0.0;
	double default_Vz_mmps = 0.0;

	double default_Height_m = 0.0;
	char default_HeightAxis = 'Z';

	double default_SensorPitch_deg = -90.0;
	double default_SensorRoll_deg = 0.0;
	double default_SensorYaw_deg = 270.0;
	bool default_RotateToSEU = true;

	/*** Load Defaults ***/

	if (configDoc.contains("defaults"))
	{
		auto defaults = configDoc["defaults"];

		if (defaults.contains("dolly speeds"))
		{
			auto speeds = defaults["dolly speeds"];

			if (speeds.contains("Vx (mm/s)"))
				default_Vx_mmps = speeds["Vx (mm/s)"];

			if (speeds.contains("Vy (mm/s)"))
				default_Vy_mmps = speeds["Vy (mm/s)"];

			if (speeds.contains("Vz (mm/s)"))
				default_Vz_mmps = speeds["Vz (mm/s)"];
		}

		if (defaults.contains("dolly height"))
		{
			auto height = defaults["dolly height"];

			default_Height_m = height["height (m)"];
			std::string axis = height["height axis"];
			default_HeightAxis = toupper(axis[0]);
		}

		if (defaults.contains("sensor orientation"))
		{
			auto orientation = defaults["sensor orientation"];

			if (orientation.contains("pitch (deg)"))
				default_SensorPitch_deg = orientation["pitch (deg)"];

			if (orientation.contains("roll (deg)"))
				default_SensorRoll_deg = orientation["roll (deg)"];

			if (orientation.contains("yaw (deg)"))
				default_SensorYaw_deg = orientation["yaw (deg)"];

			if (orientation.contains("rotate to SEU"))
				default_RotateToSEU = orientation["rotate to SEU"];
		}

		if (defaults.contains("sensor limits"))
		{
			auto limits = defaults["sensor limits"];

			if (limits.contains("min distance (m)"))
				mDefaultOptions.maxDistance_m = limits["min distance (m)"];

			if (limits.contains("max distance (m)"))
				mDefaultOptions.maxDistance_m = limits["max distance (m)"];

			if (limits.contains("min azimuth (deg)"))
				mDefaultOptions.minAzimuth_deg = limits["min azimuth (deg)"];

			if (limits.contains("max azimuth (deg)"))
				mDefaultOptions.maxAzimuth_deg = limits["max azimuth (deg)"];

			if (limits.contains("min altitude (deg)"))
				mDefaultOptions.minAltitude_deg = limits["min altitude (deg)"];

			if (limits.contains("max altitude (deg)"))
				mDefaultOptions.maxAltitude_deg = limits["max altitude (deg)"];
		}

/*
		"options":
		{
			"aggregatePointCloud" : true,
			"saveReducedPointCloud" : false
		}
*/
	}

	if (configDoc.contains("experiments"))
	{
		auto experiments = configDoc["experiments"];

		for (auto experiment : experiments)
		{
			std::string name = safeFilename(experiment["experiment_name"]);
			std::string model = experiment["kinematic model"];

			double sensorPitch_deg = -default_SensorPitch_deg;
			double sensorRoll_deg = default_SensorRoll_deg;
			double sensorYaw_deg = default_SensorYaw_deg;
			bool rotateToSEU = default_RotateToSEU;

			if (experiment.contains("sensor orientation"))
			{
				auto orientation = experiment["sensor orientation"];

				if (orientation.contains("pitch (deg)"))
					sensorPitch_deg = orientation["pitch (deg)"];

				if (orientation.contains("roll (deg)"))
					sensorRoll_deg = orientation["roll (deg)"];

				if (orientation.contains("yaw (deg)"))
					sensorYaw_deg = orientation["yaw (deg)"];

				if (orientation.contains("rotate to SEU"))
					rotateToSEU = orientation["rotate to SEU"];
			}

			if (model == "constant")
			{

				double X_m = default_X_m;
				double Y_m = default_Y_m;
				double Z_m = default_Z_m;

				double Vx_mmps = default_Vx_mmps;
				double Vy_mmps = default_Vy_mmps;
				double Vz_mmps = default_Vz_mmps;

				double height_m = default_Height_m;
				char heightAxis = default_HeightAxis;

				if (experiment.contains("start position"))
				{
					auto pos = experiment["start position"];

					if (experiment.contains("x (m)"))
						X_m = experiment["x (m)"];

					if (experiment.contains("y (m)"))
						Y_m = experiment["y (m)"];

					if (experiment.contains("z (m)"))
						Z_m = experiment["z (m)"];
				}

				if (experiment.contains("Vx (mm/s)"))
					Vx_mmps = experiment["Vx (mm/s)"];

				if (experiment.contains("Vy (mm/s)"))
					Vy_mmps = experiment["Vy (mm/s)"];

				if (experiment.contains("Vz (mm/s)"))
					Vz_mmps = experiment["Vz (mm/s)"];

				auto km = std::make_unique<cKinematics_Constant>(Vx_mmps, Vy_mmps, Vz_mmps);

				if (experiment.contains("height (m)"))
				{
					height_m = experiment["height (m)"];
					std::string axis = experiment["height axis"];
					heightAxis = toupper(axis[0]);
				}

				switch (heightAxis)
				{
				case 'X':
					km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::X);
					km->setInitialPosition_m(height_m, Y_m, Z_m);
					break;
				case 'Y':
					km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::Y);
					km->setInitialPosition_m(X_m, height_m, Z_m);
					break;
				case 'Z':
					km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::Z);
					km->setInitialPosition_m(X_m, Y_m, height_m);
					break;
				}

				km->rotateToSEU(rotateToSEU);
				km->setSensorOrientation(sensorYaw_deg, sensorPitch_deg, sensorRoll_deg);

				mKinematicModels[name] = std::move(km);

			}
			else if (model == "dolly")
			{
				auto km = std::make_unique<cKinematics_Dolly>();
//				km->useImuData(mpKM_DO_UseImuData->GetValue());
//				km->averageImuData(mpKM_DO_AverageImuData->GetValue());
//				km->setSensorPitchOffset_deg(mKM_DO_PitchOffset_deg);
//				km->setSensorRollOffset_deg(mKM_DO_RollOffset_deg);

				km->rotateToSEU(rotateToSEU);
				km->setSensorOrientation(sensorYaw_deg, sensorPitch_deg, sensorRoll_deg);

				mKinematicModels[name] = std::move(km);
			}
			else if (model == "gps")
			{
				auto km = std::make_unique<cKinematics_GPS>();

				km->rotateToSEU(rotateToSEU);
				km->setSensorOrientation(sensorYaw_deg, sensorPitch_deg, sensorRoll_deg);

				mKinematicModels[name] = std::move(km);
			}
			else if (model == "slam")
			{
				auto km = std::make_unique<cKinematics_SLAM>();

				km->rotateToSEU(rotateToSEU);
				km->setSensorOrientation(sensorYaw_deg, sensorPitch_deg, sensorRoll_deg);

				mKinematicModels[name] = std::move(km);
			}
		}
	}
}

