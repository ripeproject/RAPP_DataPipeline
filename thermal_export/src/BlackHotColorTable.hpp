
#pragma once

#include "ColorTable.hpp"

class cBlackHotColorTable : cColorTable
{
public:
	cBlackHotColorTable();
	cBlackHotColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

