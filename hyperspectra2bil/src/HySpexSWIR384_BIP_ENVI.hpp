
#pragma once

#include "HySpexSWIR384_BIP.hpp"


class cHySpexSWIR384_BIP_ENVI : public cHySpexSWIR384_BIP
{
public:
	cHySpexSWIR384_BIP_ENVI();
	virtual ~cHySpexSWIR384_BIP_ENVI();

protected:
	void writeHeader() override;
};
