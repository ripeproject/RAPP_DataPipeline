
#include "PlotDataConfigFile.hpp"
#include "StringUtils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>


cPlotDataConfigFile::cPlotDataConfigFile()
{}

cPlotDataConfigFile::~cPlotDataConfigFile()
{}

const std::string& cPlotDataConfigFile::getFileName() const
{
	return mFileName;
}

const std::string& cPlotDataConfigFile::getTemporaryFileName() const
{
	return mTmpFileName;
}

bool cPlotDataConfigFile::isDirty() const
{

	return mHeightParameters.isDirty();
}

void cPlotDataConfigFile::clear()
{
	mFileName.clear();
	mHeightParameters.clear();
	mBiomassParameters.clear();
	mTmpFileName = "~newfile.plotdata";
}

bool cPlotDataConfigFile::open(const std::string& file_name)
{
	using namespace nStringUtils;

	if (file_name.empty()) return false;

	if (is_temp_filename(file_name))
		return open_temporary_file(file_name);

	std::ifstream in;
	in.open(file_name);
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

	mFileName = file_name;
	mTmpFileName = nStringUtils::make_temp_filename(mFileName);

	mHeightParameters.load(configDoc);
	mBiomassParameters.load(configDoc);

	return true;
}

void cPlotDataConfigFile::save()
{
	if (mFileName.empty()) return;

	nlohmann::json configDoc;

	mHeightParameters.save(configDoc);
	mBiomassParameters.save(configDoc);

	std::ofstream out;
	out.open(mFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;

	if (out.good())
	{
		// Remove the temporary file
		std::filesystem::path tmp = mTmpFileName;
		if (std::filesystem::exists(tmp))
		{
			std::filesystem::remove(tmp);
		}
	}
}

void cPlotDataConfigFile::save_as(const std::string& file_name)
{
	nlohmann::json configDoc;

	mHeightParameters.save(configDoc);
	mBiomassParameters.save(configDoc);

	std::ofstream out;
	out.open(file_name);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;

	if (out.good())
	{
		// Remove the temporary file
		std::filesystem::path tmp = mTmpFileName;
		if (std::filesystem::exists(tmp))
		{
			std::filesystem::remove(tmp);
		}
	}

	mFileName = file_name;
	mTmpFileName = nStringUtils::make_temp_filename(mFileName);
}

bool cPlotDataConfigFile::open_temporary_file(const std::string& file_name)
{
	if (file_name.empty()) return false;
	if (!nStringUtils::is_temp_filename(file_name)) return false;

	std::ifstream in;
	in.open(file_name);
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

	mTmpFileName = file_name;

	mFileName = nStringUtils::convert_temp_filename(mTmpFileName);

	if (mFileName.find("newfile") != std::string::npos)
	{
		mFileName.clear();
	}

	mHeightParameters.load(configDoc);
	mHeightParameters.setDirty(true);

	mBiomassParameters.load(configDoc);
	mBiomassParameters.setDirty(true);

	return true;
}

void cPlotDataConfigFile::save_temporary_file()
{
	nlohmann::json configDoc;

	bool height_dirty = mHeightParameters.isDirty();
	mHeightParameters.save(configDoc);
	mHeightParameters.setDirty(height_dirty);

	bool biomass_dirty = mBiomassParameters.isDirty();
	mBiomassParameters.save(configDoc);
	mBiomassParameters.setDirty(biomass_dirty);

	std::ofstream out;
	out.open(mTmpFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;
}

const cPlotDataConfigHeight& cPlotDataConfigFile::getHeightParameters() const
{
	return mHeightParameters;
}

cPlotDataConfigHeight& cPlotDataConfigFile::getHeightParameters()
{
	return mHeightParameters;
}

const cPlotDataConfigBiomass& cPlotDataConfigFile::getBiomassParameters() const
{
	return mBiomassParameters;
}

cPlotDataConfigBiomass& cPlotDataConfigFile::getBiomassParameters()
{
	return mBiomassParameters;
}

