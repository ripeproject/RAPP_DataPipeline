
#include "ConfigFileData.hpp"
#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>


extern void console_message(const std::string& msg);


cConfigFileData_::cConfigFileData_(const std::string& filename)
	: mConfigFilename(filename)
{
}

cConfigFileData_::~cConfigFileData_()
{}

bool cConfigFileData_::empty() const
{
	return mParameters.empty();
}

bool cConfigFileData_::saveCompactPointCloud() const
{
	return mSaveCompactPointCloud;
}

bool cConfigFileData_::saveFrameIds() const
{
	return mSaveFrameIDs;
}

bool cConfigFileData_::savePixelInfo() const
{
	return mSavePixelInfo;
}

bool cConfigFileData_::savePlyFiles() const
{
	return mSavePlyFiles;
}

bool cConfigFileData_::plyUseBinaryFormat() const
{
	return mPlyUseBinaryFormat;
}

std::optional<nConfigFileData::sParameters_t> cConfigFileData_::getParameters(const std::string& experiment_filename)
{
	using namespace nStringUtils;

	auto in = removeMeasurementTimestamp(experiment_filename);
	auto filename = safeFilename(in.filename);

	auto it = mParameters.find(filename);

	if (it == mParameters.end())
		return std::optional<nConfigFileData::sParameters_t>();

	return it->second;
}

