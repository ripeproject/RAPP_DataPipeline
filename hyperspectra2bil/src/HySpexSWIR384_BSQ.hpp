
#pragma once

#include "HySpexSWIR384_File.hpp"

#include <cbdf/HySpexSWIR_384_Parser.hpp>
#include <cbdf/SpidercamParser.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexSWIR384_BSQ : public cHySpexSWIR384_File
{
public:
	cHySpexSWIR384_BSQ();
	~cHySpexSWIR384_BSQ();

protected:
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image) override;
	void onImage(uint8_t device_id, HySpexConnect::cImageData<uint16_t> image, uint8_t spatialSkip, uint8_t spectralSkip) override;

protected:
	void openDataFile() override;
	void writeHeader() override;
};
