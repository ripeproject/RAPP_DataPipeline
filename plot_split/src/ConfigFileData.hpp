
#pragma once

#include <string>
#include <map>
#include <memory>

// Forward declarations

namespace nConfigFileData
{
	struct sOptions_t
	{
	};
}


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

	bool empty() const;

private:
	std::string mConfigFilename;

	nConfigFileData::sOptions_t mDefaultOptions;

	std::map<std::string, nConfigFileData::sOptions_t> mOptions;
};