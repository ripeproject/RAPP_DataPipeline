
#include "Kinematics_GPS.hpp"
#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>


cKinematics_GPS::cKinematics_GPS()
{
}

void cKinematics_GPS::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::GPS);
}

//-----------------------------------------------------------------------------
void cKinematics_GPS::attachKinematicParsers(cBlockDataFileReader& file)
{
    file.attach(this);
}

void cKinematics_GPS::detachKinematicParsers(cBlockDataFileReader& file)
{
    file.detach(this->blockID());
}

//-----------------------------------------------------------------------------
void cKinematics_GPS::attachTransformParsers(cBlockDataFileReader& file)
{
}

void cKinematics_GPS::detachTransformParsers(cBlockDataFileReader& file)
{
}

//-----------------------------------------------------------------------------
void cKinematics_GPS::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{

}



