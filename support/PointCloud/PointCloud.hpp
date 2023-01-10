#pragma once

#include "PointCloudTypes.hpp"

#include <vector>


/**
 * Class to deal with point clouds.
 */
class cPointCloud
{
public:
    typedef std::vector<pointcloud::sCloudPoint_t> vCloud_t;

public:
    cPointCloud();
    ~cPointCloud();

    uint16_t frameID() const { return mFrameID; }
    void frameID(uint16_t id);

    uint64_t timestamp_ns() const { return mTimestamp_ns; }
    void timestamp_ns(uint64_t ts);

    void clear();

    std::size_t size() const;

    void addPoint(const pointcloud::sCloudPoint_t& cloudPoint);

    const vCloud_t& data() const { return mCloud; }

private:
    uint16_t mFrameID;
    uint64_t mTimestamp_ns;
    vCloud_t mCloud;
};



