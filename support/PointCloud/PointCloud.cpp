
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

cReducedPointCloudByFrame::cReducedPointCloudByFrame(const cReducedPointCloudByFrame_FrameId& pc)
{
	assign(pc);
}

cReducedPointCloudByFrame::cReducedPointCloudByFrame(const cReducedPointCloudByFrame_SensorInfo& pc)
{
	assign(pc);
}

cReducedPointCloudByFrame& cReducedPointCloudByFrame::operator=(const cReducedPointCloudByFrame_FrameId& pc)
{
	assign(pc);

	return *this;
}

cReducedPointCloudByFrame& cReducedPointCloudByFrame::operator=(const cReducedPointCloudByFrame_SensorInfo& pc)
{
	assign(pc);

	return *this;
}


/////////////////////////////////////////////////////////////////////////////////////
// Reduced Point Cloud By Frame with Frame ID info
/////////////////////////////////////////////////////////////////////////////////////

cReducedPointCloudByFrame_FrameId::cReducedPointCloudByFrame_FrameId(const cReducedPointCloudByFrame_SensorInfo& pc)
{
	assign(pc);
}

cReducedPointCloudByFrame_FrameId& cReducedPointCloudByFrame_FrameId::operator=(const cReducedPointCloudByFrame_SensorInfo& pc)
{
	assign(pc);

	return *this;
}


 /////////////////////////////////////////////////////////////////////////////////////
 // Reduced Point Cloud By Frame with Frame ID and Sensor Info
 /////////////////////////////////////////////////////////////////////////////////////



 /////////////////////////////////////////////////////////////////////////////////////
 //  Point Cloud Data
 /////////////////////////////////////////////////////////////////////////////////////

cPointCloud::cPointCloud(const cPointCloud_FrameId& pc)
{
	assign(pc);
}

cPointCloud::cPointCloud(const cPointCloud_SensorInfo& pc)
{
	assign(pc);
}

cPointCloud& cPointCloud::operator=(const cPointCloud_FrameId& pc)
{
	assign(pc);

	return *this;
}

cPointCloud& cPointCloud::operator=(const cPointCloud_SensorInfo& pc)
{
	assign(pc);

	return *this;
}

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
			if (cloudPoint.X_m < mMinX_m)
				mMinX_m = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX_m)
				mMaxX_m = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY_m)
				mMinY_m = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY_m)
				mMaxY_m = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ_m)
				mMinZ_m = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ_m)
				mMaxZ_m = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX_m = mMaxX_m = cloudPoint.X_m;
			mMinY_m = mMaxY_m = cloudPoint.Y_m;
			mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
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
		if (cloudPoint.X_m < mMinX_m)
			mMinX_m = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX_m)
			mMaxX_m = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY_m)
			mMinY_m = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY_m)
			mMaxY_m = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ_m)
			mMinZ_m = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ_m)
			mMaxZ_m = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX_m = mMaxX_m = cloudPoint.X_m;
		mMinY_m = mMaxY_m = cloudPoint.Y_m;
		mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}


 /////////////////////////////////////////////////////////////////////////////////////
 // Point Cloud with Frame ID info
 /////////////////////////////////////////////////////////////////////////////////////

cPointCloud_FrameId::cPointCloud_FrameId(const cPointCloud_SensorInfo& pc)
{
	assign(pc);
}

cPointCloud_FrameId& cPointCloud_FrameId::operator=(const cPointCloud_SensorInfo& pc)
{
	assign(pc);

	return *this;
}

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
			if (cloudPoint.X_m < mMinX_m)
				mMinX_m = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX_m)
				mMaxX_m = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY_m)
				mMinY_m = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY_m)
				mMaxY_m = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ_m)
				mMinZ_m = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ_m)
				mMaxZ_m = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX_m = mMaxX_m = cloudPoint.X_m;
			mMinY_m = mMaxY_m = cloudPoint.Y_m;
			mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
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
		if (cloudPoint.X_m < mMinX_m)
			mMinX_m = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX_m)
			mMaxX_m = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY_m)
			mMinY_m = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY_m)
			mMaxY_m = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ_m)
			mMinZ_m = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ_m)
			mMaxZ_m = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX_m = mMaxX_m = cloudPoint.X_m;
		mMinY_m = mMaxY_m = cloudPoint.Y_m;
		mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
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
			if (cloudPoint.X_m < mMinX_m)
				mMinX_m = cloudPoint.X_m;

			if (cloudPoint.X_m > mMaxX_m)
				mMaxX_m = cloudPoint.X_m;

			if (cloudPoint.Y_m < mMinY_m)
				mMinY_m = cloudPoint.Y_m;

			if (cloudPoint.Y_m > mMaxY_m)
				mMaxY_m = cloudPoint.Y_m;

			if (cloudPoint.Z_m < mMinZ_m)
				mMinZ_m = cloudPoint.Z_m;

			if (cloudPoint.Z_m > mMaxZ_m)
				mMaxZ_m = cloudPoint.Z_m;
		}
		else
		{
			mHasPoints = true;
			mMinX_m = mMaxX_m = cloudPoint.X_m;
			mMinY_m = mMaxY_m = cloudPoint.Y_m;
			mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
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
		if (cloudPoint.X_m < mMinX_m)
			mMinX_m = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX_m)
			mMaxX_m = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY_m)
			mMinY_m = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY_m)
			mMaxY_m = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ_m)
			mMinZ_m = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ_m)
			mMaxZ_m = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX_m = mMaxX_m = cloudPoint.X_m;
		mMinY_m = mMaxY_m = cloudPoint.Y_m;
		mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}


