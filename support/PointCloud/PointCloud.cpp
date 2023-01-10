
#include "PointCloud.hpp"


cPointCloud::cPointCloud()
    : mFrameID(0), mTimestamp_ns(0)
{}

cPointCloud::~cPointCloud()
{}

void cPointCloud::frameID(uint16_t id)
{
    mFrameID = id;
}

void cPointCloud::timestamp_ns(uint64_t ts)
{
    mTimestamp_ns = ts;
}

void cPointCloud::clear()
{
    mCloud.clear();
}

std::size_t cPointCloud::size() const
{
    return mCloud.size();
}

void cPointCloud::addPoint(const pointcloud::sCloudPoint_t& cloudPoint)
{
    mCloud.push_back(cloudPoint);
}
