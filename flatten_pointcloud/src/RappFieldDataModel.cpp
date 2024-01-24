
#include "RappFieldDataModel.hpp"

#include "Constants.hpp"

cRappFieldDataModel::cRappFieldDataModel()
{
}

cRappFieldDataModel::~cRappFieldDataModel()
{
	for (auto pc : mPointClouds)
	{
		delete pc.second;
	}
}

std::size_t cRappFieldDataModel::numOfGroundPoints() const
{
	return mGroundPoints.size();
}

const std::vector<rfm::rappPoint_t>& cRappFieldDataModel::getGroundPoints() const
{
	return mGroundPoints;
}

void cRappFieldDataModel::clear()
{
	clear_groundpoints();
	clear_mesh();
	clear_pointclouds();
}

void cRappFieldDataModel::clear_groundpoints()
{
	mMinX_mm = std::numeric_limits<int>::max();
	mMaxX_mm = std::numeric_limits<int>::min();

	mMinY_mm = std::numeric_limits<int>::max();
	mMaxY_mm = std::numeric_limits<int>::min();

	mMinZ_mm = std::numeric_limits<int>::max();
	mMaxZ_mm = std::numeric_limits<int>::min();

	mGroundPoints.clear();
}

void cRappFieldDataModel::clear_mesh()
{
	mGroundMesh.clear();
	mQuadTree.reset();
}

void cRappFieldDataModel::clear_pointclouds()
{
	for (auto pc : mPointClouds)
	{
		delete pc.second;
	}

	mPointClouds.clear();
}

void cRappFieldDataModel::append(const rfm::planePoint_t& gps_point)
{
	if (!mBoundary.withinBoundary(gps_point))
		return;

	auto point = mBoundary.toRappCoordinates(gps_point);

	mMinX_mm = std::min(mMinX_mm, point.x_mm);
	mMaxX_mm = std::max(mMaxX_mm, point.x_mm);
	mMinY_mm = std::min(mMinY_mm, point.y_mm);
	mMaxY_mm = std::max(mMaxY_mm, point.y_mm);
	mMinZ_mm = std::min(mMinZ_mm, point.z_mm);
	mMaxZ_mm = std::max(mMaxZ_mm, point.z_mm);

	mGroundPoints.push_back(point);
}

void cRappFieldDataModel::append(const rfm::rappPoint_t& rapp_point)
{
	if (!mBoundary.withinBoundary(rapp_point))
		return;

	mMinX_mm = std::min(mMinX_mm, rapp_point.x_mm);
	mMaxX_mm = std::max(mMaxX_mm, rapp_point.x_mm);
	mMinY_mm = std::min(mMinY_mm, rapp_point.y_mm);
	mMaxY_mm = std::max(mMaxY_mm, rapp_point.y_mm);
	mMinZ_mm = std::min(mMinZ_mm, rapp_point.z_mm);
	mMaxZ_mm = std::max(mMaxZ_mm, rapp_point.z_mm);

	mGroundPoints.push_back(rapp_point);
}

void cRappFieldDataModel::append(const std::vector<rfm::planePoint_t>& gps_points)
{
	for (const auto& gps_point : gps_points)
	{
		append(mBoundary.toRappCoordinates(gps_point));
	}
}

void cRappFieldDataModel::append(const std::vector<rfm::rappPoint_t>& rapp_points)
{
	for (const auto& rapp_point : rapp_points)
	{
		append(rapp_point);
	}
}

std::size_t cRappFieldDataModel::numOfPointCloud() const
{
	return mPointClouds.size();
}

void cRappFieldDataModel::addPointCloud(const cRappPointCloud& point_cloud)
{
	cRappPointCloud* pc = new cRappPointCloud(point_cloud);

	assert(pc->id() == point_cloud.id());

	mPointClouds.insert(std::make_pair(point_cloud.id(), pc));
}

std::map<int, cRappPointCloud*>::iterator cRappFieldDataModel::pcBegin()
{
	return mPointClouds.begin();
}

std::map<int, cRappPointCloud*>::const_iterator cRappFieldDataModel::pcBegin() const
{
	return mPointClouds.begin();
}

std::map<int, cRappPointCloud*>::const_iterator cRappFieldDataModel::pcCBegin() const
{
	return mPointClouds.cbegin();
}

std::map<int, cRappPointCloud*>::iterator cRappFieldDataModel::pcEnd()
{
	return mPointClouds.end();
}

std::map<int, cRappPointCloud*>::const_iterator cRappFieldDataModel::pcEnd() const
{
	return mPointClouds.end();
}

std::map<int, cRappPointCloud*>::const_iterator cRappFieldDataModel::pcCEnd()
{
	return mPointClouds.cend();
}

cRappPointCloud* const cRappFieldDataModel::pcFind(int id) const
{
	auto it = mPointClouds.find(id);

	if (it == mPointClouds.end())
		return nullptr;

	return it->second;
}

std::size_t cRappFieldDataModel::numOfTriangles() const
{
	return mGroundMesh.size();
}

void cRappFieldDataModel::addMeshPoint(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3)
{
	mGroundMesh.emplace_back(p1, p2, p3);
}

void cRappFieldDataModel::addMeshData(const std::vector<cRappTriangle>& mesh)
{
	if (!mQuadTree)
	{
		mQuadTree = std::make_unique<cQuadTree>(mMinX_mm, mMaxX_mm, mMinY_mm, mMaxY_mm);
	}

	for (auto& triangle : mesh)
	{
		mGroundMesh.emplace_back(triangle);
		mQuadTree->addMeshData(triangle);
	}
}

double cRappFieldDataModel::getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm)
{
	if (mQuadTree)
	{
		return mQuadTree->height(x_mm, y_mm);
	}
		
	for (auto triangle : mGroundMesh)
	{
		if (triangle.withinTriangle(x_mm, y_mm))
		{
			return triangle.height(x_mm, y_mm);
		}
	}

	return rfm::INVALID_HEIGHT;
}

