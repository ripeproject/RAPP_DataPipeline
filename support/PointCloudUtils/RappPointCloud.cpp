
#include "RappPointCloud.hpp"
#include "PointCloudUtils.hpp"

#include "Constants.hpp"

#include <cbdf/PointCloudTypes.hpp>

#include <eigen3/Eigen/Eigen>

#include <numbers>
#include <algorithm>
#include <numeric>


namespace
{
	template<typename T>
	inline void rotate(T& x, T& y, T& z, const Eigen::Matrix3d& r)
	{
		const auto& rX = r.col(0);
		const auto& rY = r.col(1);
		const auto& rZ = r.col(2);

		double a = x * rX[0] + y * rX[1] + z * rX[2];
		double b = x * rY[0] + y * rY[1] + z * rY[2];
		double c = x * rZ[0] + y * rZ[1] + z * rZ[2];

		x = static_cast<T>(a);
		y = static_cast<T>(b);
		z = static_cast<T>(c);
	}

	int id = 0;
}


rfm::sPoint3D_t& rfm::sPoint3D_t::operator=(const pointcloud::sCloudPoint_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	h_mm = INVALID_HEIGHT;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = 0;
	chnNum = 0;
	pixelNum = 0;

	return *this;
}

rfm::sPoint3D_t& rfm::sPoint3D_t::operator=(const pointcloud::sCloudPoint_FrameID_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	h_mm = INVALID_HEIGHT;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = rhs.frameID;
	chnNum = 0;
	pixelNum = 0;

	return *this;
}

rfm::sPoint3D_t& rfm::sPoint3D_t::operator=(const pointcloud::sCloudPoint_SensorInfo_t& rhs)
{
	x_mm = rhs.X_m * nConstants::M_TO_MM;
	y_mm = rhs.Y_m * nConstants::M_TO_MM;
	z_mm = rhs.Z_m * nConstants::M_TO_MM;
	h_mm = INVALID_HEIGHT;
	range_mm = rhs.range_mm;
	signal = rhs.signal;
	reflectivity = rhs.reflectivity;
	nir = rhs.nir;
	frameID = rhs.frameID;
	chnNum = rhs.chnNum;
	pixelNum = rhs.pixelNum;

	return *this;
}


rfm::sPoint3D_t to_point(const pointcloud::sCloudPoint_t& rhs)
{
	rfm::sPoint3D_t result;

	result.x_mm = rhs.X_m * nConstants::M_TO_MM;
	result.y_mm = rhs.Y_m * nConstants::M_TO_MM;
	result.z_mm = rhs.Z_m * nConstants::M_TO_MM;
	result.h_mm = rfm::INVALID_HEIGHT;
	result.range_mm = rhs.range_mm;
	result.signal = rhs.signal;
	result.reflectivity = rhs.reflectivity;
	result.nir = rhs.nir;
	result.frameID = 0;
	result.chnNum = 0;
	result.pixelNum = 0;

	return result;
}

rfm::sPoint3D_t to_point(const pointcloud::sCloudPoint_FrameID_t& rhs)
{
	rfm::sPoint3D_t result;

	result.x_mm = rhs.X_m * nConstants::M_TO_MM;
	result.y_mm = rhs.Y_m * nConstants::M_TO_MM;
	result.z_mm = rhs.Z_m * nConstants::M_TO_MM;
	result.h_mm = rfm::INVALID_HEIGHT;
	result.range_mm = rhs.range_mm;
	result.signal = rhs.signal;
	result.reflectivity = rhs.reflectivity;
	result.nir = rhs.nir;
	result.frameID = rhs.frameID;
	result.chnNum = 0;
	result.pixelNum = 0;

	return result;
}

rfm::sPoint3D_t to_point(const pointcloud::sCloudPoint_SensorInfo_t& rhs)
{
	rfm::sPoint3D_t result;

	result.x_mm = rhs.X_m * nConstants::M_TO_MM;
	result.y_mm = rhs.Y_m * nConstants::M_TO_MM;
	result.z_mm = rhs.Z_m * nConstants::M_TO_MM;
	result.h_mm = rfm::INVALID_HEIGHT;
	result.range_mm = rhs.range_mm;
	result.signal = rhs.signal;
	result.reflectivity = rhs.reflectivity;
	result.nir = rhs.nir;
	result.frameID = rhs.frameID;
	result.chnNum = rhs.chnNum;
	result.pixelNum = rhs.pixelNum;

	return result;
}

template<class POINT2>
void assign(const std::vector<POINT2>& in, std::vector<rfm::sPoint3D_t>& out)
{
	out.clear();

	auto n = in.size();
	out.resize(n);

	for (std::size_t i = 0; i < n; ++i)
	{
		rfm::sPoint3D_t point = to_point(in[i]);
		out[i] = point;

		//			POINT2 point = data[i];

		//			auto x = point.X_m * nConstants::M_TO_MM;
		//			auto y = point.Y_m * nConstants::M_TO_MM;
		//			auto z = point.Z_m * nConstants::M_TO_MM;

		//			out[i] = {static_cast<int32_t>(x), static_cast<int32_t>(y), static_cast<int32_t>(z)};
	}
}



