

#include "Kinematics_Constant.hpp"
#include "PointCloudSerializer.hpp"

namespace
{
    constexpr double SEC_TO_US = 1'000'000.0;
    constexpr double US_TO_SEC = 1.0 / SEC_TO_US;
}

cKinematics_Constant::cKinematics_Constant(double Vx_mmps, double Vy_mmps, double Vz_mmps)
{
    mVx_mps = Vx_mmps * 0.001;
    mVy_mps = Vy_mmps * 0.001;
    mVz_mps = Vz_mmps * 0.001;
}

void cKinematics_Constant::setHeightAxis(eHEIGHT_AXIS axis)
{
    mHeightAxis = axis;
}

void cKinematics_Constant::setInitialPosition_m(double x_m, double y_m, double z_m)
{
    mX_Offset_m = x_m;
    mY_Offset_m = y_m;
    mZ_Offset_m = z_m;
}

/*
void cKinematics_Constant::setX_Offset_m(double offset_m)
{
    mX_Offset_m = offset_m;
}

void cKinematics_Constant::setY_Offset_m(double offset_m)
{
    mY_Offset_m = offset_m;
}

void cKinematics_Constant::setZ_Offset_m(double offset_m)
{
    mZ_Offset_m = offset_m;
}
*/

//-----------------------------------------------------------------------------
void cKinematics_Constant::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::CONSTANT);
    serializer.writeKinematicSpeed(mVx_mps, mVy_mps, mVz_mps);
}

//-----------------------------------------------------------------------------
bool cKinematics_Constant::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud)
{
    double time_sec = time_us * US_TO_SEC;
    double x_m = mX_Offset_m + mVx_mps * time_sec;
    double y_m = mY_Offset_m + mVy_mps * time_sec;
    double z_m = mZ_Offset_m + mVz_mps * time_sec;

    auto cols = cloud.num_columns();
    auto rows = cloud.num_rows();

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            auto point = cloud.get(r, c);

            if ((point.X_m == 0.0) && (point.Y_m == 0.0) && (point.Z_m == 0.0))
                continue;

            if ( rotateToSEU() )
                rotate(point);
            
            if (mHeightAxis == eHEIGHT_AXIS::X)
                point.X_m = x_m - point.X_m;
            else
                point.X_m += x_m;

            if (mHeightAxis == eHEIGHT_AXIS::Y)
                point.Y_m = y_m - point.Y_m;
            else
                point.Y_m += y_m;

            if (mHeightAxis == eHEIGHT_AXIS::Z)
                point.Z_m = z_m - point.Z_m;
            else
                point.Z_m += z_m;

            cloud.set(r, c, point);
        }
    }

    return true;
}


