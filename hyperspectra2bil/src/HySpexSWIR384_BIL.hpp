
#pragma once

#include "HySpexSWIR384_File.hpp"

#include <deque>
#include <list>


class cHySpexSWIR384_BIL : public cHySpexSWIR384_File
{
public:
	cHySpexSWIR384_BIL();
	virtual ~cHySpexSWIR384_BIL();

protected:
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

protected:
	std::filesystem::path createDataFilename(char plotID) override;
};
