
#pragma once

#include "ColorTable.hpp"

class cRainbow_HC_ColorTable : cColorTable
{
public:
	cRainbow_HC_ColorTable();
	cRainbow_HC_ColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

