
#pragma once

#include "ColorTable.hpp"

class cRainbowColorTable : cColorTable
{
public:
	cRainbowColorTable();
	cRainbowColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

