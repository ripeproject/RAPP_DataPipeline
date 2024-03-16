#pragma once

#include "ProcessingInfo.hpp"

#include "ProcessingInfoParser.hpp"


// Forward Declarations


class cProcessingInfoLoader : public cProcessingInfoParser
{
public:
	explicit cProcessingInfoLoader(cProcessingInfo& info);
	virtual ~cProcessingInfoLoader();

protected:
	void onProcessingInfo(processing_info::sProcessInfo_1_t info) override;

private:
	cProcessingInfo& mInfo;
};

