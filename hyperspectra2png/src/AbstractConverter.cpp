

#include "AbstractConverter.hpp"

#include <numbers>


//-----------------------------------------------------------------------------
cAbstractConverter::cAbstractConverter(QObject* parent) :
    QObject(parent)
{
    mX_m = 0.0;
    mY_m = 0.0;
    mZ_m = 0.0;

    mVx_mps = 0.0;
    mVy_mps = 0.0;
    mVz_mps = 0.0;

    mYaw_deg = 0.0;
    mPitch_deg = 0.0;
    mRoll_deg = 0.0;

    mDollyHeight_m = 0;
    mSensorOffset_m = 0;
    mSensorMinDistance_m = 0;
    mSensorMaxDistance_m = 1000;
}

//-----------------------------------------------------------------------------
cAbstractConverter::~cAbstractConverter()
{
}

//-----------------------------------------------------------------------------
void cAbstractConverter::setDollySpeed(double Vx_mmps, double Vy_mmps, double Vz_mmps)
{
    mVx_mps = Vx_mmps * 0.001;
    mVy_mps = Vy_mmps * 0.001;
    mVz_mps = Vz_mmps * 0.001;
}

//-----------------------------------------------------------------------------
void cAbstractConverter::setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
    mYaw_deg = yaw_deg;
    mPitch_deg = pitch_deg;
    mRoll_deg = roll_deg;

    auto pitch = mPitch_deg * std::numbers::pi / 180.0;
    auto roll = mRoll_deg * std::numbers::pi / 180.0;
    auto yaw = mYaw_deg * std::numbers::pi / 180.0;

    auto c1 = mRotation.column(0);
    c1[0] = cos(yaw) * cos(pitch);
    c1[1] = sin(yaw) * cos(pitch);
    c1[2] = -sin(pitch);

    auto c2 = mRotation.column(1);
    c2[0] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
    c2[1] = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
    c2[2] = cos(pitch) * sin(roll);

    auto c3 = mRotation.column(2);
    c3[0] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
    c3[1] = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
    c3[2] = cos(pitch) * cos(roll);
}

//-----------------------------------------------------------------------------
void cAbstractConverter::setSensorMinDistance_mm(double d_mm)
{
    mSensorMinDistance_m = d_mm * 0.001;
}

//-----------------------------------------------------------------------------
void cAbstractConverter::setSensorMaxDistance_m(double d_m)
{
    mSensorMaxDistance_m = d_m;
}


