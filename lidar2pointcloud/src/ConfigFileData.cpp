
#include "ConfigFileData.hpp"
#include "StringUtils.hpp"

#include "Kinematics_Constant.hpp"
#include "Kinematics_Dolly.hpp"
#include "Kinematics_GPS.hpp"
#include "Kinematics_SLAM.hpp"

#include <nlohmann/json.hpp>

#include <fstream>


namespace
{
	eHeightAxis to_HeightAxis(char axis)
	{
		switch (toupper(axis))
		{
		case 'X':
			return eHeightAxis::X;
		case 'Y':
			return eHeightAxis::Y;
		case 'Z':
			return eHeightAxis::Z;
		}

		return eHeightAxis::Z;
	}
}

cConfigFileData::cConfigFileData(const std::string& filename)
	: mConfigFilename(filename)
{
}

cConfigFileData::~cConfigFileData()
{}

bool cConfigFileData::empty() const
{
	return mKinematicModels.empty();
}

void cConfigFileData::setDefaultMinRange_m(double minimumDistance_m)
{
	mDefaultOptions.minDistance_m = minimumDistance_m;
}

void cConfigFileData::setDefaultMaxRange_m(double maximumDistance_m)
{
	mDefaultOptions.maxDistance_m = maximumDistance_m;
}

void cConfigFileData::setDefaultValidRange_m(double minimumDistance_m, double maximumDistance_m)
{
	mDefaultOptions.minDistance_m = minimumDistance_m;
	mDefaultOptions.maxDistance_m = maximumDistance_m;
}

void cConfigFileData::setDefaultMinAzimuth_deg(double minimumAzimuth_deg)
{
	mDefaultOptions.minAzimuth_deg = minimumAzimuth_deg;
}

void cConfigFileData::setDefaultMaxAzimuth_deg(double maximumAzimuth_deg)
{
	mDefaultOptions.maxAzimuth_deg = maximumAzimuth_deg;
}

void cConfigFileData::setDefaultAzimuthWindow_deg(double minimumAzimuth_deg, double maximumAzimuth_deg)
{
	mDefaultOptions.minAzimuth_deg = minimumAzimuth_deg;
	mDefaultOptions.maxAzimuth_deg = maximumAzimuth_deg;
}

void cConfigFileData::setDefaultMinAltitude_deg(double minimumAltitude_deg)
{
	mDefaultOptions.minAltitude_deg = minimumAltitude_deg;
}

void cConfigFileData::setDefaultMaxAltitude_deg(double maximumAltitude_deg)
{
	mDefaultOptions.maxAltitude_deg = maximumAltitude_deg;
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


void cConfigFileData::setDefaultInitialPosition_m(double x_m, double y_m, double z_m)
{
	mDefault_X_m = x_m;
	mDefault_Y_m = y_m;
	mDefault_Z_m = z_m;
}

void cConfigFileData::setDefaultSpeeds_mmmps(double vx_mmps, double vy_mmps, double vz_mmps)
{
	mDefault_Vx_mmps = vx_mmps;
	mDefault_Vy_mmps = vy_mmps;
	mDefault_Vz_mmps = vz_mmps;
}

void cConfigFileData::setDefaultHeight_m(double height_m, eHeightAxis axis)
{
	mDefault_Height_m = height_m;
	mDefault_HeightAxis = axis;
}

void cConfigFileData::setDefaultPitch(double pitch_deg)
{
	mDefault_SensorPitch_deg = pitch_deg;
}

void cConfigFileData::setDefaultRoll(double roll_deg)
{
	mDefault_SensorRoll_deg = roll_deg;
}

void cConfigFileData::setDefaultYaw(double yaw_deg)
{
	mDefault_SensorYaw_deg = yaw_deg;
}

void cConfigFileData::setDefaultOrientation(double pitch_deg, double roll_deg, double yaw_deg, bool rotateToSEU)
{
	mDefault_SensorPitch_deg = pitch_deg;
	mDefault_SensorRoll_deg = roll_deg;
	mDefault_SensorYaw_deg = yaw_deg;
	mDefault_RotateToSEU = rotateToSEU;
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

	/*** Load Defaults ***/

	if (configDoc.contains("defaults"))
	{
		auto defaults = configDoc["defaults"];

		if (defaults.contains("dolly speeds"))
		{
			auto speeds = defaults["dolly speeds"];

			if (speeds.contains("Vx (mm/s)"))
				mDefault_Vx_mmps = speeds["Vx (mm/s)"];

			if (speeds.contains("Vy (mm/s)"))
				mDefault_Vy_mmps = speeds["Vy (mm/s)"];

			if (speeds.contains("Vz (mm/s)"))
				mDefault_Vz_mmps = speeds["Vz (mm/s)"];
		}

		if (defaults.contains("dolly height"))
		{
			auto height = defaults["dolly height"];

			mDefault_Height_m = height["height (m)"];
			std::string axis = height["height axis"];
			mDefault_HeightAxis = to_HeightAxis(axis[0]);
		}

		if (defaults.contains("sensor orientation"))
		{
			auto orientation = defaults["sensor orientation"];

			if (orientation.contains("pitch (deg)"))
				mDefault_SensorPitch_deg = orientation["pitch (deg)"];

			if (orientation.contains("roll (deg)"))
				mDefault_SensorRoll_deg = orientation["roll (deg)"];

			if (orientation.contains("yaw (deg)"))
				mDefault_SensorYaw_deg = orientation["yaw (deg)"];

			if (orientation.contains("rotate to SEU"))
				mDefault_RotateToSEU = orientation["rotate to SEU"];
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

		if (defaults.contains("options"))
		{
			auto options = defaults["options"];

			if (options.contains("aggregatePointCloud"))
				mDefaultOptions.aggregatePointCloud = options["aggregatePointCloud"];

			if (options.contains("saveReducedPointCloud"))
				mDefaultOptions.saveReducedPointCloud = options["saveReducedPointCloud"];
		}
	}

	return true;
}

bool cConfigFileData::loadKinematicModels()
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

	if (configDoc.contains("experiments"))
	{
		auto experiments = configDoc["experiments"];

		for (auto experiment : experiments)
		{
			std::string name = safeFilename(experiment["experiment_name"]);
			std::string model = experiment["kinematic model"];

			double sensorPitch_deg = mDefault_SensorPitch_deg;
			double sensorRoll_deg = mDefault_SensorRoll_deg;
			double sensorYaw_deg = mDefault_SensorYaw_deg;
			bool rotateToSEU = mDefault_RotateToSEU;

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

				double X_m = mDefault_X_m;
				double Y_m = mDefault_Y_m;
				double Z_m = mDefault_Z_m;

				double Vx_mmps = mDefault_Vx_mmps;
				double Vy_mmps = mDefault_Vy_mmps;
				double Vz_mmps = mDefault_Vz_mmps;

				double height_m = mDefault_Height_m;
				auto heightAxis = mDefault_HeightAxis;

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
					heightAxis = to_HeightAxis(axis[0]);
				}

				switch (heightAxis)
				{
				case eHeightAxis::X:
					km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::X);
					km->setInitialPosition_m(height_m, Y_m, Z_m);
					break;
				case eHeightAxis::Y:
					km->setHeightAxis(cKinematics_Constant::eHEIGHT_AXIS::Y);
					km->setInitialPosition_m(X_m, height_m, Z_m);
					break;
				case eHeightAxis::Z:
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

	return true;
}

