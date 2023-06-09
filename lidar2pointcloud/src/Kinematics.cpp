
#pragma once

#include "Kinematics.hpp"

#include <eigen3/Eigen/Eigen>

#include <numbers>

namespace
{
	template<typename T1, typename T2>
	inline void rotate(T1& x, T1& y, T1& z, const ouster::cRotationMatrix<T2>& r)
	{
		const auto& rX = r.column(0);
		const auto& rY = r.column(1);
		const auto& rZ = r.column(2);

		double a = x * rX[0] + y * rX[1] + z * rX[2];
		double b = x * rY[0] + y * rY[1] + z * rY[2];
		double c = x * rZ[0] + y * rZ[1] + z * rZ[2];

		x = static_cast<T1>(a);
		y = static_cast<T1>(b);
		z = static_cast<T1>(c);
	}
}

cKinematics::cKinematics()
{
	mSensorToSEU.identity();
}

double cKinematics::getSensorPitch_deg() const { return mPitch_deg; }
double cKinematics::getSensorRoll_deg() const { return mRoll_deg; }
double cKinematics::getSensorYaw_deg() const { return mYaw_deg; }
bool   cKinematics::rotateToSEU() const { return mRotateSensorData; }
void   cKinematics::rotateToSEU(bool apply) { mRotateSensorData = apply; }

void cKinematics::setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
	mPitch_deg = pitch_deg;
	mRoll_deg = roll_deg;
	mYaw_deg = yaw_deg;

	double pitch_rad = 0.0;
	double roll_rad = 0.0;
	double yaw_rad = 0.0;

	pitch_rad = -pitch_deg * std::numbers::pi / 180.0;
	roll_rad = -roll_deg * std::numbers::pi / 180.0;
	yaw_rad = -yaw_deg * std::numbers::pi / 180.0;

	Eigen::AngleAxisd rollAngle(roll_rad, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd yawAngle(yaw_rad, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd pitchAngle(pitch_rad, Eigen::Vector3d::UnitY());

//	Eigen::Quaternion<double> q = rollAngle * pitchAngle * yawAngle;
	Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
	Eigen::Matrix3d rotationMatrix = q.matrix();

	double e; // Used for debugging;

	auto c1 = mSensorToSEU.column(0);
	c1[0] = e = rotationMatrix.col(0)[0];
	c1[1] = e = rotationMatrix.col(0)[1];
	c1[2] = e = rotationMatrix.col(0)[2];

	auto c2 = mSensorToSEU.column(1);
	c2[0] = e = rotationMatrix.col(1)[0];
	c2[1] = e = rotationMatrix.col(1)[1];
	c2[2] = e = rotationMatrix.col(1)[2];

	auto c3 = mSensorToSEU.column(2);
	c3[0] = e = rotationMatrix.col(2)[0];
	c3[1] = e = rotationMatrix.col(2)[1];
	c3[2] = e = rotationMatrix.col(2)[2];
}

void cKinematics::setSensorOrientation(double pitch_deg, double roll_deg)
{
	setSensorOrientation(mYaw_deg, pitch_deg, roll_deg);
}

void cKinematics::rotate(float& x, float& y, float& z)
{
	if (mRotateSensorData)
		::rotate(x, y, z, mSensorToSEU);
}

void cKinematics::rotate(double& x, double& y, double& z)
{
	if (mRotateSensorData)
		::rotate(x, y, z, mSensorToSEU);
}

void cKinematics::rotate(pointcloud::sCloudPoint_t& p)
{
	if (mRotateSensorData)
		::rotate(p.X_m, p.Y_m, p.Z_m, mSensorToSEU);
}


