
#pragma once

#include "HySpexVNIR3000N_File.hpp"


class cHySpexVNIR3000N_BSQ : public cHySpexVNIR3000N_File
{
public:
	cHySpexVNIR3000N_BSQ();
	virtual ~cHySpexVNIR3000N_BSQ();

protected:
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

protected:
	std::filesystem::path createDataFilename(char plotID) override;
};
