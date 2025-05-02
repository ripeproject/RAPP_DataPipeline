
#pragma once

#include "HySpexSWIR384_BSQ.hpp"


class cHySpexSWIR384_BSQ_ArcMap : public cHySpexSWIR384_BSQ
{
public:
	cHySpexSWIR384_BSQ_ArcMap();
	virtual ~cHySpexSWIR384_BSQ_ArcMap();

protected:
	void writeHeader(std::filesystem::path filename) override;
};
