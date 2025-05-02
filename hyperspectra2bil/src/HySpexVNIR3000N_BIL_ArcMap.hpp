
#pragma once

#include "HySpexVNIR3000N_BIL.hpp"

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexVNIR3000N_BIL_ArcMap : public cHySpexVNIR3000N_BIL
{
public:
	cHySpexVNIR3000N_BIL_ArcMap();
	virtual ~cHySpexVNIR3000N_BIL_ArcMap();

protected:
	void writeHeader(std::filesystem::path filename) override;
};
