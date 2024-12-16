
#include "PlotPointCloud.hpp"
#include "PointCloudUtils.hpp"

#include "Constants.hpp"

#include <numbers>
#include <algorithm>
#include <numeric>


plot::sPoint3D_t& plot::sPoint3D_t::operator=(const pointcloud::sCloudPoint_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = 0;
	chnNum = 0;
	pixelNum = 0;

	return *this;
}

plot::sPoint3D_t& plot::sPoint3D_t::operator=(const pointcloud::sCloudPoint_FrameID_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = rhs.frameID;
	chnNum = 0;
	pixelNum = 0;

	return *this;
}

plot::sPoint3D_t& plot::sPoint3D_t::operator=(const pointcloud::sCloudPoint_SensorInfo_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = rhs.frameID;
	chnNum = rhs.chnNum;
	pixelNum = rhs.pixelNum;

	return *this;
}

cPlotPointCloud::cPlotPointCloud() : mCentroid()
{}

cPlotPointCloud::~cPlotPointCloud()
{}

cPlotPointCloud::cPlotPointCloud(plot::sCentroid_t centroid, const vCloud_t& pc)
	: mCentroid(centroid)
{
	mMinX_mm = std::numeric_limits<int>::max();
	mMaxX_mm = std::numeric_limits<int>::min();

	mMinY_mm = std::numeric_limits<int>::max();
	mMaxY_mm = std::numeric_limits<int>::min();

	mMinZ_mm = std::numeric_limits<int>::max();
	mMaxZ_mm = std::numeric_limits<int>::min();

	for (auto& point : pc)
	{
		mMinX_mm = std::min(mMinX_mm, point.x_mm);
		mMaxX_mm = std::max(mMaxX_mm, point.x_mm);
		mMinY_mm = std::min(mMinY_mm, point.y_mm);
		mMaxY_mm = std::max(mMaxY_mm, point.y_mm);
		mMinZ_mm = std::min(mMinZ_mm, point.z_mm);
		mMaxZ_mm = std::max(mMaxZ_mm, point.z_mm);
	}

	mCloud = pc;
}

cPlotPointCloud::cPlotPointCloud(const cBasePointCloud<pointcloud::sCloudPoint_t>& pc)
	: mCentroid()
{
	assign(pc);

	mHasFrameIDs = false;
	mHasPixelInfo = false;
}

cPlotPointCloud::cPlotPointCloud(const cBasePointCloud<pointcloud::sCloudPoint_FrameID_t>& pc)
	: mCentroid()
{
	assign(pc);

	mHasFrameIDs = true;
	mHasPixelInfo = false;
}

cPlotPointCloud::cPlotPointCloud(const cBasePointCloud<pointcloud::sCloudPoint_SensorInfo_t>& pc)
	: mCentroid()
{
	assign(pc);

	mHasFrameIDs = true;
	mHasPixelInfo = true;
}


bool cPlotPointCloud::vegetationOnly() const
{
	return mVegetationOnly;
}

void cPlotPointCloud::setVegetationOnly(const bool vegetation_only)
{
	mVegetationOnly = vegetation_only;
}

bool cPlotPointCloud::hasFrameIDs() const { return mHasFrameIDs && mEnableFrameIDs; }
bool cPlotPointCloud::hasPixelInfo() const { return mHasPixelInfo && mEnableFrameIDs && mEnablePixelInfo; }

void cPlotPointCloud::disableFrameIDs()
{
	mEnableFrameIDs = false; 
}

void cPlotPointCloud::enableFrameIDs()
{
	mEnableFrameIDs = true;
}

void cPlotPointCloud::disablePixelInfo()
{
	mEnablePixelInfo = false;
}

void cPlotPointCloud::enablePixelInfo()
{
	mEnablePixelInfo = true;
}

void cPlotPointCloud::clear()
{
	mCloud.clear();

	mMinX_mm = 0.0;
	mMaxX_mm = 0.0;

	mMinY_mm = 0.0;
	mMaxY_mm = 0.0;

	mMinZ_mm = 0.0;
	mMaxZ_mm = 0.0;

	mCentroid = plot::sCentroid_t();
}

