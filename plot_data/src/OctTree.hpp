#pragma once

#include "OctNode.hpp"

#include <memory>


/**
 */
class cOctTree
{
public:
	cOctTree(int minX_mm, int maxX_mm, int minY_mm, int maxY_mm, int minZ_mm, int maxZ_mm, double size_mm);
	virtual ~cOctTree() = default;

	void addPoint(int x_mm, int y_mm, int z_mm);

	double volume_mm3(int min_voxel_count) const;

	cOctNode* getNode(int x_mm, int y_mm, int z_mm);

private:
	int	mMinX_mm = 0;
	int mMaxX_mm = 0;
	int mMinY_mm = 0;
	int mMaxY_mm = 0;
	int mMinZ_mm = 0;
	int mMaxZ_mm = 0;

	std::unique_ptr<cOctNode>	mNW_U;
	std::unique_ptr<cOctNode>	mNW_L;
	std::unique_ptr<cOctNode>	mNE_U;
	std::unique_ptr<cOctNode>	mNE_L;
	std::unique_ptr<cOctNode>	mSW_U;
	std::unique_ptr<cOctNode>	mSW_L;
	std::unique_ptr<cOctNode>	mSE_U;
	std::unique_ptr<cOctNode>	mSE_L;
};


