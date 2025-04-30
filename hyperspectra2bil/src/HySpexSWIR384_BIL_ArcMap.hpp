
#pragma once

#include "HySpexSWIR384_BIL.hpp"


class cHySpexSWIR384_BIL_ArcMap : public cHySpexSWIR384_BIL
{
public:
	cHySpexSWIR384_BIL_ArcMap();
	virtual ~cHySpexSWIR384_BIL_ArcMap();

protected:
	void writeHeader() override;
};
