
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
	for (auto plot : mPlots)
	{
		delete plot.second;
	}

	mPlots.clear();
}

bool cConfigFileData::empty() const
{
	return mPlots.empty();
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

	auto scans = configDoc["scans"];
	for (auto scan : scans)
	{
		std::string name = scan["experiment_name"];

		cPlotBoundaries* plots = new cPlotBoundaries();

		plots->load(scan);

		mPlots.insert(std::make_pair(name, plots));
	}

	return true;
}



