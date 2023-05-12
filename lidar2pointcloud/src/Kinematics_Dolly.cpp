

#include "Kinematics_Dolly.hpp"
#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>


cKinematics_Dolly::cKinematics_Dolly()
{
}

void cKinematics_Dolly::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::DOLLY);
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
    mDollyInfoIndex = 0;
    mDollyInfoMax = 0;

    if (!mDollyInfo.empty())
    {
        sDollyInfo_t info = mDollyInfo.front();

        // The dolly x movement is the north/south direction
        mSouth_Offset_m = info.x_m;

        // The dolly y movement is the east/west direction
        mEast_Offset_m = info.y_m;

        mHeight_Offset_m = info.z_m;

        mDollyInfoMax = mDollyInfo.size() - 1;
    }
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{
    if (mDollyInfo.empty())
    {
        throw cKinematicNoData("File does not contain dolly information.");
    }

    auto lower = mDollyInfoIndex;
    while (lower < mDollyInfoMax)
    {
        if (time_us < mDollyInfo[lower+1].timestamp_us)
            break;
        ++lower;
    }

    auto upper = std::upper_bound(mDollyInfo.begin(),
        mDollyInfo.end(), time_us, [](double time_us, const sDollyInfo_t& info)
        {
            return time_us < info.timestamp_us;
        });

    if (lower != mDollyInfoIndex)
    {
        mDollyInfoIndex = lower;

        sDollyInfo_t info = mDollyInfo[mDollyInfoIndex];

        // The dolly x movement is the north/south direction
        mSouth_Offset_m = info.x_m;

        // The dolly y movement is the east/west direction
        mEast_Offset_m = info.y_m;

        mHeight_Offset_m = info.z_m;
    }

    sDollyInfo_t info = mDollyInfo[mDollyInfoIndex];

    double time_sec = (time_us - info.timestamp_us) / 1000000.0;

    // The dolly x movement is the north/south direction
    mSouth_m = mSouth_Offset_m + info.vx_mps * time_sec;

    // The dolly y movement is the east/west direction
    mEast_m = mEast_Offset_m + info.vy_mps * time_sec;

    mHeight_m = mHeight_Offset_m + info.vz_mps * time_sec;

    auto cols = cloud.num_columns();
    auto rows = cloud.num_rows();

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            auto point = cloud.get(r, c);

            if ((point.X_m == 0.0) && (point.Y_m == 0.0) && (point.Z_m == 0.0))
                continue;

            point.X_m += mSouth_m;
            point.Y_m += mEast_m;
            point.Z_m = mHeight_m - point.Z_m;

            cloud.set(r, c, point);
        }
    }
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

        mStartTimestamp = position.timestamp;

    }
    else
    {
        auto& previous = mDollyInfo.back();

        double dx = (position.X_mm * 0.001) - previous.x_m;
        double dy = (position.Y_mm * 0.001) - previous.y_m;
        double dz = (position.Z_mm * 0.001) - previous.z_m;

        double dts = position.timestamp - mStartTimestamp;
        double dt = dts * 0.0000001;

        info.timestamp_us = dt * 1'000'000.0;

        previous.vx_mps = dx / dt;
        previous.vy_mps = dy / dt;
        previous.vz_mps = dz / dt;
    }

    info.x_m = position.X_mm * 0.001;
    info.y_m = position.Y_mm * 0.001;
    info.z_m = position.Z_mm * 0.001;

    mDollyInfo.push_back(info);
}


