
#pragma once

#include "HySpexVNIR3000N_File.hpp"

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexVNIR3000N_BIP : public cHySpexVNIR3000N_File
{
public:
	cHySpexVNIR3000N_BIP();
	~cHySpexVNIR3000N_BIP();

protected:
	void onImage(HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

protected:
	void openDataFile() override;
	void writeHeader() override;
};
