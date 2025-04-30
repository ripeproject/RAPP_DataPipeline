
#pragma once

#include "HySpexVNIR3000N_BIL.hpp"

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexVNIR3000N_BIL_ENVI : public cHySpexVNIR3000N_BIL
{
public:
	cHySpexVNIR3000N_BIL_ENVI();
	virtual ~cHySpexVNIR3000N_BIL_ENVI();

protected:
	void writeHeader() override;
};
