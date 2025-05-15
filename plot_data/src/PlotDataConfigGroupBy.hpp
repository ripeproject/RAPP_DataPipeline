
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>

enum class eGroupByTypes { EVENT, SPECIES, CULTIVAR, TREATMENT, CONSTRUCT, POT_LABEL, SEED_GENERATION, COPY_NUMBER, LEAF_TYPE };


class cPlotDataConfigGroupBy
{
public:
	cPlotDataConfigGroupBy();
	~cPlotDataConfigGroupBy() = default;

	void clear();

	bool empty() const;

	bool isDirty() const;

	const std::vector<eGroupByTypes>& getGroupByTypes() const;

	void setGroupByTypes(const std::vector<eGroupByTypes>& groupby);
	void addGroupByType(eGroupByTypes groupby);

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	void save(nlohmann::json& jdoc);

private:
	bool mDirty = false;

	std::vector<eGroupByTypes> mGroupBy;

	friend class cPlotDataConfigFile;
};


