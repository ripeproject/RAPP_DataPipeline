
#pragma once

#include "HySpexVNIR3000N_BSQ.hpp"


class cHySpexVNIR3000N_BSQ_ArcMap : public cHySpexVNIR3000N_BSQ
{
public:
	cHySpexVNIR3000N_BSQ_ArcMap();
	virtual ~cHySpexVNIR3000N_BSQ_ArcMap();

protected:
	void writeHeader() override;
};
