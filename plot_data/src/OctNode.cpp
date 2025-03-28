
#include "OctNode.hpp"



cOctNode::cOctNode(double minX_mm, double maxX_mm, double minY_mm, double maxY_mm, double minZ_mm, double maxZ_mm, double size_mm)
	: mMinX_mm(minX_mm), mMaxX_mm(maxX_mm), mMinY_mm(minY_mm), mMaxY_mm(maxY_mm), mMinZ_mm(minZ_mm), mMaxZ_mm(maxZ_mm)
{
	mSize_mm = std::max(MINIMUM_SIZE_MM, size_mm);

	double avg_size_mm = ((mMaxX_mm - mMinX_mm) + (mMaxY_mm - mMinY_mm) + (mMaxZ_mm - mMinZ_mm)) / 3.0;

	mAtMinSize = avg_size_mm <= size_mm;
}

void cOctNode::addPoint(int x_mm, int y_mm, int z_mm)
{
	if (mAtMinSize)
	{
		++mNumOfPoints;
		return;
	}

	if (!mHasSubNodes)
	{
		if (!addSubLevel())
		{
			++mNumOfPoints;
			return;
		}
	}

	if (mHasSubNodes)
	{
		if (mNW_L->withinBounds(x_mm, y_mm, z_mm))
		{
			mNW_L->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mNW_U->withinBounds(x_mm, y_mm, z_mm))
		{
			mNW_U->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mNE_L->withinBounds(x_mm, y_mm, z_mm))
		{
			mNE_L->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mNE_U->withinBounds(x_mm, y_mm, z_mm))
		{
			mNE_U->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mSW_L->withinBounds(x_mm, y_mm, z_mm))
		{
			mSW_L->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mSW_U->withinBounds(x_mm, y_mm, z_mm))
		{
			mSW_U->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mSE_L->withinBounds(x_mm, y_mm, z_mm))
		{
			mSE_L->addPoint(x_mm, y_mm, z_mm);
			return;
		}

		if (mSE_U->withinBounds(x_mm, y_mm, z_mm))
		{
			mSE_U->addPoint(x_mm, y_mm, z_mm);
			return;
		}
	}
}

double cOctNode::volume_mm3(int min_voxel_count) const
{
	double volume = 0.0;

	if (mAtMinSize)
	{
		if (mNumOfPoints > min_voxel_count)
			volume = (mMaxX_mm - mMinX_mm) * (mMaxY_mm - mMinY_mm) * (mMaxZ_mm - mMinZ_mm);

		return volume;
	}

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


bool cOctNode::withinBounds(int x_mm, int y_mm, int z_mm) const
{
	if ((x_mm < mMinX_mm) || (x_mm > mMaxX_mm))
		return false;

	if ((y_mm < mMinY_mm) || (y_mm > mMaxY_mm))
		return false;

	if ((z_mm < mMinZ_mm) || (z_mm > mMaxZ_mm))
		return false;

	return true;
}

cOctNode* cOctNode::getNode(int x_mm, int y_mm, int z_mm)
{
	if (mNW_L)
	{
		if (mNW_L->withinBounds(x_mm, y_mm, z_mm))
			return mNW_L->getNode(x_mm, y_mm, z_mm);
	}

	if (mNW_U)
	{
		if (mNW_U->withinBounds(x_mm, y_mm, z_mm))
			return mNW_U->getNode(x_mm, y_mm, z_mm);
	}

	if (mNE_L)
	{
		if (mNE_L->withinBounds(x_mm, y_mm, z_mm))
			return mNE_L->getNode(x_mm, y_mm, z_mm);
	}

	if (mNE_U)
	{
		if (mNE_U->withinBounds(x_mm, y_mm, z_mm))
			return mNE_U->getNode(x_mm, y_mm, z_mm);
	}

	if (mSW_L)
	{
		if (mSW_L->withinBounds(x_mm, y_mm, z_mm))
			return mSW_L->getNode(x_mm, y_mm, z_mm);
	}

	if (mSW_U)
	{
		if (mSW_U->withinBounds(x_mm, y_mm, z_mm))
			return mSW_U->getNode(x_mm, y_mm, z_mm);
	}

	if (mSE_L)
	{
		if (mSE_L->withinBounds(x_mm, y_mm, z_mm))
			return mSE_L->getNode(x_mm, y_mm, z_mm);
	}

	if (mSE_U)
	{
		if (mSE_U->withinBounds(x_mm, y_mm, z_mm))
			return mSE_U->getNode(x_mm, y_mm, z_mm);
	}

	return this;
}

bool cOctNode::addSubLevel()
{
	double dx_mm = mMaxX_mm - mMinX_mm;
	double dy_mm = mMaxY_mm - mMinY_mm;
	double dz_mm = mMaxZ_mm - mMinZ_mm;

	if ((dx_mm <= mSize_mm) || (dy_mm <= mSize_mm) || (dz_mm <= mSize_mm))
	{
		mAtMinSize = true;
		return false;
	}

	double x_mm = mMinX_mm + dx_mm / 2.0;
	double y_mm = mMinY_mm + dy_mm / 2.0;
	double z_mm = mMinZ_mm + dz_mm / 2.0;

	mNW_L.reset(new cOctNode(mMinX_mm, x_mm, mMinY_mm, y_mm, mMinZ_mm, z_mm, mSize_mm));
	mNW_U.reset(new cOctNode(mMinX_mm, x_mm, mMinY_mm, y_mm, z_mm, mMaxZ_mm, mSize_mm));

	mNE_L.reset(new cOctNode(mMinX_mm, x_mm, y_mm, mMaxY_mm, mMinZ_mm, z_mm, mSize_mm));
	mNE_U.reset(new cOctNode(mMinX_mm, x_mm, y_mm, mMaxY_mm, z_mm, mMaxZ_mm, mSize_mm));

	mSW_L.reset(new cOctNode(x_mm, mMaxX_mm, mMinY_mm, y_mm, mMinZ_mm, z_mm, mSize_mm));
	mSW_U.reset(new cOctNode(x_mm, mMaxX_mm, mMinY_mm, y_mm, z_mm, mMaxZ_mm, mSize_mm));

	mSE_L.reset(new cOctNode(x_mm, mMaxX_mm, y_mm, mMaxY_mm, mMinZ_mm, z_mm, mSize_mm));
	mSE_U.reset(new cOctNode(x_mm, mMaxX_mm, y_mm, mMaxY_mm, z_mm, mMaxZ_mm, mSize_mm));

	mHasSubNodes = true;

	return true;
}

