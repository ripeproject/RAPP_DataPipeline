
#include "LidarStatistics.hpp"

#include <ouster/simple_blas.h>

#include <eigen3/Eigen/Eigen>

#include <algorithm>
#include <numbers>
#include <cmath>
#include <valarray>
#include <iostream>
#include <fmt/core.h>

using namespace ouster;

namespace
{
	/** Unit of range from sensor packet, in meters. */
	constexpr double range_unit_m = 0.001;
	constexpr double range_unit_mm = 1.0;

	constexpr double m_to_mm = 1000.0;
	constexpr double mm_to_m = 0.001;

	constexpr double g_mps2 = 9.80665;

	/** Lookup table of beam directions and offsets. */
	struct sXYZ_Lut_t
	{
		std::vector<sPoint_t> direction;
		std::vector<sPoint_t> offset;
	};

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

	template<typename T>
	inline void rotate(sPoint_t& p, const ouster::cRotationMatrix<T>& r)
	{
		rotate(p.x_mm, p.y_mm, p.z_mm, r);
	}

	template<typename T>
	inline void rotate(sCloudPoint_t& p, const ouster::cRotationMatrix<T>& r)
	{
		rotate(p.x_m, p.y_m, p.z_m, r);
	}

	template<typename T>
	inline void rotate(std::vector<sPoint_t>& lhs, const ouster::cRotationMatrix<T>& r)
	{
		const auto& rX = r.column(0);
		const auto& rY = r.column(1);
		const auto& rZ = r.column(2);

		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			sPoint_t p = lhs[i];

			double x = p.x_mm * rX[0] + p.y_mm * rX[1] + p.z_mm * rX[2];
			double y = p.x_mm * rY[0] + p.y_mm * rY[1] + p.z_mm * rY[2];
			double z = p.x_mm * rZ[0] + p.y_mm * rZ[1] + p.z_mm * rZ[2];

			lhs[i] = sPoint_t(x, y, z);
		}
	}

	template<typename T>
	inline void translate(std::vector<sPoint_t>& lhs, const ouster::cTranslation<T>& t)
	{
		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			sPoint_t& p = lhs[i];

			p.x_mm += t.x();
			p.y_mm += t.y();
			p.z_mm += t.z();
		}
	}

	void set_x(std::vector<sPoint_t>& lhs, const std::valarray<double>& x)
	{
		assert(lhs.size() == x.size());
		for (std::size_t i = 0; i < x.size(); ++i)
			lhs[i].x_mm = x[i];
	}

	void set_y(std::vector<sPoint_t>& lhs, const std::valarray<double>& y)
	{
		assert(lhs.size() == y.size());
		for (std::size_t i = 0; i < y.size(); ++i)
			lhs[i].y_mm = y[i];
	}

	void set_z(std::vector<sPoint_t>& lhs, const std::valarray<double>& z)
	{
		assert(lhs.size() == z.size());
		for (std::size_t i = 0; i < z.size(); ++i)
			lhs[i].z_mm = z[i];
	}

	void operator*=(std::vector<sPoint_t>& lhs, const double rhs)
	{
		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			lhs[i].x_mm *= rhs;
			lhs[i].y_mm *= rhs;
			lhs[i].z_mm *= rhs;
		}
	}

	sXYZ_Lut_t make_xyz_lut(size_t w, size_t h, double range_unit, double lidar_origin_to_beam_origin_mm,
		const ouster::cTransformMatrix<double>& transform,
		const std::vector<double>& azimuth_angles_deg,
		const std::vector<double>& altitude_angles_deg)
	{
		if (w <= 0 || h <= 0)
			throw std::invalid_argument("lut dimensions must be greater than zero");

		if (azimuth_angles_deg.size() != h || altitude_angles_deg.size() != h)
			throw std::invalid_argument("unexpected scan dimensions");

		std::valarray<double> encoder_rad(w * h);   // theta_e
		std::valarray<double> azimuth_rad(w * h);   // theta_a
		std::valarray<double> altitude_rad(w * h);  // phi

		const double azimuth_radians = std::numbers::pi * 2.0 / w;

		// populate angles for each pixel
		for (size_t v = 0; v < w; v++)
		{
			for (size_t u = 0; u < h; u++)
			{
				size_t i = u * w + v;
				encoder_rad[i] = 2.0 * std::numbers::pi - (v * azimuth_radians);
				azimuth_rad[i] = -azimuth_angles_deg[u] * std::numbers::pi / 180.0;
				altitude_rad[i] = altitude_angles_deg[u] * std::numbers::pi / 180.0;
			}
		}

		sXYZ_Lut_t lut;

		auto altitude_cos = cos(altitude_rad);
		auto x = cos(encoder_rad + azimuth_rad) * altitude_cos;
		auto y = sin(encoder_rad + azimuth_rad) * altitude_cos;
		auto z = sin(altitude_rad);

		// unit vectors for each pixel
		lut.direction.resize(w * h);
		set_x(lut.direction, x);
		set_y(lut.direction, y);
		set_z(lut.direction, z);

		// offsets due to beam origin
		lut.offset.resize(w * h);
		set_x(lut.offset, cos(encoder_rad) - x);
		set_y(lut.offset, sin(encoder_rad) - y);
		set_z(lut.offset, -z);
		lut.offset *= lidar_origin_to_beam_origin_mm;

		// apply the supplied transform
		auto rot = transform.rotation();
		auto trans = transform.translation();

		rotate(lut.direction, rot);
		rotate(lut.offset, rot);

		translate(lut.offset, trans);

		// apply scaling factor
		lut.direction *= range_unit;
		lut.offset *= range_unit;

		return lut;
	}
}

