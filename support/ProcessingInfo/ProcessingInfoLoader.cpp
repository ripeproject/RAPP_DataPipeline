
#include "ProcessingInfoLoader.hpp"
#include "ProcessingInfo.hpp"


cProcessingInfoLoader::cProcessingInfoLoader(std::weak_ptr<cProcessingInfo> info)
{
    mInfo = info.lock();
}

cProcessingInfoLoader::~cProcessingInfoLoader()
{}


void cProcessingInfoLoader::onProcessingInfo(processing_info::sProcessInfo_1_t info)
{
    mInfo->addProcessingInfo(info);
}
