
#pragma once

#include <cstdint>


class cColorTable
{
public:
	struct sColor_t
	{
		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
	};

public:
	cColorTable(uint16_t table_size);
	cColorTable(double min, double max, uint16_t table_size);

	void setRange(double min, double max);

	virtual uint32_t getColorValue(double v) const = 0;
	
	sColor_t getColorRGB(double v) const;

protected:
	int getIndex(double v) const;

protected:
	const uint16_t mTableSize;

private:
	double mMinValue = 0;
	double mMaxValue = 0;
	double mDeltaValue = 0;
};

