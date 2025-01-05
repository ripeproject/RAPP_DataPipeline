#pragma once

#include "datatypes.hpp"

#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include <string>
#include <vector>
#include <map>
#include <memory>

//Forward Declarations
class cQuadTree;

class cGroundDataModel
{
public:
	cGroundDataModel();
	~cGroundDataModel();

	void clearGroundPoints();
	void clearGroundMesh();

	std::size_t numOfTriangles() const;
	std::size_t numOfGroundPoints() const;

	void addGroundPoint(const rfm::planePoint_t& gps_point);
	void addGroundPoint(const rfm::rappPoint_t& rapp_point);
	void addGroundPoints(const std::vector<rfm::planePoint_t>& gps_points);
	void addGroundPoints(const std::vector<rfm::rappPoint_t>& rapp_points);

	void addMeshPoint(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3);
	void addMeshData(const std::vector<cRappTriangle>& mesh);

	const std::vector<rfm::rappPoint_t>& getGroundPoints() const;
	double getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm);

private:
	std::vector<rfm::rappPoint_t> mGroundPoints;
	std::vector<cRappTriangle>	  mGroundMesh;
	std::unique_ptr<cQuadTree>	  mQuadTree;
};