cRappPointCloud::cRappPointCloud() : mCentroid(), mID(::id++)
{}

cRappPointCloud::cRappPointCloud(rfm::sCentroid_t centroid, const vCloud_t& pc)
	: mCentroid(centroid), mID(::id++)
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

cRappPointCloud::cRappPointCloud(const std::vector<pointcloud::sCloudPoint_t>& pc)
	: mCentroid(), mID(::id++) 
{
	assign(pc, mCloud);

	recomputeBounds();

	mHasFrameIDs = false;
	mHasPixelInfo = false;
}

cRappPointCloud::cRappPointCloud(const std::vector<pointcloud::sCloudPoint_FrameID_t>& pc)
	: mCentroid(), mID(::id++) 
{
	assign(pc, mCloud);

	recomputeBounds();

	mHasFrameIDs = true;
	mHasPixelInfo = false;
}

cRappPointCloud::cRappPointCloud(const std::vector<pointcloud::sCloudPoint_SensorInfo_t>& pc)
	: mCentroid(), mID(::id++) 
{
	assign(pc, mCloud);

	recomputeBounds();

	mHasFrameIDs = true;
	mHasPixelInfo = true;
}

int cRappPointCloud::id() const { return mID; }

const std::string& cRappPointCloud::name() const { return mName; }
void cRappPointCloud::setName(const std::string& name) { mName = name; }

bool cRappPointCloud::vegetationOnly() const
{
	return mVegetationOnly;
}

void cRappPointCloud::setVegetationOnly(const bool vegetation_only)
{
	mVegetationOnly = vegetation_only;
}

bool cRappPointCloud::hasFrameIDs() const { return mHasFrameIDs && mEnableFrameIDs; }
bool cRappPointCloud::hasPixelInfo() const { return mHasPixelInfo && mEnableFrameIDs && mEnablePixelInfo; }

void cRappPointCloud::disableFrameIDs() 
{
	mEnableFrameIDs = false; 
}

void cRappPointCloud::enableFrameIDs()
{
	mEnableFrameIDs = true;
}

void cRappPointCloud::disablePixelInfo()
{
	mEnablePixelInfo = false;
}

void cRappPointCloud::enablePixelInfo()
{
	mEnablePixelInfo = true;
}

void cRappPointCloud::clear()
{
	mCloud.clear();

	mMinX_mm = 0.0;
	mMaxX_mm = 0.0;

	mMinY_mm = 0.0;
	mMaxY_mm = 0.0;

	mMinZ_mm = 0.0;
	mMaxZ_mm = 0.0;

	mCentroid = rfm::sCentroid_t();
}

bool cRappPointCloud::empty() const
{
	return mCloud.empty(); 
}

void cRappPointCloud::reserve(size_type new_cap)
{
	mCloud.reserve(new_cap);
}

void cRappPointCloud::resize(size_type count)
{
	mCloud.resize(count);
}

void cRappPointCloud::resize(size_type count, const value_type& value)
{
	mCloud.resize(count, value);
}

std::size_t cRappPointCloud::size() const
{
	return mCloud.size(); 
}

bool cRappPointCloud::referenceValid() const
{
	return mReferencePointValid && (mReferencePoint.x_mm > 0) 
		&& (mReferencePoint.y_mm > 0) && (mReferencePoint.z_mm > 0);
}

rfm::rappPoint_t cRappPointCloud::referencePoint() const
{
	return mReferencePoint;
}

void cRappPointCloud::setReferencePoint(rfm::rappPoint_t point, bool valid)
{
	mReferencePoint = point;
	mReferencePointValid = valid;
}

int cRappPointCloud::minX_mm() const { return mMinX_mm; }
int cRappPointCloud::maxX_mm() const { return mMaxX_mm; }

int cRappPointCloud::minY_mm() const { return mMinY_mm; }
int cRappPointCloud::maxY_mm() const { return mMaxY_mm; }

int cRappPointCloud::minZ_mm() const { return mMinZ_mm; }
int cRappPointCloud::maxZ_mm() const { return mMaxZ_mm; }

int cRappPointCloud::length_mm() const { return mMaxX_mm - mMinX_mm; }
int cRappPointCloud::width_mm() const { return mMaxY_mm - mMinY_mm; }
int cRappPointCloud::height_mm() const { return mMaxZ_mm - mMinZ_mm; }

void cRappPointCloud::recomputeBounds()
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

rfm::rappPoint_t cRappPointCloud::center() const
{
	return {static_cast<std::int32_t>((mMaxX_mm + mMinX_mm) / 2),
		static_cast<std::int32_t>((mMaxY_mm + mMinY_mm) / 2), 
		static_cast<std::int32_t>((mMaxZ_mm + mMinZ_mm) / 2) };
}

rfm::sCentroid_t cRappPointCloud::centroid() const { return mCentroid; }

void cRappPointCloud::addPoint(const rfm::sPoint3D_t& cloudPoint)
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

