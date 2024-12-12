
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotDataConfigOptions
{
public:
	cPlotDataConfigOptions();
	~cPlotDataConfigOptions() = default;

	void clear();

	bool isDirty() const;

	bool getSaveDataRowMajor() const;

	void setSaveDataRowMajor(bool row_major);

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	bool mSaveDataRowMajor = false;


	friend class cPlotDataConfigFile;
};


