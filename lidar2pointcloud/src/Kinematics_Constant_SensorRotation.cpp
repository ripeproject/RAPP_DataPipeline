

#include "Kinematics_Constant_SensorRotation.hpp"
#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <fstream>


namespace
{
    constexpr double SEC_TO_US = 1'000'000.0;
    constexpr double US_TO_SEC = 1.0 / SEC_TO_US;

    constexpr double SEC_TO_NS = 1'000'000'000.0;
    constexpr double NS_TO_SEC = 1.0 / SEC_TO_NS;
}

cKinematics_Constant_SensorRotation::cKinematics_Constant_SensorRotation(double Vx_mmps, double Vy_mmps, double Vz_mmps)
    : cKinematics_Constant(Vx_mmps, Vy_mmps, Vz_mmps)
{
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::setInitialOrientation_deg(double yaw_deg, double pitch_deg, double roll_deg)
{
    mStartPitch_deg = pitch_deg;
    mStartRoll_deg = roll_deg;
    mStartYaw_deg = yaw_deg;
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::setFinalOrientation_deg(double yaw_deg, double pitch_deg, double roll_deg)
{
    mEndPitch_deg = pitch_deg;
    mEndRoll_deg = roll_deg;
    mEndYaw_deg = yaw_deg;
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::addOrientationPoint(double time_sec, double yaw_deg, double pitch_deg, double roll_deg)
{
    sSensorOrientation_t point;
    
    point.timestamp_us = time_sec * SEC_TO_US;
    point.pitch_deg = pitch_deg;
    point.roll_deg  = roll_deg;
    point.yaw_deg   = yaw_deg;

    mSensorInfo.push_back(point);
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::setRotationalRates_deg(double yawRate_dps, double pitchRate_dps, double rollRate_dps)
{
    mHaveRotationalRates = true;
    mPitchRate_dps = pitchRate_dps;
    mRollRate_dps = rollRate_dps;
    mYawRate_dps = yawRate_dps;
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::attachKinematicParsers(cBlockDataFileReader& file)
{
    file.attach(static_cast<cOusterParser*>(this));
}

void cKinematics_Constant_SensorRotation::detachKinematicParsers(cBlockDataFileReader& file)
{
    file.detach(static_cast<cOusterParser*>(this)->blockID());
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::CONSTANT_SENSOR_ROTATION);
    serializer.writeKinematicSpeed(Vx_mps(), Vy_mps(), Vz_mps());
    serializer.writeSensorAngles(mStartPitch_deg, mStartRoll_deg, mStartYaw_deg);
    serializer.writeSensorRotationalSpeeds(mPitchRate_dps, mRollRate_dps, mYawRate_dps);
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::telemetryPassComplete()
{
    if (mHaveRotationalRates) return;

    double total_time_sec = (mLastTimestamp_ns - mStartTimestamp_ns) * NS_TO_SEC;

    mPitchRate_dps = (mEndPitch_deg - mStartPitch_deg) / total_time_sec;
    mRollRate_dps  = (mEndRoll_deg - mStartRoll_deg) / total_time_sec;
    mYawRate_dps   = (mEndYaw_deg - mStartYaw_deg) / total_time_sec;

    if (mImuCount > 0)
    {
        mImuData.pitchRate_dps /= mImuCount;
        mImuData.rollRate_dps /= mImuCount;
        mImuData.yawRate_dps /= mImuCount;
    }

    double time2 = total_time_sec * total_time_sec;

    mPitchAccel_dps2 = 2.0 * (mEndPitch_deg - mStartPitch_deg) / time2;
    mRollAccel_dps2 = 2.0 * (mEndRoll_deg - mStartRoll_deg) / time2;
    mYawAccel_dps2 = 2.0 * (mEndYaw_deg - mStartYaw_deg) / time2;

    if (mSensorInfo.size() > 1)
    {
        for (auto& info : mSensorInfo)
        { 
            if (info.timestamp_us < 0.0)
                info.timestamp_us = total_time_sec * SEC_TO_US;
        }

        std::sort(mSensorInfo.begin(), mSensorInfo.end(), [](sSensorOrientation_t a, sSensorOrientation_t b)
            {
                if (a.timestamp_us < b.timestamp_us) return true;

                return false;
            });

        for (std::size_t i = 1; i < mSensorInfo.size(); ++i)
        {
            sSensorOrientation_t& prev          = mSensorInfo[i - 1];
            const sSensorOrientation_t& curr    = mSensorInfo[i];

            double dtime_sec = (curr.timestamp_us - prev.timestamp_us) * US_TO_SEC;

            prev.pitchRate_dps  = (curr.pitch_deg - prev.pitch_deg) / dtime_sec;
            prev.rollRate_dps   = (curr.roll_deg - prev.roll_deg) / dtime_sec;
            prev.yawRate_dps    = (curr.yaw_deg - prev.yaw_deg) / dtime_sec;
        }
    }
};

//-----------------------------------------------------------------------------
bool cKinematics_Constant_SensorRotation::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud)
{
    double time_sec = time_us * US_TO_SEC;

    double pitch_deg = mStartPitch_deg;
    double roll_deg = mStartRoll_deg;
    double yaw_deg = mStartYaw_deg;

    if (!mSensorInfo.empty())
    {
        auto info = mSensorInfo.front();

        for (auto i : mSensorInfo)
        {
            if (i.timestamp_us > time_us) break;
            info = i;
        }

        double dtime_sec = (time_us - info.timestamp_us) * US_TO_SEC;

        pitch_deg   = info.pitch_deg + info.pitchRate_dps * dtime_sec;
        roll_deg    = info.roll_deg  + info.rollRate_dps * dtime_sec;
        yaw_deg     = info.yaw_deg   + info.yawRate_dps * dtime_sec;
    }
    else
    {
        pitch_deg   += mPitchRate_dps * time_sec;
        roll_deg    += mRollRate_dps * time_sec;
        yaw_deg     += mYawRate_dps * time_sec;
    }

    setSensorOrientation(yaw_deg, pitch_deg, roll_deg);

    if (mDebugActive)
        mDebugFile << time_sec << ", " << yaw_deg << ", " << getSensorYaw_deg() << std::endl;

    return cKinematics_Constant::transform(time_us, cloud);
}

//-----------------------------------------------------------------------------
void cKinematics_Constant_SensorRotation::onConfigParam(ouster::config_param_2_t config_param) {}
void cKinematics_Constant_SensorRotation::onSensorInfo(ouster::sensor_info_2_t sensor_info) {}
void cKinematics_Constant_SensorRotation::onTimestamp(ouster::timestamp_2_t timestamp) {}
void cKinematics_Constant_SensorRotation::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) {}
void cKinematics_Constant_SensorRotation::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) {}
void cKinematics_Constant_SensorRotation::onMultipurposeIo(ouster::multipurpose_io_2_t io) {}
void cKinematics_Constant_SensorRotation::onNmea(ouster::nmea_2_t nmea) {}
void cKinematics_Constant_SensorRotation::onTimeInfo(ouster::time_info_2_t time_info) {}
void cKinematics_Constant_SensorRotation::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) {}
void cKinematics_Constant_SensorRotation::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics) {}

void cKinematics_Constant_SensorRotation::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) {};
void cKinematics_Constant_SensorRotation::onLidarDataFormat(ouster::lidar_data_format_2_t format) {};

void cKinematics_Constant_SensorRotation::onImuData(ouster::imu_data_t data)
{
    mImuData.timestamp_ns = data.gyroscope_read_time_ns;

    mImuData.pitchRate_dps += data.angular_velocity_Yaxis_deg_per_sec;
    mImuData.rollRate_dps += data.angular_velocity_Zaxis_deg_per_sec;
    mImuData.yawRate_dps += data.angular_velocity_Xaxis_deg_per_sec;

    ++mImuCount;
}

void cKinematics_Constant_SensorRotation::onLidarData(cOusterLidarData data) 
{
    mLastTimestamp_ns = data.timestamp_ns();

    if (mStartTimestamp_ns == 0)
        mStartTimestamp_ns = mLastTimestamp_ns;
}

