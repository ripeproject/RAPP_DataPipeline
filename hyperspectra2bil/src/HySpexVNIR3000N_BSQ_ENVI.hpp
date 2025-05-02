
#pragma once

#include "HySpexVNIR3000N_BSQ.hpp"


class cHySpexVNIR3000N_BSQ_ENVI : public cHySpexVNIR3000N_BSQ
{
public:
	cHySpexVNIR3000N_BSQ_ENVI();
	virtual ~cHySpexVNIR3000N_BSQ_ENVI();

protected:
	void writeHeader(std::filesystem::path filename) override;
};