/////////////////////////////////////////////////////////////////////////////////////
// Generic Point Cloud
/////////////////////////////////////////////////////////////////////////////////////

cGenericPointCloud::cGenericPointCloud(const cPointCloud& pc)
{
	mHasFrameIDs = false;
	mHasPixelInfo = false;

	assign(pc);
}

cGenericPointCloud::cGenericPointCloud(const cPointCloud_FrameId& pc)
{
	mHasFrameIDs = true;
	mHasPixelInfo = false;

	assign(pc);
}

cGenericPointCloud::cGenericPointCloud(const cPointCloud_SensorInfo& pc)
{
	mHasFrameIDs = true;
	mHasPixelInfo = true;

	assign(pc);
}

cGenericPointCloud& cGenericPointCloud::operator=(const cPointCloud& pc)
{
	mHasFrameIDs = false;
	mHasPixelInfo = false;

	assign(pc);

	return *this;
}

cGenericPointCloud& cGenericPointCloud::operator=(const cPointCloud_FrameId& pc)
{
	mHasFrameIDs = true;
	mHasPixelInfo = false;

	assign(pc);

	return *this;
}

cGenericPointCloud& cGenericPointCloud::operator=(const cPointCloud_SensorInfo& pc)
{
	mHasFrameIDs = true;
	mHasPixelInfo = true;

	assign(pc);

	return *this;
}

std::unique_ptr<cPointCloud> cGenericPointCloud::createPointCloud()
{
	std::unique_ptr<cPointCloud> point_cloud = std::make_unique<cPointCloud>();

	point_cloud->setExtents(mMinX_m, mMaxX_m, mMinY_m, mMaxY_m, mMinZ_m, mMaxZ_m);

	auto n = size();
	point_cloud->resize(n);

	for (std::size_t i = 0; i < n; ++i)
	{
		pointcloud::sCloudPoint_t point = mCloud[i];
		point_cloud->set(i, point);
	}

	return point_cloud;
}

std::unique_ptr<cPointCloud_FrameId> cGenericPointCloud::createPointCloud_FrameId()
{
	if (!mHasFrameIDs)
		return std::unique_ptr<cPointCloud_FrameId>();

	std::unique_ptr<cPointCloud_FrameId> point_cloud = std::make_unique<cPointCloud_FrameId>();

	point_cloud->setExtents(mMinX_m, mMaxX_m, mMinY_m, mMaxY_m, mMinZ_m, mMaxZ_m);

	auto n = size();
	point_cloud->resize(n);

	for (std::size_t i = 0; i < n; ++i)
	{
		pointcloud::sCloudPoint_FrameID_t point = mCloud[i];
		point_cloud->set(i, point);
	}

	return point_cloud;
}

std::unique_ptr<cPointCloud_SensorInfo> cGenericPointCloud::createPointCloud_SensorInfo()
{
	if (!mHasPixelInfo)
		return std::unique_ptr<cPointCloud_SensorInfo>();


	std::unique_ptr<cPointCloud_SensorInfo> point_cloud = std::make_unique<cPointCloud_SensorInfo>();

	point_cloud->setExtents(mMinX_m, mMaxX_m, mMinY_m, mMaxY_m, mMinZ_m, mMaxZ_m);

	auto n = size();
	point_cloud->resize(n);

	for (std::size_t i = 0; i < n; ++i)
	{
		pointcloud::sCloudPoint_SensorInfo_t point = mCloud[i];
		point_cloud->set(i, point);
	}

	return point_cloud;
}

bool cGenericPointCloud::hasFrameIDs() const
{
	return mHasFrameIDs && mEnableFrameIDs;
}

bool cGenericPointCloud::hasPixelInfo() const
{
	return mHasPixelInfo && mEnableFrameIDs && mEnablePixelInfo;
}

void cGenericPointCloud::disableFrameIDs()
{
	mEnableFrameIDs = false;
}

void cGenericPointCloud::enableFrameIDs()
{
	mEnableFrameIDs = true;
}

void cGenericPointCloud::disablePixelInfo()
{
	mEnablePixelInfo = false;
}

void cGenericPointCloud::enablePixelInfo()
{
	mEnablePixelInfo = true;
}

/*
void cGenericPointCloud::resize(std::size_t num_of_points)
{
	mCloud.resize(num_of_points);
}

void cGenericPointCloud::addPoint(const pointcloud::sCloudPoint_SensorInfo_t& cloudPoint)
{
	if ((cloudPoint.X_m == 0.0) && (cloudPoint.Y_m == 0.0) && (cloudPoint.Z_m == 0.0))
		return;

	if (mHasPoints)
	{
		if (cloudPoint.X_m < mMinX_m)
			mMinX_m = cloudPoint.X_m;

		if (cloudPoint.X_m > mMaxX_m)
			mMaxX_m = cloudPoint.X_m;

		if (cloudPoint.Y_m < mMinY_m)
			mMinY_m = cloudPoint.Y_m;

		if (cloudPoint.Y_m > mMaxY_m)
			mMaxY_m = cloudPoint.Y_m;

		if (cloudPoint.Z_m < mMinZ_m)
			mMinZ_m = cloudPoint.Z_m;

		if (cloudPoint.Z_m > mMaxZ_m)
			mMaxZ_m = cloudPoint.Z_m;
	}
	else
	{
		mHasPoints = true;
		mMinX_m = mMaxX_m = cloudPoint.X_m;
		mMinY_m = mMaxY_m = cloudPoint.Y_m;
		mMinZ_m = mMaxZ_m = cloudPoint.Z_m;
	}

	mCloud.push_back(cloudPoint);
}
*/
