
#include "OctTree.hpp"


cOctTree::cOctTree(int minX_mm, int maxX_mm, int minY_mm, int maxY_mm, int minZ_mm, int maxZ_mm, double size_mm)
	: mMinX_mm(minX_mm), mMaxX_mm(maxX_mm), mMinY_mm(minY_mm), mMaxY_mm(maxY_mm), mMinZ_mm(minZ_mm), mMaxZ_mm(maxZ_mm)
{
	double x_mm = mMinX_mm + (mMaxX_mm - mMinX_mm) / 2.0;
	double y_mm = mMinY_mm + (mMaxY_mm - mMinY_mm) / 2.0;
	double z_mm = mMinZ_mm + (mMaxZ_mm - mMinZ_mm) / 2.0;

	mNW_L.reset(new cOctNode(mMinX_mm, x_mm, mMinY_mm, y_mm, mMinZ_mm, z_mm, size_mm));
	mNW_U.reset(new cOctNode(mMinX_mm, x_mm, mMinY_mm, y_mm, z_mm, mMaxZ_mm, size_mm));

	mNE_L.reset(new cOctNode(mMinX_mm, x_mm, y_mm, mMaxY_mm, mMinZ_mm, z_mm, size_mm));
	mNE_U.reset(new cOctNode(mMinX_mm, x_mm, y_mm, mMaxY_mm, z_mm, mMaxZ_mm, size_mm));

	mSW_L.reset(new cOctNode(x_mm, mMaxX_mm, mMinY_mm, y_mm, mMinZ_mm, z_mm, size_mm));
	mSW_U.reset(new cOctNode(x_mm, mMaxX_mm, mMinY_mm, y_mm, z_mm, mMaxZ_mm, size_mm));

	mSE_L.reset(new cOctNode(x_mm, mMaxX_mm, y_mm, mMaxY_mm, mMinZ_mm, z_mm, size_mm));
	mSE_U.reset(new cOctNode(x_mm, mMaxX_mm, y_mm, mMaxY_mm, z_mm, mMaxZ_mm, size_mm));
}

void cOctTree::addPoint(int x_mm, int y_mm, int z_mm)
{
	if (mNW_L->withinBounds(x_mm, y_mm, z_mm))
		mNW_L->addPoint(x_mm, y_mm, z_mm);

	if (mNW_U->withinBounds(x_mm, y_mm, z_mm))
		mNW_U->addPoint(x_mm, y_mm, z_mm);

	if (mNE_L->withinBounds(x_mm, y_mm, z_mm))
		mNE_L->addPoint(x_mm, y_mm, z_mm);

	if (mNE_U->withinBounds(x_mm, y_mm, z_mm))
		mNE_U->addPoint(x_mm, y_mm, z_mm);

	if (mSW_L->withinBounds(x_mm, y_mm, z_mm))
		mSW_L->addPoint(x_mm, y_mm, z_mm);

	if (mSW_U->withinBounds(x_mm, y_mm, z_mm))
		mSW_U->addPoint(x_mm, y_mm, z_mm);

	if (mSE_L->withinBounds(x_mm, y_mm, z_mm))
		mSE_L->addPoint(x_mm, y_mm, z_mm);

	if (mSE_U->withinBounds(x_mm, y_mm, z_mm))
		mSE_U->addPoint(x_mm, y_mm, z_mm);
}


double cOctTree::volume_mm3(int min_voxel_count) const
{
	double volume = 0.0;

	if (mNW_L)
		volume += mNW_L->volume_mm3(min_voxel_count);

	if (mNW_U)
		volume += mNW_U->volume_mm3(min_voxel_count);

	if (mNE_L)
		volume += mNE_L->volume_mm3(min_voxel_count);

	if (mNE_U)
		volume += mNE_U->volume_mm3(min_voxel_count);

	if (mSW_L)
		volume += mSW_L->volume_mm3(min_voxel_count);

	if (mSW_U)
		volume += mSW_U->volume_mm3(min_voxel_count);

	if (mSE_L)
		volume += mSE_L->volume_mm3(min_voxel_count);

	if (mSE_U)
		volume += mSE_U->volume_mm3(min_voxel_count);

	return volume;
}


cOctNode* cOctTree::getNode(int x_mm, int y_mm, int z_mm)
{
	if (mNW_L->withinBounds(x_mm, y_mm, z_mm))
		return mNW_L->getNode(x_mm, y_mm, z_mm);

	if (mNW_U->withinBounds(x_mm, y_mm, z_mm))
		return mNW_U->getNode(x_mm, y_mm, z_mm);

	if (mNE_L->withinBounds(x_mm, y_mm, z_mm))
		return mNE_L->getNode(x_mm, y_mm, z_mm);

	if (mNE_U->withinBounds(x_mm, y_mm, z_mm))
		return mNE_U->getNode(x_mm, y_mm, z_mm);

	if (mSW_L->withinBounds(x_mm, y_mm, z_mm))
		return mSW_L->getNode(x_mm, y_mm, z_mm);

	if (mSW_U->withinBounds(x_mm, y_mm, z_mm))
		return mSW_U->getNode(x_mm, y_mm, z_mm);

	if (mSE_L->withinBounds(x_mm, y_mm, z_mm))
		return mSE_L->getNode(x_mm, y_mm, z_mm);

	if (mSE_U->withinBounds(x_mm, y_mm, z_mm))
		return mSE_U->getNode(x_mm, y_mm, z_mm);

	return nullptr;
}
