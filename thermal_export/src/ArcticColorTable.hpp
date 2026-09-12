
#pragma once

#include "ColorTable.hpp"

class cArcticColorTable : cColorTable
{
public:
	cArcticColorTable();
	cArcticColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

