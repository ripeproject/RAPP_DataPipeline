
#include "AerialDataModel.hpp"

#include "QuadTree.hpp"


cAerialDataModel::cAerialDataModel()
{}

cAerialDataModel::~cAerialDataModel()
{
    mAerialPoints.clear();
    mAerialMesh.clear();
}

//-----------------------------------------------------------------------------
std::size_t cAerialDataModel::numOfTriangles() const
{
    return mAerialMesh.size();
}

std::size_t cAerialDataModel::numOfAerialPoints() const
{
    return mAerialPoints.size();
}

void cAerialDataModel::clearAerialPoints()
{
    mAerialPoints.clear();
}

void cAerialDataModel::clearAerialMesh()
{
    mAerialMesh.clear();
    mQuadTree.reset();
}

void cAerialDataModel::addAerialPoint(const rfm::planePoint_t& gps_point)
{
    if (!rfb::withinBoundary(gps_point))
        return;

    auto point = rfb::toRappCoordinates(gps_point);

    mAerialPoints.push_back(point);
}

void cAerialDataModel::addAerialPoint(const rfm::rappPoint_t& rapp_point)
{
    if (!rfb::withinBoundary(rapp_point))
        return;

    mAerialPoints.push_back(rapp_point);
}

void cAerialDataModel::addAerialPoints(const std::vector<rfm::planePoint_t>& gps_points)
{
    for (const auto& gps_point : gps_points)
    {
        addAerialPoint(rfb::toRappCoordinates(gps_point));
    }
}

void cAerialDataModel::addAerialPoints(const std::vector<rfm::rappPoint_t>& rapp_points)
{
    for (const auto& rapp_point : rapp_points)
    {
        addAerialPoint(rapp_point);
    }
}

void cAerialDataModel::addMeshPoint(const rfm::rappPoint_t& p1, const rfm::rappPoint_t& p2, const rfm::rappPoint_t& p3)
{
    mAerialMesh.emplace_back(p1, p2, p3);
}

void cAerialDataModel::addMeshData(const std::vector<cRappTriangle>& mesh)
{
    if (!mQuadTree)
    {
        mQuadTree = std::make_unique<cQuadTree>(rfb::minX_mm(), rfb::maxX_mm(), rfb::minY_mm(), rfb::maxY_mm());
    }

    for (auto& triangle : mesh)
    {
        mAerialMesh.emplace_back(triangle);
        mQuadTree->addMeshData(triangle);
    }
}

const std::vector<rfm::rappPoint_t>& cAerialDataModel::getAerialPoints() const
{
    return mAerialPoints;
}

double cAerialDataModel::getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm)
{
    if (mQuadTree)
    {
        auto h = mQuadTree->height(x_mm, y_mm);

        if (h != rfm::INVALID_HEIGHT)
            return h;
    }

    for (auto triangle : mAerialMesh)
    {
        if (triangle.withinTriangle(x_mm, y_mm))
        {
            if (mQuadTree)
            {
                auto* pNode = mQuadTree->getNode(x_mm, y_mm);
                if (pNode)
                    pNode->addMeshData(triangle);
            }

            return triangle.height(x_mm, y_mm);
        }
    }

    return rfm::INVALID_HEIGHT;
}