double cLidarStatistics::mMinDistance_m = 0.001;
double cLidarStatistics::mMaxDistance_m = 1000.0;
ouster::cRotationMatrix<double> cLidarStatistics::mSensorToENU;

void cLidarStatistics::setValidRange_m(double min_dist_m, double max_dist_m)
{
	mMinDistance_m = std::max(min_dist_m, 0.001);
	mMaxDistance_m = std::min(max_dist_m, 1000.0);
}

void cLidarStatistics::setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
	auto pitch = -pitch_deg * std::numbers::pi / 180.0;
	auto roll = -roll_deg * std::numbers::pi / 180.0;
	auto yaw = -yaw_deg * std::numbers::pi / 180.0;

	Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());

	Eigen::Quaternion<double> q = rollAngle * pitchAngle * yawAngle;
	Eigen::Matrix3d rotationMatrix = q.matrix();

	double e; // Used for debugging;

	auto c1 = mSensorToENU.column(0);
	c1[0] = e = rotationMatrix.col(0)[0];
	c1[1] = e = rotationMatrix.col(0)[1];
	c1[2] = e = rotationMatrix.col(0)[2];

	auto c2 = mSensorToENU.column(1);
	c2[0] = e = rotationMatrix.col(1)[0];
	c2[1] = e = rotationMatrix.col(1)[1];
	c2[2] = e = rotationMatrix.col(1)[2];

	auto c3 = mSensorToENU.column(2);
	c3[0] = e = rotationMatrix.col(2)[0];
	c3[1] = e = rotationMatrix.col(2)[1];
	c3[2] = e = rotationMatrix.col(2)[2];

/*
	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

	double e; // Used for debugging;

	auto c1 = mSensorToENU.column(0);
	c1[0] = e = cos(yaw) * cos(pitch);
	c1[1] = e = sin(yaw) * cos(pitch);
	c1[2] = e = -sin(pitch);

	auto c2 = mSensorToENU.column(1);
	c2[0] = e = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
	c2[1] = e = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
	c2[2] = e = cos(pitch) * sin(roll);

	auto c3 = mSensorToENU.column(2);
	c3[0] = e = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
	c3[1] = e = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
	c3[2] = e = cos(pitch) * cos(roll);
*/
}


cLidarStatistics::cLidarStatistics()
{
	for (int i = 0; i < 20; ++i)
	{
		mImuHistogram[i] = 0;
	}
}

cLidarStatistics::~cLidarStatistics()
{
	std::string filename = mBaseFilename + ".imu.histogram.csv";
	auto imu_histogram = mOutDir / filename;

	std::ofstream histogram;
	histogram.open(imu_histogram);

	for (int i = 0; i < 20; ++i)
	{
		histogram << i << ", " << mImuHistogram[i] << "\n";
	}

	histogram.close();


	filename = mBaseFilename + ".imu.averages.csv";
	auto imu_averages = mOutDir / filename;

	std::ofstream averages;
	averages.open(imu_averages);

	averages << "Average Accelerations:\n";
	averages << "X (g), Y (g), Z (g)\n";

	mAvg_X_g /= mCount;
	mAvg_Y_g /= mCount;
	mAvg_Z_g /= mCount;

	averages << mAvg_X_g << ", " << mAvg_Y_g << ", " << mAvg_Z_g << "\n\n";

	averages << "Average Orientation Rates:\n";
	averages << "Pitch Rate (deg/sec), Roll Rate (deg/sec), Yaw Rate (deg/sec)\n";

	mAvgPitchRate_deg_per_sec /= mCount;
	mAvgRollRate_deg_per_sec /= mCount;
	mAvgYawRate_deg_per_sec /= mCount;

	averages << mAvgPitchRate_deg_per_sec << ", " << mAvgRollRate_deg_per_sec << ", " << mAvgYawRate_deg_per_sec << std::endl;

	averages.close();
}

