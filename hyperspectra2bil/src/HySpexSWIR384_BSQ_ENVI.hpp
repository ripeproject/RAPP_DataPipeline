
#pragma once

#include "HySpexSWIR384_BSQ.hpp"


class cHySpexSWIR384_BSQ_ENVI : public cHySpexSWIR384_BSQ
{
public:
	cHySpexSWIR384_BSQ_ENVI();
	virtual ~cHySpexSWIR384_BSQ_ENVI();

protected:
	void writeHeader(std::filesystem::path filename) override;
};
