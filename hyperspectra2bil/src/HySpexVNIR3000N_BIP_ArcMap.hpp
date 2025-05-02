
#pragma once

#include "HySpexVNIR3000N_BIP.hpp"


class cHySpexVNIR3000N_BIP_ArcMap : public cHySpexVNIR3000N_BIP
{
public:
	cHySpexVNIR3000N_BIP_ArcMap();
	virtual ~cHySpexVNIR3000N_BIP_ArcMap();

protected:
	void writeHeader(std::filesystem::path filename) override;
};
