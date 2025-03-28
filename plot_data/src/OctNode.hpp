#pragma once

#include <vector>
#include <memory>


/**
 * An Octal Container Node.  The node stores the number of points in the node volume.
 */
class cOctNode
{
public:

public:
	cOctNode(double minX_mm, double maxX_mm, double minY_mm, double maxY_mm, double minZ_mm, double maxZ_mm, double size_mm);
	virtual ~cOctNode() = default;

	double volume_mm3(int min_voxel_count) const;

	void addPoint(int x_mm, int y_mm, int z_mm);

	bool withinBounds(int x_mm, int y_mm, int z_mm) const;

	cOctNode* getNode(int x_mm, int y_mm, int z_mm);

protected:
	bool addSubLevel();

private:
	const double MINIMUM_SIZE_MM = 1;

	double mMinX_mm = 0;
	double mMaxX_mm = 0;
	double mMinY_mm = 0;
	double mMaxY_mm = 0;
	double mMinZ_mm = 0;
	double mMaxZ_mm = 0;

	double mSize_mm = MINIMUM_SIZE_MM;

	bool mHasSubNodes = false;
	bool mAtMinSize   = false;

	std::unique_ptr<cOctNode>	mNW_U;
	std::unique_ptr<cOctNode>	mNW_L;

	std::unique_ptr<cOctNode>	mNE_U;
	std::unique_ptr<cOctNode>	mNE_L;

	std::unique_ptr<cOctNode>	mSW_U;
	std::unique_ptr<cOctNode>	mSW_L;

	std::unique_ptr<cOctNode>	mSE_U;
	std::unique_ptr<cOctNode>	mSE_L;

	int	mNumOfPoints = 0;
};


