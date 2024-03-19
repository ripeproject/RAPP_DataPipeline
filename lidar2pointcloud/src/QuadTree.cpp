
#include "QuadTree.hpp"



cQuadTree::cQuadTree(int minX_mm, int maxX_mm, int minY_mm, int maxY_mm)
	: mMinX_mm(minX_mm), mMaxX_mm(maxX_mm), mMinY_mm(minY_mm), mMaxY_mm(maxY_mm)
{
	double x_mm = mMinX_mm + (mMaxX_mm - mMinX_mm) / 2.0;
	double y_mm = mMinY_mm + (mMaxY_mm - mMinY_mm) / 2.0;

	mNW.reset(new cQuadNode(mMinX_mm, x_mm, mMinY_mm, y_mm));
	mNE.reset(new cQuadNode(mMinX_mm, x_mm, y_mm, mMaxY_mm));

	mSW.reset(new cQuadNode(x_mm, mMaxX_mm, mMinY_mm, y_mm));
	mSE.reset(new cQuadNode(x_mm, mMaxX_mm, y_mm, mMaxY_mm));
}

void cQuadTree::addMeshData(cRappTriangle mesh)
{
	if (mNW->withinBounds(mesh))
		mNW->addMeshData(mesh);

	if (mNE->withinBounds(mesh))
		mNE->addMeshData(mesh);

	if (mSW->withinBounds(mesh))
		mSW->addMeshData(mesh);

	if (mSE->withinBounds(mesh))
		mSE->addMeshData(mesh);

}

double cQuadTree::height(const rfm::rappPoint_t& point) const
{
	return height(static_cast<int>(point.x_mm), static_cast<int>(point.y_mm));
}

double cQuadTree::height(int x_mm, int y_mm) const
{
	if (mNW->withinBounds(x_mm, y_mm))
		return mNW->height(x_mm, y_mm);

	if (mNE->withinBounds(x_mm, y_mm))
		return mNE->height(x_mm, y_mm);

	if (mSW->withinBounds(x_mm, y_mm))
		return mSW->height(x_mm, y_mm);

	if (mSE->withinBounds(x_mm, y_mm))
		return mSE->height(x_mm, y_mm);

	return rfm::INVALID_HEIGHT;
}

cQuadNode* cQuadTree::getNode(int x_mm, int y_mm)
{
	if (mNW->withinBounds(x_mm, y_mm))
		return mNW->getNode(x_mm, y_mm);

	if (mNE->withinBounds(x_mm, y_mm))
		return mNE->getNode(x_mm, y_mm);

	if (mSW->withinBounds(x_mm, y_mm))
		return mSW->getNode(x_mm, y_mm);

	if (mSE->withinBounds(x_mm, y_mm))
		return mSE->getNode(x_mm, y_mm);

	return nullptr;
}
