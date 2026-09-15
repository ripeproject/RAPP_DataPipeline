
#pragma once

#include "ColorTable.hpp"

class cWhiteHotColorTable : public cColorTable
{
public:
	cWhiteHotColorTable();
	cWhiteHotColorTable(double min, double max);

	uint32_t getColorValue(double v) const override;
};

