#pragma once

#include "RappTriangle.hpp"
#include "QuadNode.hpp"

#include <memory>

/**
 */
class cQuadTree
{
public:
	cQuadTree(int minX_mm, int maxX_mm, int minY_mm, int maxY_mm);
	virtual ~cQuadTree() = default;

	void addMeshData(cRappTriangle mesh);

	double height(const rfm::rappPoint_t& point) const;
	double height(int x_mm, int y_mm) const;

private:
	int	mMinX_mm = 0;
	int mMaxX_mm = 0;
	int mMinY_mm = 0;
	int mMaxY_mm = 0;

	std::unique_ptr<cQuadNode>	mNW;
	std::unique_ptr<cQuadNode>	mNE;
	std::unique_ptr<cQuadNode>	mSW;
	std::unique_ptr<cQuadNode>	mSE;
};


