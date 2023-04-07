

#include "Kinematics_Constant.hpp"


cKinematics_Constant::cKinematics_Constant(double Vx_mmps, double Vy_mmps, double Vz_mmps)
{
    mVx_mps = Vx_mmps * 0.001;
    mVy_mps = Vy_mmps * 0.001;
    mVz_mps = Vz_mmps * 0.001;
}

//-----------------------------------------------------------------------------
void cKinematics_Constant::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{
    double time_sec = time_us / 1000000.0;
    double x_m = mVx_mps * time_sec;
    double y_m = mVy_mps * time_sec;
    double z_m = mVz_mps * time_sec;

    auto cols = cloud.num_columns();
    auto rows = cloud.num_rows();

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            auto point = cloud.get(r, c);

            if ((point.X_m == 0.0) && (point.Y_m == 0.0) && (point.Z_m == 0.0))
                continue;

            point.X_m += x_m;
            point.Y_m += y_m;
            point.Z_m += z_m;

            cloud.set(r, c, point);
        }
    }

}


