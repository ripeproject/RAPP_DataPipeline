
#include "ProcessingInfo.hpp"


void cProcessingInfo::clear()
{
	mProcessingSteps.clear();
}

bool cProcessingInfo::empty() const
{
	return mProcessingSteps.empty();
}

std::size_t cProcessingInfo::size() const
{
	return mProcessingSteps.size();
}

cProcessingInfo::const_iterator cProcessingInfo::begin() const
{
	return mProcessingSteps.begin();
}

cProcessingInfo::const_iterator cProcessingInfo::end() const
{
	return mProcessingSteps.end();
}


void cProcessingInfo::addProcessingInfo(processing_info::sProcessInfo_1_t info)
{
	if (mProcessingSteps.empty())
		mProcessingSteps.push_back(info);
	else
		mProcessingSteps.insert(mProcessingSteps.begin(), info);
}
