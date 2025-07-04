
#pragma once

#include "PlotConfigScan.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <limits>


// Forward Declaration


class cPlotConfigCatalog
{
public:
	typedef std::vector<cPlotConfigPlotInfo> Plot_t;

	typedef Plot_t::iterator				iterator;
	typedef Plot_t::const_iterator			const_iterator;

public:
	cPlotConfigCatalog();
	~cPlotConfigCatalog();

	bool isDirty() const;

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;


	friend class cPlotConfigFile;
};


