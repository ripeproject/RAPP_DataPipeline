
#include "PointCloud.hpp"


/******************************************************************************
 * 
 * Classes to store a point clouds generated by generated by a LiDAR sensor.
 * 
 *****************************************************************************/


/******************************************************************************
 *
 * Class to store a reduced point clouds generated by one given frame of the sensor.
 * Invalid points (x=0, y=0, and z=0) are not stored in the point cloud.
 *
 *****************************************************************************/

 /////////////////////////////////////////////////////////////////////////////////////
 // Reduced Point Cloud By Frame
 /////////////////////////////////////////////////////////////////////////////////////

cReducedPointCloudByFrame::cReducedPointCloudByFrame()
    : mFrameID(0), mTimestamp_ns(0)
{}

void cReducedPointCloudByFrame::frameID(uint16_t id)
{
    mFrameID = id;
}

void cReducedPointCloudByFrame::timestamp_ns(uint64_t ts)
{
    mTimestamp_ns = ts;
}

void cReducedPointCloudByFrame::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cReducedPointCloudByFrame::set(std::size_t point, const pointcloud::sCloudPoint_t& cloudPoint)
{
	if (point < mCloud.size())
	{

		if (mHasPoints)
		{
			if (cloudPoint.X_m < mMinX)
				mMinX = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX)
				mMaxX = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY)
				mMinY = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY)
				mMaxY = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ)
				mMinZ = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ)
				mMaxZ = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX = mMaxX = cloudPoint.X_m;
			mMinY = mMaxY = cloudPoint.Y_m;
			mMinZ = mMaxZ = cloudPoint.Z_m;
		}

		mCloud[point] = cloudPoint;
	}
}

void cReducedPointCloudByFrame::addPoint(const pointcloud::sCloudPoint_t& cloudPoint)
{
    if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
        return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX)
			mMinX = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX)
			mMaxX = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY)
			mMinY = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY)
			mMaxY = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ)
			mMinZ = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ)
			mMaxZ = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX = mMaxX = cloudPoint.X_m;
		mMinY = mMaxY = cloudPoint.Y_m;
		mMinZ = mMaxZ = cloudPoint.Z_m;
	}

    mCloud.push_back(cloudPoint);
}


/////////////////////////////////////////////////////////////////////////////////////
// Reduced Point Cloud By Frame with Frame ID info
/////////////////////////////////////////////////////////////////////////////////////

cReducedPointCloudByFrame_FrameId::cReducedPointCloudByFrame_FrameId()
	: mFrameID(0), mTimestamp_ns(0)
{}

void cReducedPointCloudByFrame_FrameId::frameID(uint16_t id)
{
	mFrameID = id;
}

void cReducedPointCloudByFrame_FrameId::timestamp_ns(uint64_t ts)
{
	mTimestamp_ns = ts;
}

void cReducedPointCloudByFrame_FrameId::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cReducedPointCloudByFrame_FrameId::set(std::size_t point, const pointcloud::sCloudPoint_FrameID_t& cloudPoint)
{
	if (point < mCloud.size())
	{

		if (mHasPoints)
		{
			if (cloudPoint.X_m < mMinX)
				mMinX = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX)
				mMaxX = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY)
				mMinY = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY)
				mMaxY = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ)
				mMinZ = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ)
				mMaxZ = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX = mMaxX = cloudPoint.X_m;
			mMinY = mMaxY = cloudPoint.Y_m;
			mMinZ = mMaxZ = cloudPoint.Z_m;
		}

		mCloud[point] = cloudPoint;
	}
}

void cReducedPointCloudByFrame_FrameId::addPoint(const pointcloud::sCloudPoint_FrameID_t& cloudPoint)
{
	if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
		return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX)
			mMinX = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX)
			mMaxX = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY)
			mMinY = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY)
			mMaxY = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ)
			mMinZ = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ)
			mMaxZ = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX = mMaxX = cloudPoint.X_m;
		mMinY = mMaxY = cloudPoint.Y_m;
		mMinZ = mMaxZ = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}


 /////////////////////////////////////////////////////////////////////////////////////
 // Reduced Point Cloud By Frame with Frame ID and Sensor Info
 /////////////////////////////////////////////////////////////////////////////////////

cReducedPointCloudByFrame_SensorInfo::cReducedPointCloudByFrame_SensorInfo()
	: mFrameID(0), mTimestamp_ns(0)
{}

void cReducedPointCloudByFrame_SensorInfo::frameID(uint16_t id)
{
	mFrameID = id;
}

void cReducedPointCloudByFrame_SensorInfo::timestamp_ns(uint64_t ts)
{
	mTimestamp_ns = ts;
}

