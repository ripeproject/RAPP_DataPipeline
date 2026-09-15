
#pragma once

#include "ColorTable.hpp"


class cIronbowColorTable : public cColorTable
{
public:
	cIronbowColorTable();
	cIronbowColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

