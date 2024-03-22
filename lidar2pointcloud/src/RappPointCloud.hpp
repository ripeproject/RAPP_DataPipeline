#pragma once

#include "PointCloudTypes.hpp"
#include "datatypes.hpp"
#include "PointCloud.hpp"

#include "Constants.hpp"

#include <vector>
#include <cassert>


/**
 * Class to store a point clouds generated by a LiDAR sensor.
 */
class cRappPointCloud
{
public:
	typedef std::vector<rfm::sPoint3D_t>  vCloud_t;

	typedef std::vector<rfm::sPoint3D_t>::value_type				value_type;
	typedef std::vector<rfm::sPoint3D_t>::size_type					size_type;
	typedef std::vector<rfm::sPoint3D_t>::difference_type			difference_type;
	typedef std::vector<rfm::sPoint3D_t>::reference					reference;
	typedef std::vector<rfm::sPoint3D_t>::const_reference			const_reference;
	typedef std::vector<rfm::sPoint3D_t>::pointer					pointer;
	typedef std::vector<rfm::sPoint3D_t>::const_pointer				const_pointer;
	typedef std::vector<rfm::sPoint3D_t>::iterator					iterator;
	typedef std::vector<rfm::sPoint3D_t>::const_iterator			const_iterator;
	typedef std::vector<rfm::sPoint3D_t>::reverse_iterator			reverse_iterator;
	typedef std::vector<rfm::sPoint3D_t>::const_reverse_iterator	const_reverse_iterator;

public:
	cRappPointCloud();
	cRappPointCloud(const cRappPointCloud& pc) = default;
	virtual ~cRappPointCloud() = default;

	explicit cRappPointCloud(rfm::sCentroid_t centroid, const vCloud_t& pc);

/*
	explicit cRappPointCloud(const cBasePointCloud<pointcloud::sCloudPoint_t>& pc);
	explicit cRappPointCloud(const cBasePointCloud<pointcloud::sCloudPoint_FrameID_t>& pc);
	explicit cRappPointCloud(const cBasePointCloud<pointcloud::sCloudPoint_SensorInfo_t>& pc);
*/

	cRappPointCloud& operator=(const cRappPointCloud& pc) = default;

	int id() const;

	bool hasFrameIDs() const;
	bool hasPixelInfo() const;

    void clear();

    bool empty() const;

	void reserve(size_type new_cap);
	void resize(size_type count);
	void resize(size_type count, const value_type& value);

	size_type size() const;

	void addPoint(const rfm::sPoint3D_t& cloudPoint);

	const cRappPointCloud& operator+=(const cRappPointCloud& pc);

	void sort();

	/** SpiderCam uses millimeters **/
	int minX_mm() const;
	int maxX_mm() const;

	int minY_mm() const;
	int maxY_mm() const;

	int minZ_mm() const;
	int maxZ_mm() const;

	void recomputeBounds();

	rfm::sCentroid_t centroid() const;

	rfm::sPoint3D_t getPoint(int x_mm, int y_mm, int r_mm) const;

	void rotate(double yaw_deg, double pitch_deg, double roll_deg);
	void translate(int dx_mm, int dy_mm, int dz_mm);

	void trim_outside(pointcloud::sBoundingBox_t box);

	void trim_below(int z_mm);
	void trim_above(int z_mm);

	iterator begin() { return mCloud.begin(); }
	const_iterator begin() const { return mCloud.begin(); }
	const_iterator cbegin() { return mCloud.cbegin(); }

	iterator end() { return mCloud.end(); }
	const_iterator end() const { return mCloud.end(); }
	const_iterator cend() { return mCloud.cend(); }

	reverse_iterator rbegin() { return mCloud.rbegin(); }
	const_reverse_iterator rbegin() const { return mCloud.rbegin(); }
	const_reverse_iterator crbegin() { return mCloud.crbegin(); }

	reverse_iterator rend() { return mCloud.rend(); }
	const_reverse_iterator rend() const { return mCloud.rend(); }
	const_reverse_iterator crend() { return mCloud.crend(); }

	rfm::sPoint3D_t operator[](int i) const { return mCloud[i]; }

    const vCloud_t& data() const { return mCloud; }

	void disableFrameIDs();
	void enableFrameIDs();

	void disablePixelInfo();
	void enablePixelInfo();

protected:
	template<class POINT2>
	void assign(const cBasePointCloud<POINT2>& pc);

private:
	int mID;

	int mMinX_mm = 0;
	int mMaxX_mm = 0;

	int mMinY_mm = 0;
	int mMaxY_mm = 0;

	int mMinZ_mm = 0;
	int mMaxZ_mm = 0;

	rfm::sCentroid_t mCentroid;

	bool mHasFrameIDs  = true;
	bool mHasPixelInfo = true;

	bool mEnableFrameIDs = true;
	bool mEnablePixelInfo = true;

	vCloud_t mCloud;
};


///////////////////////////////////////////////////////////////////////////////
// Implementation Details
///////////////////////////////////////////////////////////////////////////////

template<class POINT2>
void cRappPointCloud::assign(const cBasePointCloud<POINT2>& pc)
{
	mMinX_mm = static_cast<int>(pc.minX_m() * nConstants::M_TO_MM);
	mMaxX_mm = static_cast<int>(pc.maxX_m() * nConstants::M_TO_MM);

	mMinY_mm = static_cast<int>(pc.minY_m() * nConstants::M_TO_MM);
	mMaxY_mm = static_cast<int>(pc.maxY_m() * nConstants::M_TO_MM);

	mMinZ_mm = static_cast<int>(pc.minZ_m() * nConstants::M_TO_MM);
	mMaxZ_mm = static_cast<int>(pc.maxZ_m() * nConstants::M_TO_MM);

	mCloud.clear();

	auto n = pc.size();
	mCloud.resize(n);

	double sum_x = 0.0;
	double sum_y = 0.0;
	double sum_z = 0.0;

	auto& data = pc.data();
	for (std::size_t i = 0; i < n; ++i)
	{
		POINT2 point = data[i];

		sum_x += point.X_m;
		sum_y += point.Y_m;
		sum_z += point.Z_m;

		mCloud[i] = point;
	}

	double x_mm = (sum_x / n) * nConstants::M_TO_MM;
	double y_mm = (sum_y / n) * nConstants::M_TO_MM;
	double z_mm = (sum_z / n) * nConstants::M_TO_MM;

	mCentroid = { x_mm , y_mm, z_mm };
}

