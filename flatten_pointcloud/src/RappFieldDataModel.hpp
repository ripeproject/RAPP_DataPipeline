
#pragma once

#include "datatypes.hpp"
#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include "RappPointCloud.hpp"
#include "QuadTree.hpp"

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward Declarations


class cRappFieldDataModel
{
public:
	cRappFieldDataModel();
	~cRappFieldDataModel();

	void clear();
	void clear_groundpoints();
	void clear_mesh();
	void clear_pointclouds();

	void append(const rfm::planePoint_t& gps_point);
	void append(const rfm::rappPoint_t& rapp_point);
	void append(const std::vector<rfm::planePoint_t>& gps_points);
	void append(const std::vector<rfm::rappPoint_t>& rapp_points);

	std::size_t numOfGroundPoints() const;
	const std::vector<rfm::rappPoint_t>& getGroundPoints() const;

	std::size_t numOfPointCloud() const;
	void addPointCloud(const cRappPointCloud& point_cloud);

	std::map<int, cRappPointCloud*>::iterator		pcBegin();
	std::map<int, cRappPointCloud*>::const_iterator pcBegin() const;
	std::map<int, cRappPointCloud*>::const_iterator pcCBegin() const;

	std::map<int, cRappPointCloud*>::iterator		pcEnd();
	std::map<int, cRappPointCloud*>::const_iterator pcEnd() const;
	std::map<int, cRappPointCloud*>::const_iterator pcCEnd();

	cRappPointCloud* const pcFind(int id) const;

	std::size_t numOfTriangles() const;
	void addMeshPoint(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3);
	void addMeshData(const std::vector<cRappTriangle>& mesh);

	double getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm);

private:
	cRappFieldBoundary mBoundary;

	int mMinX_mm = std::numeric_limits<int>::max();
	int mMaxX_mm = std::numeric_limits<int>::min();

	int mMinY_mm = std::numeric_limits<int>::max();
	int mMaxY_mm = std::numeric_limits<int>::min();

	int mMinZ_mm = std::numeric_limits<int>::max();
	int mMaxZ_mm = std::numeric_limits<int>::min();

	std::vector<rfm::rappPoint_t> mGroundPoints;
	std::vector<cRappTriangle>	  mGroundMesh;

	std::map<int, cRappPointCloud*> mPointClouds;

	std::unique_ptr<cQuadTree> mQuadTree;
};