void cReducedPointCloudByFrame_SensorInfo::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cReducedPointCloudByFrame_SensorInfo::set(std::size_t point, const pointcloud::sCloudPoint_SensorInfo_t& cloudPoint)
{
	if (point < mCloud.size())
	{

		if (mHasPoints)
		{
			if (cloudPoint.X_m < mMinX)
				mMinX = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX)
				mMaxX = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY)
				mMinY = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY)
				mMaxY = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ)
				mMinZ = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ)
				mMaxZ = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX = mMaxX = cloudPoint.X_m;
			mMinY = mMaxY = cloudPoint.Y_m;
			mMinZ = mMaxZ = cloudPoint.Z_m;
		}

		mCloud[point] = cloudPoint;
	}
}

void cReducedPointCloudByFrame_SensorInfo::addPoint(const pointcloud::sCloudPoint_SensorInfo_t& cloudPoint)
{
	if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
		return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX)
			mMinX = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX)
			mMaxX = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY)
			mMinY = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY)
			mMaxY = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ)
			mMinZ = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ)
			mMaxZ = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX = mMaxX = cloudPoint.X_m;
		mMinY = mMaxY = cloudPoint.Y_m;
		mMinZ = mMaxZ = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}


 /////////////////////////////////////////////////////////////////////////////////////
 //  Point Cloud Data
 /////////////////////////////////////////////////////////////////////////////////////

void cPointCloud::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cPointCloud::set(std::size_t point, const pointcloud::sCloudPoint_t& cloudPoint)
{
	if (point < mCloud.size())
	{
		if (mHasPoints)
		{
			if (cloudPoint.X_m < mMinX)
				mMinX = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX)
				mMaxX = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY)
				mMinY = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY)
				mMaxY = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ)
				mMinZ = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ)
				mMaxZ = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX = mMaxX = cloudPoint.X_m;
			mMinY = mMaxY = cloudPoint.Y_m;
			mMinZ = mMaxZ = cloudPoint.Z_m;
		}

		mCloud[point] = cloudPoint;
	}
}

void cPointCloud::addPoint(const pointcloud::sCloudPoint_t& cloudPoint)
{
	if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
		return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX)
			mMinX = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX)
			mMaxX = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY)
			mMinY = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY)
			mMaxY = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ)
			mMinZ = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ)
			mMaxZ = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX = mMaxX = cloudPoint.X_m;
		mMinY = mMaxY = cloudPoint.Y_m;
		mMinZ = mMaxZ = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}


 /////////////////////////////////////////////////////////////////////////////////////
 // Point Cloud with Frame ID info
 /////////////////////////////////////////////////////////////////////////////////////

void cPointCloud_FrameId::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cPointCloud_FrameId::set(std::size_t point, const pointcloud::sCloudPoint_FrameID_t& cloudPoint)
{
	if (point < mCloud.size())
	{
		if (mHasPoints)
		{
			if (cloudPoint.X_m < mMinX)
				mMinX = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX)
				mMaxX = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY)
				mMinY = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY)
				mMaxY = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ)
				mMinZ = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ)
				mMaxZ = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX = mMaxX = cloudPoint.X_m;
			mMinY = mMaxY = cloudPoint.Y_m;
			mMinZ = mMaxZ = cloudPoint.Z_m;
		}

		mCloud[point] = cloudPoint;
	}
}

void cPointCloud_FrameId::addPoint(const pointcloud::sCloudPoint_FrameID_t& cloudPoint)
{
	if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
		return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX)
			mMinX = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX)
			mMaxX = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY)
			mMinY = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY)
			mMaxY = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ)
			mMinZ = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ)
			mMaxZ = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX = mMaxX = cloudPoint.X_m;
		mMinY = mMaxY = cloudPoint.Y_m;
		mMinZ = mMaxZ = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}


 /////////////////////////////////////////////////////////////////////////////////////
 // Point Cloud with Frame ID and Sensor Info
 /////////////////////////////////////////////////////////////////////////////////////

void cPointCloud_SensorInfo::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cPointCloud_SensorInfo::set(std::size_t point, const pointcloud::sCloudPoint_SensorInfo_t& cloudPoint)
{
	if (point < mCloud.size())
	{
		if (mHasPoints)
		{
			if (cloudPoint.X_m < mMinX)
				mMinX = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX)
				mMaxX = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY)
				mMinY = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY)
				mMaxY = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ)
				mMinZ = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ)
				mMaxZ = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX = mMaxX = cloudPoint.X_m;
			mMinY = mMaxY = cloudPoint.Y_m;
			mMinZ = mMaxZ = cloudPoint.Z_m;
		}

		mCloud[point] = cloudPoint;
	}
}

void cPointCloud_SensorInfo::addPoint(const pointcloud::sCloudPoint_SensorInfo_t& cloudPoint)
{
	if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
		return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX)
			mMinX = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX)
			mMaxX = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY)
			mMinY = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY)
			mMaxY = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ)
			mMinZ = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ)
			mMaxZ = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX = mMaxX = cloudPoint.X_m;
		mMinY = mMaxY = cloudPoint.Y_m;
		mMinZ = mMaxZ = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}

