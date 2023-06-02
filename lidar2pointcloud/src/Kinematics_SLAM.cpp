

#include "Kinematics_SLAM.hpp"
#include "PointCloudSerializer.hpp"


cKinematics_SLAM::cKinematics_SLAM()
{
}

void cKinematics_SLAM::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::SLAM);
}

//-----------------------------------------------------------------------------
void cKinematics_SLAM::attachKinematicParsers(cBlockDataFileReader& file)
{
}

void cKinematics_SLAM::detachKinematicParsers(cBlockDataFileReader& file)
{
}

//-----------------------------------------------------------------------------
void cKinematics_SLAM::attachTransformParsers(cBlockDataFileReader& file)
{
}

void cKinematics_SLAM::detachTransformParsers(cBlockDataFileReader& file)
{
}

//-----------------------------------------------------------------------------
void cKinematics_SLAM::attachTransformSerializers(cBlockDataFileWriter& file)
{
}

void cKinematics_SLAM::detachTransformSerializers(cBlockDataFileWriter& file)
{
}

//-----------------------------------------------------------------------------
void cKinematics_SLAM::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{

}


