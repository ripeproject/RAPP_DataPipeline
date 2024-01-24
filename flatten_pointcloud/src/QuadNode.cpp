
#include "QuadNode.hpp"



cQuadNode::cQuadNode()
{}

cQuadNode::cQuadNode(double minX_mm, double maxX_mm, double minY_mm, double maxY_mm)
	: mMinX_mm(minX_mm), mMaxX_mm(maxX_mm), mMinY_mm(minY_mm), mMaxY_mm(maxY_mm)
{}

void cQuadNode::setBounds(double minX_mm, double maxX_mm, double minY_mm, double maxY_mm)
{
	mMinX_mm = minX_mm;
	mMaxX_mm = maxX_mm;
	mMinY_mm = minY_mm;
	mMaxY_mm = maxY_mm;
}

void cQuadNode::addMeshData(cRappTriangle mesh)
{
	if (mHasSubNodes)
	{
		bool moved = false;

		if (mNW->withinBounds(mesh))
		{
			mNW->addMeshData(mesh);
			moved = true;
		}

		if (mNE->withinBounds(mesh))
		{
			mNE->addMeshData(mesh);
			moved = true;
		}

		if (mSW->withinBounds(mesh))
		{
			mSW->addMeshData(mesh);
			moved = true;
		}

		if (mSE->withinBounds(mesh))
		{
			mSE->addMeshData(mesh);
			moved = true;
		}

		if (!moved)
			mGroundMesh.push_back(mesh);

		return;
	}

	mGroundMesh.push_back(mesh);

	auto n = mGroundMesh.size();

	if ((n > 100) && !mAtMinSize)
	{
		if (!addSubLevel())
			return;

		std::vector<cRappTriangle>	save;

		while (mGroundMesh.size() > 0)
		{
			auto mesh = mGroundMesh.back();
			mGroundMesh.pop_back();

			bool moved = false;

			if (mNW->withinBounds(mesh))
			{
				mNW->addMeshData(mesh);
				moved = true;
			}

			if (mNE->withinBounds(mesh))
			{
				mNE->addMeshData(mesh);
				moved = true;
			}

			if (mSW->withinBounds(mesh))
			{
				mSW->addMeshData(mesh);
				moved = true;
			}

			if (mSE->withinBounds(mesh))
			{
				mSE->addMeshData(mesh);
				moved = true;
			}

			if (!moved)
				save.push_back(mesh);
		}

		mGroundMesh = save;
	}
}

bool cQuadNode::withinBounds(const cRappTriangle& point) const
{
	if (withinBounds(point.p1()) || withinBounds(point.p2()) || withinBounds(point.p3()))
		return true;

	if (point.withinTriangle(mMinX_mm, mMinY_mm))
		return true;

	if (point.withinTriangle(mMaxX_mm, mMinY_mm))
		return true;

	if (point.withinTriangle(mMinX_mm, mMaxY_mm))
		return true;

	if (point.withinTriangle(mMaxX_mm, mMaxY_mm))
		return true;

	return false;
}

bool cQuadNode::withinBounds(const rfm::rappPoint_t& point) const
{
	return withinBounds(static_cast<int>(point.x_mm), static_cast<int>(point.y_mm));
}

bool cQuadNode::withinBounds(int x_mm, int y_mm) const
{
	if ((x_mm < mMinX_mm) || (x_mm > mMaxX_mm))
		return false;

	if ((y_mm < mMinY_mm) || (y_mm > mMaxY_mm))
		return false;

	return true;
}

double cQuadNode::height(const rfm::rappPoint_t& point) const
{
	return height(static_cast<int>(point.x_mm), static_cast<int>(point.y_mm));
}

double cQuadNode::height(int x_mm, int y_mm) const
{
	for (auto triangle : mGroundMesh)
	{
		if (triangle.withinTriangle(x_mm, y_mm))
		{
			return triangle.height(x_mm, y_mm);
		}
	}

	if (mNW)
	{
		if (mNW->withinBounds(x_mm, y_mm))
			return mNW->height(x_mm, y_mm);
	}

	if (mNE)
	{
		if (mNE->withinBounds(x_mm, y_mm))
			return mNE->height(x_mm, y_mm);
	}

	if (mSW)
	{
		if (mSW->withinBounds(x_mm, y_mm))
			return mSW->height(x_mm, y_mm);
	}

	if (mSE)
	{
		if (mSE->withinBounds(x_mm, y_mm))
			return mSE->height(x_mm, y_mm);
	}

	return rfm::INVALID_HEIGHT;
}

bool cQuadNode::addSubLevel()
{
	double dx_mm = (mMaxX_mm - mMinX_mm) / 2.0;
	double dy_mm = (mMaxY_mm - mMinY_mm) / 2.0;

	if ((dx_mm < MINIMUM_SIZE_MM) || (dy_mm < MINIMUM_SIZE_MM))
	{
		mAtMinSize = true;
		return false;
	}

	double x_mm = mMinX_mm + dx_mm;
	double y_mm = mMinY_mm + dy_mm;

	mNW.reset(new cQuadNode(mMinX_mm, x_mm, mMinY_mm, y_mm));
	mNE.reset(new cQuadNode(mMinX_mm, x_mm, y_mm, mMaxY_mm));

	mSW.reset(new cQuadNode(x_mm, mMaxX_mm, mMinY_mm, y_mm));
	mSE.reset(new cQuadNode(x_mm, mMaxX_mm, y_mm, mMaxY_mm));

	mHasSubNodes = true;

	return true;
}

