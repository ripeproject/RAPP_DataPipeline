
#include "ProcessingInfoLoader.hpp"


cProcessingInfoLoader::cProcessingInfoLoader(cProcessingInfo& info)
    : mInfo(info)
{
}

cProcessingInfoLoader::~cProcessingInfoLoader()
{}


void cProcessingInfoLoader::onProcessingInfo(processing_info::sProcessInfo_1_t info)
{
    mInfo.addProcessingInfo(info);
}
