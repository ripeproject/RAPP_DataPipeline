
#pragma once

#include "HySpexSWIR384_File.hpp"


class cHySpexSWIR384_BSQ : public cHySpexSWIR384_File
{
public:
	cHySpexSWIR384_BSQ();
	virtual ~cHySpexSWIR384_BSQ();

protected:
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

protected:
	std::filesystem::path createDataFilename(char plotID) override;
};