void cRappPointCloud::insert(const_iterator first, const_iterator last)
{
	mCloud.insert(mCloud.end(), first, last);
	recomputeBounds();
}

void cRappPointCloud::push_back(const rfm::sPoint3D_t& cloudPoint)
{
	if ((cloudPoint.x_mm == 0) && (cloudPoint.y_mm == 0) && (cloudPoint.z_mm == 0))
		return;

	mCloud.push_back(cloudPoint);
}

const cRappPointCloud& cRappPointCloud::operator+=(const cRappPointCloud& pc)
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


void cRappPointCloud::sort()
{
	std::sort(mCloud.begin(), mCloud.end(), [](rfm::sPoint3D_t a, rfm::sPoint3D_t b)
		{
			if (a.x_mm < b.x_mm) return true;

			if (a.x_mm == b.x_mm)
			{
				if (a.y_mm < b.y_mm)
					return true;
			}

			return false;
		});
}

rfm::sPoint3D_t cRappPointCloud::getPoint(int x_mm, int y_mm, int r_mm) const
{
	rfm::sPoint3D_t result = {x_mm, y_mm, rfm::INVALID_HEIGHT, rfm::INVALID_HEIGHT};

	if ((x_mm < mMinX_mm) || (x_mm > mMaxX_mm)) return result;
	if ((y_mm < mMinY_mm) || (y_mm > mMaxY_mm)) return result;

	auto lb = result;
	lb.x_mm -= r_mm;

	auto first = std::lower_bound(mCloud.begin(), mCloud.end(), lb,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.x_mm < value.x_mm;
		});

	if (first == mCloud.end())
		return result;

	auto ub = result;
	ub.x_mm += r_mm;

	auto last = std::upper_bound(mCloud.begin(), mCloud.end(), ub,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.x_mm < value.x_mm;
		});

	std::vector<rfm::sPoint3D_t>  xs(first, last);

	lb = result;
	lb.y_mm -= r_mm;

	first = std::lower_bound(xs.begin(), xs.end(), lb,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.y_mm < value.y_mm;
		});

	if (first == xs.end())
		return result;

	ub = result;
	ub.y_mm += r_mm;

	last = std::upper_bound(xs.begin(), xs.end(), ub,
		[](const rfm::sPoint3D_t& a, rfm::sPoint3D_t value)
		{
			return a.y_mm < value.y_mm;
		});

	if (first == last)
		return *first;

	std::vector<rfm::sPoint3D_t>  points(first, last);

	if (points.empty())
		return result;

	std::size_t n = 0;
	int z = 0;
	for (const auto& point : points)
	{
		if (point.z_mm != rfm::INVALID_HEIGHT)
		{
			z += point.z_mm;
			++n;
		}
	}

	if (n > 0)
		result.z_mm = z / n;

	return result;
}


void cRappPointCloud::rotate(double yaw_deg, double pitch_deg, double roll_deg)
{
	double pitch_rad = pitch_deg * nConstants::DEG_TO_RAD;
	double roll_rad = roll_deg * nConstants::DEG_TO_RAD;
	double yaw_rad = yaw_deg * nConstants::DEG_TO_RAD;

	Eigen::AngleAxisd rollAngle(roll_rad, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd yawAngle(yaw_rad, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd pitchAngle(pitch_rad, Eigen::Vector3d::UnitY());

	//	Eigen::Quaternion<double> q = rollAngle * pitchAngle * yawAngle;
	Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
	Eigen::Matrix3d rotationMatrix = q.matrix();

	for (auto& point : mCloud)
	{
		auto x = point.x_mm - mCentroid.x_mm;
		auto y = point.y_mm - mCentroid.y_mm;
		auto z = point.z_mm - mCentroid.z_mm;

		::rotate(x, y, z, rotationMatrix);

		point.x_mm = x + mCentroid.x_mm;
		point.y_mm = y + mCentroid.y_mm;
		point.z_mm = z + mCentroid.z_mm;
	}

	recomputeBounds();
}

void cRappPointCloud::translate(int dx_mm, int dy_mm, int dz_mm)
{
	for (auto& point : mCloud)
	{
		point.x_mm += dx_mm;
		point.y_mm += dy_mm;
		point.z_mm += dz_mm;
	}

	recomputeBounds();
}

void cRappPointCloud::trim_outside(pointcloud::sBoundingBox_t box)
{
	auto cloud = pointcloud::trim_outside(mCloud, box);

	mCloud = cloud;

	recomputeBounds();
}

void cRappPointCloud::trim_below(int z_mm)
{
	auto it = std::remove_if(mCloud.begin(), mCloud.end(), [z_mm](auto a)
		{
			return a.z_mm < z_mm;
		});

	mCloud.erase(it, mCloud.end());

	recomputeBounds();
}

void cRappPointCloud::trim_above(int z_mm)
{
	auto it = std::remove_if(mCloud.begin(), mCloud.end(), [z_mm](auto a)
		{
			return a.z_mm > z_mm;
		});

	mCloud.erase(it, mCloud.end());

	recomputeBounds();
}


