#pragma once

#include "PointCloud.hpp"

#include "simple_blas.h"

#include <string>

#include <QObject>
#include <QString>

//Forward Declarations
class cOusterDataFile;

class cAbstractConverter : public QObject
{
    Q_OBJECT

public:
    explicit cAbstractConverter(QObject* parent = nullptr);
    ~cAbstractConverter();

    void setDollySpeed(double Vx_mmps, double Vy_mmps, double Vz_mmps);

    // Rotation Matrix (roll, pitch, yaw)  = 
    //      cos(p)cos(r)   sin(y)sin(p)cos(r)-cos(y)sin(r)   cos(y)sin(p)cos(r)+sin(y)sin(r)
    //      cos(p)sin(r)   sin(y)sin(p)sin(r)-cos(y)cos(r)   cos(y)sin(p)sin(r)+sin(y)cos(r)
    //        -sin(p)                sin(y)cos(p)                       cos(y)cos(p)
    void setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg);

    void setSensorMinDistance_mm(double d_mm);
    void setSensorMaxDistance_m(double d_m);

    virtual void convert(cOusterDataFile& source, const std::string& destination) = 0;

signals:
    void statusMessage(QString msg);
    void errorMessage(QString title, QString msg);

protected:
    double mX_m;
    double mY_m;
    double mZ_m;

    double mDollyHeight_m;
    double mSensorMinDistance_m;
    double mSensorMaxDistance_m;
    double mSensorOffset_m;

    double mVx_mps;
    double mVy_mps;
    double mVz_mps;

    double mYaw_deg;
    double mPitch_deg;
    double mRoll_deg;

    ouster::cRotationMatrix<double> mRotation;

    cPointCloud mPointCloud;
};

