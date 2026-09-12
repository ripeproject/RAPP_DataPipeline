
#pragma once

#include "ColorTable.hpp"

class cWhiteHotColorTable : cColorTable
{
public:
	cWhiteHotColorTable();
	cWhiteHotColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

