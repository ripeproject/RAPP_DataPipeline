
#include "ConfigFileData.hpp"

#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>

namespace
{
}

cConfigFileData::cConfigFileData(const std::string& filename)
	: mConfigFilename(filename)
{
}

cConfigFileData::~cConfigFileData()
{
	mPlots.clear();
}

bool cConfigFileData::empty() const
{
	return mPlots.empty();
}

bool cConfigFileData::savePlotsInSingleFile() const
{
	return mSavePlotsInSingleFile;
}

bool cConfigFileData::savePlysFiles() const
{
	return mSavePlyFiles;
}

bool cConfigFileData::plysUseBinaryFormat() const
{
	return mPlyUseBinaryFormat;
}

bool cConfigFileData::saveFrameIds() const
{
	return mSaveFrameIds;
}

bool cConfigFileData::savePixelInfo() const
{
	return mSavePixelInfo;
}

bool cConfigFileData::load()
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

	if (configDoc.contains("options"))
	{
		auto options = configDoc["options"];

		if (options.contains("create plys"))
			mSavePlyFiles = options["create plys"];

		if (options.contains("save plots in single file"))
			mSavePlotsInSingleFile = options["save plots in single file"];

		if (options.contains("plys use binary format"))
			mPlyUseBinaryFormat = options["plys use binary format"];

		if (options.contains("save frame id info"))
			mSaveFrameIds = options["save frame id info"];

		if (options.contains("save pixel info"))
			mSavePixelInfo = options["save pixel info"];
	}

	auto scans = configDoc["scans"];
	for (auto scan : scans)
	{
		std::string name;
		
		if (scan.contains("experiment_name"))
			name = scan["experiment_name"];
		else
			name = scan["experiment name"];

		auto filename = safeFilename(name);

		std::shared_ptr<cPlotBoundaries> plots = std::make_shared<cPlotBoundaries>();

		plots->load(scan);

		mPlots.insert(std::make_pair(filename, plots));
	}

	return true;
}

bool cConfigFileData::hasPlotInfo(const std::string& scan_name) const
{
	using namespace nStringUtils;

	auto measurement_name = removeProcessedTimestamp(scan_name);
	auto in = removeMeasurementTimestamp(measurement_name.filename);
	auto filename = safeFilename(in.filename);

	return mPlots.contains(filename);
}

std::shared_ptr<cPlotBoundaries> cConfigFileData::getPlotInfo(const std::string& scan_name) const
{
	using namespace nStringUtils;

	auto measurement_name = removeProcessedTimestamp(scan_name);
	auto in = removeMeasurementTimestamp(measurement_name.filename);
	auto filename = safeFilename(in.filename);

	auto it = mPlots.find(filename);

	if (it == mPlots.end())
		return std::shared_ptr<cPlotBoundaries>();

	return it->second;
}