bool cPlotPointCloud::empty() const
{
	return mCloud.empty(); 
}

void cPlotPointCloud::reserve(size_type new_cap)
{
	mCloud.reserve(new_cap);
}

void cPlotPointCloud::resize(size_type count)
{
	mCloud.resize(count);
}

void cPlotPointCloud::resize(size_type count, const value_type& value)
{
	mCloud.resize(count, value);
}

std::size_t cPlotPointCloud::size() const
{
	return mCloud.size(); 
}

void cPlotPointCloud::push_back(const value_type& value)
{
	mCloud.push_back(value);
}

int cPlotPointCloud::minX_mm() const { return mMinX_mm; }
int cPlotPointCloud::maxX_mm() const { return mMaxX_mm; }

int cPlotPointCloud::minY_mm() const { return mMinY_mm; }
int cPlotPointCloud::maxY_mm() const { return mMaxY_mm; }

int cPlotPointCloud::minZ_mm() const { return mMinZ_mm; }
int cPlotPointCloud::maxZ_mm() const { return mMaxZ_mm; }

int cPlotPointCloud::length_mm() const { return mMaxX_mm - mMinX_mm; }
int cPlotPointCloud::width_mm() const { return mMaxY_mm - mMinY_mm; }
int cPlotPointCloud::height_mm() const { return mMaxZ_mm - mMinZ_mm; }

void cPlotPointCloud::recomputeBounds()
{
	if (mCloud.empty())
	{
		mMinX_mm = 0.0;
		mMaxX_mm = 0.0;

		mMinY_mm = 0.0;
		mMaxY_mm = 0.0;

		mMinZ_mm = 0.0;
		mMaxZ_mm = 0.0;

		return;
	}

	auto minX_mm = std::numeric_limits<int>::max();
	auto maxX_mm = std::numeric_limits<int>::min();

	auto minY_mm = std::numeric_limits<int>::max();
	auto maxY_mm = std::numeric_limits<int>::min();

	auto minZ_mm = std::numeric_limits<int>::max();
	auto maxZ_mm = std::numeric_limits<int>::min();

	for (auto& point : mCloud)
	{
		minX_mm = std::min(minX_mm, static_cast<int>(point.x_mm));
		maxX_mm = std::max(maxX_mm, static_cast<int>(point.x_mm));
		minY_mm = std::min(minY_mm, static_cast<int>(point.y_mm));
		maxY_mm = std::max(maxY_mm, static_cast<int>(point.y_mm));
		minZ_mm = std::min(minZ_mm, static_cast<int>(point.z_mm));
		maxZ_mm = std::max(maxZ_mm, static_cast<int>(point.z_mm));
	}

	mMinX_mm = minX_mm;
	mMaxX_mm = maxX_mm;
	mMinY_mm = minY_mm;
	mMaxY_mm = maxY_mm;
	mMinZ_mm = minZ_mm;
	mMaxZ_mm = maxZ_mm;

	double sum_x = 0.0;
	double sum_y = 0.0;
	double sum_z = 0.0;
	auto n = mCloud.size();


	for (auto& point : mCloud)
	{
		sum_x += point.x_mm;
		sum_y += point.y_mm;
		sum_z += point.z_mm;
	}

	double x_mm = sum_x / n;
	double y_mm = sum_y / n;
	double z_mm = sum_z / n;

	mCentroid = { x_mm , y_mm, z_mm };
}

plot::rappPoint_t cPlotPointCloud::center() const
{
	return {static_cast<std::int32_t>((mMaxX_mm + mMinX_mm) / 2),
		static_cast<std::int32_t>((mMaxY_mm + mMinY_mm) / 2), 
		static_cast<std::int32_t>((mMaxZ_mm + mMinZ_mm) / 2) };
}

plot::sCentroid_t cPlotPointCloud::centroid() const { return mCentroid; }

