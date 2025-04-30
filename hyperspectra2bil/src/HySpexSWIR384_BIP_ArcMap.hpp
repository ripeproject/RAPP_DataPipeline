
#pragma once

#include "HySpexSWIR384_BIP.hpp"


class cHySpexSWIR384_BIP_ArcMap : public cHySpexSWIR384_BIP
{
public:
	cHySpexSWIR384_BIP_ArcMap();
	virtual ~cHySpexSWIR384_BIP_ArcMap();

protected:
	void writeHeader() override;
};
