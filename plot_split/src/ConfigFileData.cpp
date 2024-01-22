
#include "ConfigFileData.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>

C:\ripe\RAPP_DataPipeline\plot_split\support\StringUtils\StringUtils.hpp
namespace
{
}

cConfigFileData::cConfigFileData(const std::string& filename)
	: mConfigFilename(filename)
{
}

cConfigFileData::~cConfigFileData()
{}

bool cConfigFileData::empty() const
{
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

			if (options.contains("output"))
			{
				std::string output = options["output"];
				if (output == "aggregate")
					mDefaultOptions.outputOption = nConfigFileData::eOutputOptions::AGGREGATE;
				else if (output == "reduced")
					mDefaultOptions.outputOption = nConfigFileData::eOutputOptions::REDUCED_SINGLE_FRAMES;
				else if (output == "sensor")
					mDefaultOptions.outputOption = nConfigFileData::eOutputOptions::SENSOR_SINGLE_FRAMES;
				else
				{
					std::string msg = "Unknown output option: ";
					msg += output;
					msg += ", valid options are aggregate, reduced, and sensor.";
					throw std::logic_error(msg);
				}
			}

			if (options.contains("save"))
			{
				std::string save = options["save"];
				if (save == "basic")
					mDefaultOptions.saveOption = nConfigFileData::eSaveOptions::BASIC;
				else if (save == "frame_id")
					mDefaultOptions.saveOption = nConfigFileData::eSaveOptions::FRAME_ID;
				else if (save == "sensor_info")
					mDefaultOptions.saveOption = nConfigFileData::eSaveOptions::SENSOR_INFO;
				else
				{
					std::string msg = "Unknown save option: ";
					msg += save;
					msg += ", valid options are basic, frame_id, and sensor_info.";
					throw std::logic_error(msg);
				}
			}
		}
	}

	return true;
}



