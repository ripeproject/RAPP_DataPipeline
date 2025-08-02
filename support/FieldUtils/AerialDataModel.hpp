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

class cAerialDataModel
{
public:
	cAerialDataModel();
	~cAerialDataModel();

	void clearAerialPoints();
	void clearAerialMesh();

	std::size_t numOfTriangles() const;
	std::size_t numOfAerialPoints() const;

	void addAerialPoint(const rfm::planePoint_t& gps_point);
	void addAerialPoint(const rfm::rappPoint_t& rapp_point);
	void addAerialPoints(const std::vector<rfm::planePoint_t>& gps_points);
	void addAerialPoints(const std::vector<rfm::rappPoint_t>& rapp_points);

	void addMeshPoint(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3);
	void addMeshData(const std::vector<cRappTriangle>& mesh);

	const std::vector<rfm::rappPoint_t>& getAerialPoints() const;
	double getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm);
	double getDollyOffset_mm(std::int32_t x_mm, std::int32_t y_mm, std::int32_t ref_height_mm);

private:
	std::vector<rfm::rappPoint_t> mAerialPoints;
	std::vector<cRappTriangle>	  mAerialMesh;
	std::unique_ptr<cQuadTree>	  mQuadTree;
};

