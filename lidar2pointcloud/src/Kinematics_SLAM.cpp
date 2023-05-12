

#include "Kinematics_SLAM.hpp"
#include "PointCloudSerializer.hpp"


cKinematics_SLAM::cKinematics_SLAM()
{
}

void cKinematics_SLAM::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::SLAM);
}

void cKinematics_SLAM::attachParsers(cBlockDataFileReader& file)
{
}

void cKinematics_SLAM::detachParsers(cBlockDataFileReader& file)
{
}

//-----------------------------------------------------------------------------
void cKinematics_SLAM::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{

}


