

#include "Kinematics_Dolly.hpp"

#include <cbdf/BlockDataFile.hpp>


cKinematics_Dolly::cKinematics_Dolly()
{
}

void cKinematics_Dolly::attachParsers(cBlockDataFileReader& file)
{
    file.attach(this);
}

void cKinematics_Dolly::detachParsers(cBlockDataFileReader& file)
{
    file.detach(this->blockID());
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::telemetryPassComplete()
{
    mDollyInfoIter = mDollyInfo.begin();
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{
    if (mDollyInfo.empty())
    {
        throw cKinematicNoData("File does not contain dolly information.");
    }

    auto lower = std::lower_bound(mDollyInfo.begin(),
        mDollyInfo.end(), time_us, [](const sDollyInfo_t& info, double time_us)
        {
            return info.timestamp_us < time_us;
        });

    auto upper = std::upper_bound(mDollyInfo.begin(),
        mDollyInfo.end(), time_us, [](double time_us, const sDollyInfo_t& info)
        {
            return time_us < info.timestamp_us;
        });
}

void cKinematics_Dolly::onPosition(spidercam::sPosition_1_t position)
{
    sDollyInfo_t info;

    if (mDollyInfo.empty())
    {
        info.timestamp_us = 0;
        info.vx_mps = 0;
        info.vy_mps = 0;
        info.vz_mps = 0;

        mX_Offset_m = position.X_mm * 0.001;
        mY_Offset_m = position.Y_mm * 0.001;
        mZ_Offset_m = position.Z_mm * 0.001;
    }
    else
    {
        auto previous = mDollyInfo.back();

        double dx = (position.X_mm * 0.001) - previous.x_m;
        double dy = (position.Y_mm * 0.001) - previous.y_m;
        double dz = (position.Z_mm * 0.001) - previous.z_m;

        double dts = position.timestamp - mLastTimestamp;
        double dt = dts * 0.0000001;

        info.timestamp_us = dt * 1'000'000.0;

        info.vx_mps = dx / dt;
        info.vy_mps = dy / dt;
        info.vz_mps = dz / dt;
    }

    info.x_m = position.X_mm * 0.001;
    info.y_m = position.Y_mm * 0.001;
    info.z_m = position.Z_mm * 0.001;

    mLastTimestamp = position.timestamp;

    mDollyInfo.push_back(info);
}


