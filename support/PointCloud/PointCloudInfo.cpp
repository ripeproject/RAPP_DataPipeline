
#include "PointCloudInfo.hpp"


void cPointCloudInfo::clear()
{
}

bool cPointCloudInfo::empty() const
{
	return true;
}

std::size_t cPointCloudInfo::size() const
{
	return 0;
}

/*
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
*/