void cPlotPointCloud::addPoint(const plot::sPoint3D_t& cloudPoint)
{
	if ((cloudPoint.x_mm == 0) && (cloudPoint.y_mm == 0) && (cloudPoint.z_mm == 0))
		return;

	if (mCloud.empty())
	{
		mMinX_mm = mMaxX_mm = cloudPoint.x_mm;
		mMinY_mm = mMaxY_mm = cloudPoint.y_mm;
		mMinZ_mm = mMaxZ_mm = cloudPoint.z_mm;
	}
	else
	{
		if (cloudPoint.x_mm < mMinX_mm)
			mMinX_mm = cloudPoint.x_mm;

		if (cloudPoint.x_mm > mMaxX_mm)
			mMaxX_mm = cloudPoint.x_mm;

		if (cloudPoint.y_mm < mMinY_mm)
			mMinY_mm = cloudPoint.y_mm;

		if (cloudPoint.y_mm > mMaxY_mm)
			mMaxY_mm = cloudPoint.y_mm;

		if (cloudPoint.z_mm < mMinZ_mm)
			mMinZ_mm = cloudPoint.z_mm;

		if (cloudPoint.z_mm > mMaxZ_mm)
			mMaxZ_mm = cloudPoint.z_mm;
	}

	mCloud.push_back(cloudPoint);
}

const cPlotPointCloud& cPlotPointCloud::operator+=(const cPlotPointCloud& pc)
{
	for (const auto& point : pc)
	{
		if ((point.x_mm == 0) && (point.y_mm == 0) && (point.z_mm == 0))
			continue;

		mCloud.push_back(point);
	}

	recomputeBounds();

	return *this;
}

plot::sPoint3D_t cPlotPointCloud::getPoint(int x_mm, int y_mm, int r_mm) const
{
	plot::sPoint3D_t result = {x_mm, y_mm, plot::INVALID_HEIGHT };

	if ((x_mm < mMinX_mm) || (x_mm > mMaxX_mm)) return result;
	if ((y_mm < mMinY_mm) || (y_mm > mMaxY_mm)) return result;

	auto lb = result;
	lb.x_mm -= r_mm;

	auto first = std::lower_bound(mCloud.begin(), mCloud.end(), lb,
		[](const plot::sPoint3D_t& a, plot::sPoint3D_t value)
		{
			return a.x_mm < value.x_mm;
		});

	if (first == mCloud.end())
		return result;

	auto ub = result;
	ub.x_mm += r_mm;

	auto last = std::upper_bound(mCloud.begin(), mCloud.end(), ub,
		[](const plot::sPoint3D_t& a, plot::sPoint3D_t value)
		{
			return a.x_mm < value.x_mm;
		});

	std::vector<plot::sPoint3D_t>  xs(first, last);

	lb = result;
	lb.y_mm -= r_mm;

	first = std::lower_bound(xs.begin(), xs.end(), lb,
		[](const plot::sPoint3D_t& a, plot::sPoint3D_t value)
		{
			return a.y_mm < value.y_mm;
		});

	if (first == xs.end())
		return result;

	ub = result;
	ub.y_mm += r_mm;

	last = std::upper_bound(xs.begin(), xs.end(), ub,
		[](const plot::sPoint3D_t& a, plot::sPoint3D_t value)
		{
			return a.y_mm < value.y_mm;
		});

	if (first == last)
		return *first;

	std::vector<plot::sPoint3D_t>  points(first, last);

	if (points.empty())
		return result;

	std::size_t n = 0;
	int z = 0;
	for (const auto& point : points)
	{
		if (point.z_mm != plot::INVALID_HEIGHT)
		{
			z += point.z_mm;
			++n;
		}
	}

	if (n > 0)
		result.z_mm = z / n;

	return result;
}

/*
bool cPlotPointCloud::contains(const rfm::sPlotBoundingBox_t& box)
{
	if ((box.northEastCorner.x_mm < mMinX_mm)
		|| (box.northWestCorner.x_mm < mMinX_mm))
		return false;

	if ((box.southEastCorner.x_mm > mMaxX_mm)
		|| (box.southWestCorner.x_mm > mMaxX_mm))
		return false;

	if ((box.northWestCorner.y_mm < mMinY_mm)
		|| (box.southWestCorner.y_mm < mMinY_mm))
		return false;

	if ((box.northEastCorner.y_mm > mMaxY_mm)
		|| (box.southEastCorner.y_mm > mMaxY_mm))
		return false;

	return true;
}
*/




