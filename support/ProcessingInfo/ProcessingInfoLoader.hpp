#pragma once

#include "ProcessingInfo.hpp"

#include "ProcessingInfoParser.hpp"

#include <memory>

// Forward Declarations
class cProcessingInfo;

class cProcessingInfoLoader : public cProcessingInfoParser
{
public:
	explicit cProcessingInfoLoader(std::weak_ptr<cProcessingInfo> info);
	virtual ~cProcessingInfoLoader();

protected:
	void onProcessingInfo(processing_info::sProcessInfo_1_t info) override;

private:
	std::shared_ptr<cProcessingInfo> mInfo;
};