void cLidarStatistics::setOutputPath(std::filesystem::path path)
{
	mBaseFilename = path.filename().string();
	mOutDir = path.parent_path();

	std::string filename = mBaseFilename + ".imu.accelerations.csv";
	auto accelerations = mOutDir / filename;

	filename = mBaseFilename + ".imu.orientation_rates.csv";
	auto orientationRates = mOutDir / filename;

	filename = mBaseFilename + ".positions.csv";
	auto positions = mOutDir / filename;

	mPosition.open(positions);
	mAccelerations.open(accelerations);
	mOrientationRates.open(orientationRates);
}

void cLidarStatistics::createXyzLookupTable(const beam_intrinsics_2_t& beam,
    const lidar_intrinsics_2_t& lidar, const lidar_data_format_2_t& format)
{
    cTransformMatrix<double> transform(lidar.lidar_to_sensor_transform, true);

    auto xyz = make_xyz_lut(format.columns_per_frame, format.pixels_per_column, range_unit_mm,
        beam.lidar_to_beam_origins_mm, transform, beam.azimuth_angles_deg, beam.altitude_angles_deg);

    mUnitVectors = xyz.direction;
    mOffsets = xyz.offset;
}


void cLidarStatistics::computerPointCloud(const cOusterLidarData& data)
{
	double minDistance_mm = mMinDistance_m * m_to_mm;
	double maxDistance_mm = mMaxDistance_m * m_to_mm;

	auto frameID = data.frame_id();
	auto timestamp_ns = data.timestamp_ns();

	if (mLidarStartTimestamp_ns == 0)
	{
		mLidarStartTimestamp_ns = timestamp_ns;
	}

	auto time_ms = static_cast<double>(timestamp_ns - mLidarStartTimestamp_ns)/1000000.0;

    auto columns_per_frame = data.columnsPerFrame();
    auto pixels_per_column = data.pixelsPerColumn();
    mCloud.resize(pixels_per_column, columns_per_frame);

    // Their example code seems to indicate that we need to destagger the image, but
    // that does not seem to be true!
    //    auto lidar_data = destagger(data, mPixelShiftByRow);
    auto lidar_data = ouster::to_matrix_row_major(data.data());

	sCloudPoint_t point;

    for (int c = 0; c < columns_per_frame; ++c)
    {
        auto column = lidar_data.column(c);
        for (int p = 0; p < pixels_per_column; ++p)
        {
            std::size_t i = p * columns_per_frame + c;

            auto unit_vec = mUnitVectors[i];
            auto offset = mOffsets[i];

            auto range_mm = column[p].range_mm;

			if ((range_mm < minDistance_mm) || (range_mm > maxDistance_mm))
			{
				point.x_m = 0.0;
				point.y_m = 0.0;
				point.z_m = 0.0;
				point.r_m = 0.0;
			}
			else
			{
				double x_mm = unit_vec.x_mm * range_mm;
				double y_mm = unit_vec.y_mm * range_mm;
				double z_mm = unit_vec.z_mm * range_mm;

				x_mm += offset.x_mm;
				y_mm += offset.y_mm;
				z_mm += offset.z_mm;

				point.x_m = x_mm * mm_to_m;
				point.y_m = y_mm * mm_to_m;
				point.z_m = z_mm * mm_to_m;
				point.r_m = range_mm * mm_to_m;

				rotate(point, mSensorToENU);

				if (p == mCenterRow && c == mCenterCol)
				{
					mPosition << frameID << ", " 
						<< fmt::format("{:10.3f}", time_ms)
						<< ", " << point.x_m << ", "
						<< point.y_m << ", " << point.z_m << ", "
						<< point.r_m << "\n";
				}
			}

            mCloud.set(p, c, point);
        }
    }
}

void cLidarStatistics::onConfigParam(ouster::config_param_2_t config_param) {}
void cLidarStatistics::onSensorInfo(ouster::sensor_info_2_t sensor_info) {}
void cLidarStatistics::onTimestamp(ouster::timestamp_2_t timestamp) {}
void cLidarStatistics::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) {}
void cLidarStatistics::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) {}
void cLidarStatistics::onMultipurposeIo(ouster::multipurpose_io_2_t io) {}
void cLidarStatistics::onNmea(ouster::nmea_2_t nmea) {}
void cLidarStatistics::onTimeInfo(ouster::time_info_2_t time_info) {}