bool cConfigFileData_::load()
{
	using namespace nConfigFileData;
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
		console_message(e.what());
		return false;
	}
	catch (const std::exception& e)
	{
		console_message(e.what());
		return false;
	}

	/*** Load Options ***/
	try
	{
		if (configDoc.contains("options"))
		{
			auto options = configDoc["options"];

			if (options.contains("save compact data file"))
				mSaveCompactPointCloud = options["save compact data file"];

			if (options.contains("save lidar frame ids"))
				mSaveFrameIDs = options["save lidar frame ids"];

			if (options.contains("save lidar pixel info"))
				mSavePixelInfo = options["save lidar pixel info"];

			if (options.contains("create plys"))
				mSavePlyFiles = options["create plys"];

			if (options.contains("plys use binary format"))
				mPlyUseBinaryFormat = options["plys use binary format"];
		}
	}
	catch (const nlohmann::json::invalid_iterator& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::type_error& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::out_of_range& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::other_error& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::exception& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const std::exception& e)
	{
		console_message(e.what());
		return false;
	}


	/*** Load Defaults ***/
	sParameters_t defaultParameters;
	double height_m = 0.0;

	try
	{
		if (configDoc.contains("defaults"))
		{
			auto defaults = configDoc["defaults"];

			if (defaults.contains("dolly speeds"))
			{
				auto speeds = defaults["dolly speeds"];

				if (speeds.contains("Vx (mm/s)"))
					defaultParameters.Vx_mmps = speeds["Vx (mm/s)"];

				if (speeds.contains("Vy (mm/s)"))
					defaultParameters.Vy_mmps = speeds["Vy (mm/s)"];

				if (speeds.contains("Vz (mm/s)"))
					defaultParameters.Vz_mmps = speeds["Vz (mm/s)"];
			}

			if (defaults.contains("dolly height"))
			{
				auto height = defaults["dolly height"];

				height_m = height["height (m)"];
			}

			if (defaults.contains("sensor mount orientation"))
			{
				auto orientation = defaults["sensor mount orientation"];

				if (orientation.contains("pitch (deg)"))
					defaultParameters.sensorMountPitch_deg = orientation["pitch (deg)"];

				if (orientation.contains("roll (deg)"))
					defaultParameters.sensorMountRoll_deg = orientation["roll (deg)"];

				if (orientation.contains("yaw (deg)"))
					defaultParameters.sensorMountYaw_deg = orientation["yaw (deg)"];
			}

			if (defaults.contains("sensor limits"))
			{
				auto limits = defaults["sensor limits"];

				if (limits.contains("min distance (m)"))
					defaultParameters.maxDistance_m = limits["min distance (m)"];

				if (limits.contains("max distance (m)"))
					defaultParameters.maxDistance_m = limits["max distance (m)"];

				if (limits.contains("min azimuth (deg)"))
					defaultParameters.minAzimuth_deg = limits["min azimuth (deg)"];

				if (limits.contains("max azimuth (deg)"))
					defaultParameters.maxAzimuth_deg = limits["max azimuth (deg)"];

				if (limits.contains("min altitude (deg)"))
					defaultParameters.minAltitude_deg = limits["min altitude (deg)"];

				if (limits.contains("max altitude (deg)"))
					defaultParameters.maxAltitude_deg = limits["max altitude (deg)"];
			}

			if (defaults.contains("options"))
			{
				auto options = defaults["options"];

				if (options.contains("translate to ground"))
					defaultParameters.translateToGround = options["translate to ground"];

				if (options.contains("translation threshold (%)"))
					defaultParameters.translateThreshold_pct = options["translation threshold (%)"];

				if (options.contains("rotate to ground"))
					defaultParameters.rotateToGround = options["rotate to ground"];

				if (options.contains("rotation threshold (%)"))
					defaultParameters.rotateThreshold_pct = options["rotation threshold (%)"];
			}
		}

		if (configDoc.contains("experiments"))
		{
			auto experiments = configDoc["experiments"];

			for (auto experiment : experiments)
			{
				sParameters_t parameters = defaultParameters;

				std::string name;

				if (experiment.contains("experiment_name"))
					name = safeFilename(experiment["experiment_name"]);
				else
					name = safeFilename(experiment["experiment name"]);

				if (experiment.contains("sensor mount orientation"))
				{
					auto orientation = experiment["sensor mount orientation"];

					if (orientation.contains("pitch (deg)"))
						parameters.sensorMountPitch_deg = orientation["pitch (deg)"];

					if (orientation.contains("roll (deg)"))
						parameters.sensorMountRoll_deg = orientation["roll (deg)"];

					if (orientation.contains("yaw (deg)"))
						parameters.sensorMountYaw_deg = orientation["yaw (deg)"];
				}

				parameters.startZ_m = height_m;

				if (experiment.contains("start position"))
				{
					auto pos = experiment["start position"];

					if (pos.contains("x (m)"))
						parameters.startX_m = pos["x (m)"];

					if (pos.contains("y (m)"))
						parameters.startY_m = pos["y (m)"];

					if (pos.contains("z (m)"))
						parameters.startZ_m = pos["z (m)"];
				}

				parameters.endX_m = parameters.startX_m;
				parameters.endY_m = parameters.startY_m;
				parameters.endZ_m = parameters.startZ_m;

				if (experiment.contains("end position"))
				{
					auto pos = experiment["end position"];

					if (pos.contains("x (m)"))
						parameters.endX_m = pos["x (m)"];

					if (pos.contains("y (m)"))
						parameters.endY_m = pos["y (m)"];

					if (pos.contains("z (m)"))
						parameters.endZ_m = pos["z (m)"];
				}

				if (experiment.contains("Vx (mm/s)"))
					parameters.Vx_mmps = experiment["Vx (mm/s)"];

				if (experiment.contains("Vy (mm/s)"))
					parameters.Vy_mmps = experiment["Vy (mm/s)"];

				if (experiment.contains("Vz (mm/s)"))
					parameters.Vz_mmps = experiment["Vz (mm/s)"];

				if (experiment.contains("sensor start orientation offset"))
				{
					auto orientation = experiment["sensor start orientation offset"];

					if (orientation.contains("pitch (deg)"))
						parameters.startPitchOffset_deg = orientation["pitch (deg)"];

					if (orientation.contains("roll (deg)"))
						parameters.startRollOffset_deg = orientation["roll (deg)"];

					if (orientation.contains("yaw (deg)"))
						parameters.startYawOffset_deg = orientation["yaw (deg)"];
				}
				else if (experiment.contains("sensor orientation offset"))
				{
					auto orientation = experiment["sensor orientation offset"];

					if (orientation.contains("pitch (deg)"))
						parameters.startPitchOffset_deg = orientation["pitch (deg)"];

					if (orientation.contains("roll (deg)"))
						parameters.startRollOffset_deg = orientation["roll (deg)"];

					if (orientation.contains("yaw (deg)"))
						parameters.startYawOffset_deg = orientation["yaw (deg)"];
				}

				parameters.endPitchOffset_deg = parameters.startPitchOffset_deg;
				parameters.endRollOffset_deg = parameters.startRollOffset_deg;
				parameters.endYawOffset_deg = parameters.startYawOffset_deg;

				if (experiment.contains("sensor final orientation offset"))
				{
					auto orientation = experiment["sensor final orientation offset"];

					if (orientation.contains("pitch (deg)"))
						parameters.endPitchOffset_deg = orientation["pitch (deg)"];

					if (orientation.contains("roll (deg)"))
						parameters.endRollOffset_deg = orientation["roll (deg)"];

					if (orientation.contains("yaw (deg)"))
						parameters.endYawOffset_deg = orientation["yaw (deg)"];
				}

				mParameters[name] = parameters;
			}
		}
	}
	catch (const nlohmann::json::invalid_iterator& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::type_error& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::out_of_range& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::other_error& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const nlohmann::json::exception& e)
	{
		console_message(e.what());
		return false;
	}
	catch (const std::exception& e)
	{
		console_message(e.what());
		return false;
	}

	return true;
}

