
#pragma once

#include "HySpexSWIR384_BIL.hpp"

#include <filesystem>
#include <string>
#include <fstream>



class cHySpexSWIR384_BIL_ENVI : public cHySpexSWIR384_BIL
{
public:
	cHySpexSWIR384_BIL_ENVI();
	virtual ~cHySpexSWIR384_BIL_ENVI();

protected:
	void writeHeader() override;
};
