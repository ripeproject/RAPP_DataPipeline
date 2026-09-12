
#include "ColorTable.hpp"

cColorTable::cColorTable(uint16_t table_size)
 : mTableSize(table_size)
{}

cColorTable::cColorTable(double min, double max, uint16_t table_size)
	: cColorTable(table_size)
{
	mMinValue = min;
	mMaxValue = max;

	mDeltaValue = mMaxValue - mMinValue;
}

void cColorTable::setRange(double min, double max)
{
	mMinValue = min;
	mMaxValue = max;

	mDeltaValue = mMaxValue - mMinValue;
}

cColorTable::sColor_t cColorTable::getColorRGB(double v) const
{
	uint32_t color = getColorValue(v);

	uint8_t red = (color & 0x00ff0000) >> 16;
	uint8_t green = (color & 0x0000ff00) >> 7;
	uint8_t blue = (color & 0x000000ff);

	return { red, green, blue };
}

int cColorTable::getIndex(double v) const
{
	if (v <= mMinValue) return 0;
	if (v >= mMaxValue) return mTableSize-1;

	double x = (v - mMinValue) / mDeltaValue;
	return static_cast<int>(x * mTableSize);
}