void cLidarStatistics::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics)
{
	mBeamIntrinsics = intrinsics;

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidarStatistics::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics)
{
	mImuIntrinsics = intrinsics;
	mImuTransform.set(mImuIntrinsics.imu_to_sensor_transform, true);
	mImuToSensor = mImuTransform.rotation();
//	auto trans = mImuTransform.translation();
}

void cLidarStatistics::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics)
{
	mLidarIntrinsics = intrinsics;

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidarStatistics::onLidarDataFormat(ouster::lidar_data_format_2_t format)
{
	mLidarDataFormat = format;

	mCenterCol = (format.columns_per_frame / 2) - 1;
	mCenterRow = (format.pixels_per_column / 2) - 1;

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidarStatistics::onImuData(ouster::imu_data_t data)
{
	++mImuCount;

	auto accelerometer_read_time_ns = data.accelerometer_read_time_ns;
	auto gyroscope_read_time_ns = data.gyroscope_read_time_ns;

	if (mAccelStartTimestamp_ns == 0)
	{
		mAccelStartTimestamp_ns = accelerometer_read_time_ns;

		mAccelerations << "Time (ms), X (g), Y (g), Z (g)\n";
	}

	auto accel_time_ms = static_cast<double>(accelerometer_read_time_ns - mAccelStartTimestamp_ns) / 1000000.0;

	if (mGyroStartTimestamp_ns == 0)
	{
		mGyroStartTimestamp_ns = gyroscope_read_time_ns;

		mOrientationRates << "Time (ms), Pitch Rate (deg/sec), Roll Rate (deg/sec), Yaw Rate (deg/sec)\n";

	}

	auto gyro_time_ms = static_cast<double>(gyroscope_read_time_ns - mGyroStartTimestamp_ns) / 1000000.0;

	double acceleration_X_g = data.acceleration_Xaxis_g;
	double acceleration_Y_g = data.acceleration_Yaxis_g;
	double acceleration_Z_g = data.acceleration_Zaxis_g;
	double angular_velocity_Xaxis_deg_per_sec = data.angular_velocity_Xaxis_deg_per_sec;
	double angular_velocity_Yaxis_deg_per_sec = data.angular_velocity_Yaxis_deg_per_sec;
	double angular_velocity_Zaxis_deg_per_sec = data.angular_velocity_Zaxis_deg_per_sec;

	rotate(acceleration_X_g, acceleration_Y_g, acceleration_Z_g, mImuToSensor);
	rotate(acceleration_X_g, acceleration_Y_g, acceleration_Z_g, mSensorToENU);

	rotate(angular_velocity_Xaxis_deg_per_sec,
		angular_velocity_Yaxis_deg_per_sec,
		angular_velocity_Zaxis_deg_per_sec, mImuToSensor);

	rotate(angular_velocity_Xaxis_deg_per_sec, 
			angular_velocity_Yaxis_deg_per_sec,
			angular_velocity_Zaxis_deg_per_sec, mSensorToENU);

	mX_g = acceleration_X_g;
	mY_g = acceleration_Y_g;
	mZ_g = acceleration_Z_g;

	mAccelerations << fmt::format("{:10.3f}", accel_time_ms) << ", " << mX_g << ", " << mY_g << ", " << mZ_g << "\n";

	mPitchRate_deg_per_sec = angular_velocity_Yaxis_deg_per_sec;
	mRollRate_deg_per_sec = angular_velocity_Xaxis_deg_per_sec;
	mYawRate_deg_per_sec = angular_velocity_Zaxis_deg_per_sec;

	mOrientationRates << fmt::format("{:10.3f}", gyro_time_ms) << ", "
		<< mPitchRate_deg_per_sec << ", " << mRollRate_deg_per_sec << ", " 
		<< mYawRate_deg_per_sec << "\n";

	++mCount;
	mAvg_X_g += mX_g;
	mAvg_Y_g += mY_g;
	mAvg_Z_g += mZ_g;
	mAvgPitchRate_deg_per_sec += mPitchRate_deg_per_sec;
	mAvgRollRate_deg_per_sec += mRollRate_deg_per_sec;
	mAvgYawRate_deg_per_sec += mYawRate_deg_per_sec;
}

void cLidarStatistics::onLidarData(cOusterLidarData data)
{
	int n = mImuHistogram[mImuCount] + 1;
	mImuHistogram[mImuCount] = n;

	mImuCount = 0;

	if (mUnitVectors.empty())
	{
		return;
	}

	computerPointCloud(data);
}

