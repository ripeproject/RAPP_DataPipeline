
#pragma once

#include "HySpexVNIR3000N_BIP.hpp"


class cHySpexVNIR3000N_BIP_ENVI : public cHySpexVNIR3000N_BIP
{
public:
	cHySpexVNIR3000N_BIP_ENVI();
	virtual ~cHySpexVNIR3000N_BIP_ENVI();

protected:
	void writeHeader(std::filesystem::path filename) override;
};
