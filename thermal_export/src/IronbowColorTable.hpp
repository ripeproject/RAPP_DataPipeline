
#pragma once

#include "ColorTable.hpp"


class cIronbowColorTable : cColorTable
{
public:
	cIronbowColorTable();
	cIronbowColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

