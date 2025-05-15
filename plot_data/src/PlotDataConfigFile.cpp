
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
	mOptions.clear();
	mGroupBy.clear();
	mHeightParameters.clear();
	mBiomassParameters.clear();
	mLaiParameters.clear();
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

	clear();

	mFileName = file_name;
	mTmpFileName = nStringUtils::make_temp_filename(mFileName);

	mResultsRootFileName = configDoc["result file name"];

	if (configDoc.contains("experiment title"))
		mExperimentTitle = configDoc["experiment title"];

	mOptions.load(configDoc);
	mGroupBy.load(configDoc);
	mHeightParameters.load(configDoc);
	mBiomassParameters.load(configDoc);
	mLaiParameters.load(configDoc);

	return true;
}

void cPlotDataConfigFile::save()
{
	if (mFileName.empty()) return;

	nlohmann::json configDoc;

	mOptions.save(configDoc);
	mGroupBy.save(configDoc);
	mHeightParameters.save(configDoc);
	mBiomassParameters.save(configDoc);
	mLaiParameters.save(configDoc);

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

	mOptions.save(configDoc);
	mGroupBy.save(configDoc);
	mHeightParameters.save(configDoc);
	mBiomassParameters.save(configDoc);
	mLaiParameters.save(configDoc);

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

	mOptions.load(configDoc);
	mOptions.setDirty(true);

	mGroupBy.load(configDoc);
	mGroupBy.setDirty(true);

	mHeightParameters.load(configDoc);
	mHeightParameters.setDirty(true);

	mBiomassParameters.load(configDoc);
	mBiomassParameters.setDirty(true);

	mLaiParameters.load(configDoc);
	mLaiParameters.setDirty(true);

	return true;
}

void cPlotDataConfigFile::save_temporary_file()
{
	nlohmann::json configDoc;

	bool options_dirty = mOptions.isDirty();
	mOptions.save(configDoc);
	mOptions.setDirty(options_dirty);

	bool group_by_dirty = mGroupBy.isDirty();
	mGroupBy.save(configDoc);
	mGroupBy.setDirty(group_by_dirty);

	bool height_dirty = mHeightParameters.isDirty();
	mHeightParameters.save(configDoc);
	mHeightParameters.setDirty(height_dirty);

	bool biomass_dirty = mBiomassParameters.isDirty();
	mBiomassParameters.save(configDoc);
	mBiomassParameters.setDirty(biomass_dirty);

	bool lai_dirty = mLaiParameters.isDirty();
	mLaiParameters.save(configDoc);
	mLaiParameters.setDirty(lai_dirty);

	std::ofstream out;
	out.open(mTmpFileName, std::ios::trunc);
	if (!out.is_open())
		return;

	out << std::setw(4) << configDoc << std::endl;
}

const std::string& cPlotDataConfigFile::getResultsRootFileName() const
{
	return mResultsRootFileName;
}

bool cPlotDataConfigFile::hasExperimentTitle() const
{
	return !mExperimentTitle.empty();
}

const std::string& cPlotDataConfigFile::getExperimentTitle() const
{
	return mExperimentTitle;
}

const cPlotDataConfigOptions& cPlotDataConfigFile::getOptions() const
{
	return mOptions;

}

cPlotDataConfigOptions& cPlotDataConfigFile::getOptions()
{
	return mOptions;
}


const cPlotDataConfigGroupBy& cPlotDataConfigFile::getGroupBy() const
{ 
	return mGroupBy;
}

cPlotDataConfigGroupBy& cPlotDataConfigFile::getGroupBy()
{
	return mGroupBy;
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

const cPlotDataConfigLAI& cPlotDataConfigFile::getLaiParameters() const
{
	return mLaiParameters;
}

cPlotDataConfigLAI& cPlotDataConfigFile::getLaiParameters()
{
	return mLaiParameters;
}


