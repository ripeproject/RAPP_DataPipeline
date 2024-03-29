
#include "lidar2pointcloud.hpp"
#include "PointCloudTypes.hpp"

#include <ouster/simple_blas.h>
#include <ouster/ouster_utils.h>

#include <eigen3/Eigen/Eigen>

#include <cassert>
#include <algorithm>
#include <numbers>
#include <cmath>
#include <valarray>
#include <iostream>

using namespace ouster;

namespace
{
	/** Unit of range from sensor packet, in meters. */
	constexpr double range_unit_m = 0.001;
	constexpr double range_unit_mm = 1.0;

	constexpr double m_to_mm = 1000.0;
	constexpr double mm_to_m = 0.001;

	constexpr double g_mps2 = 9.80665;

	double min_encoder_rad = 0.0;
	double max_encoder_rad = 2.0 * std::numbers::pi;

	double min_altitude_rad = -std::numbers::pi;
	double max_altitude_rad = +std::numbers::pi;


	/** Lookup table of beam directions and offsets. */
	struct sXYZ_Lut_t
	{
		std::vector<bool>	  exclude;
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
	inline void rotate(std::vector<sPoint_t>& lhs, const ouster::cRotationMatrix<T>& r)
	{
		const auto& rX = r.column(0);
		const auto& rY = r.column(1);
		const auto& rZ = r.column(2);

		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			sPoint_t p = lhs[i];

			double x = p.x * rX[0] + p.y * rX[1] + p.z * rX[2];
			double y = p.x * rY[0] + p.y * rY[1] + p.z * rY[2];
			double z = p.x * rZ[0] + p.y * rZ[1] + p.z * rZ[2];

			lhs[i] = sPoint_t(x, y, z);
		}
	}

	template<typename T>
	inline void translate(std::vector<sPoint_t>& lhs, const ouster::cTranslation<T>& t)
	{
		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			sPoint_t& p = lhs[i];

			if ((p.x == 0.0) && (p.y == 0.0) && (p.z == 0.0))
			{
				continue;
			}
			p.x += t.x();
			p.y += t.y();
			p.z += t.z();
		}
	}

	void set_x(std::vector<sPoint_t>& lhs, const std::valarray<double>& x)
	{
		assert(lhs.size() == x.size());
		for (std::size_t i = 0; i < x.size(); ++i)
			lhs[i].x = x[i];
	}

	void set_y(std::vector<sPoint_t>& lhs, const std::valarray<double>& y)
	{
		assert(lhs.size() == y.size());
		for (std::size_t i = 0; i < y.size(); ++i)
			lhs[i].y = y[i];
	}

	void set_z(std::vector<sPoint_t>& lhs, const std::valarray<double>& z)
	{
		assert(lhs.size() == z.size());
		for (std::size_t i = 0; i < z.size(); ++i)
			lhs[i].z = z[i];
	}

	void operator*=(std::vector<sPoint_t>& lhs, const double rhs)
	{
		for (std::size_t i = 0; i < lhs.size(); ++i)
		{
			lhs[i].x *= rhs;
			lhs[i].y *= rhs;
			lhs[i].z *= rhs;
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
		lut.exclude.resize(w * h);
		lut.direction.resize(w * h);
		lut.offset.resize(w * h);

		auto n = encoder_rad.size();
		for (size_t i = 0; i < n; ++i)
		{
			double encoder  = encoder_rad[i];
			double altitude = altitude_rad[i];
			double azimuth  = azimuth_rad[i];

			if (((min_encoder_rad < encoder) && (encoder < max_encoder_rad))
				&& ((min_altitude_rad < altitude) && (altitude < max_altitude_rad)))
			{
				auto altitude_cos = cos(altitude);
				auto x = cos(encoder + azimuth) * altitude_cos;
				auto y = sin(encoder + azimuth) * altitude_cos;
				auto z = sin(altitude);

				lut.exclude[i] = false;

				lut.direction[i].x = x;
				lut.direction[i].y = y;
				lut.direction[i].z = z;

				lut.offset[i].x = cos(encoder) - x;
				lut.offset[i].y = sin(encoder) - y;
				lut.offset[i].z = -z;
			}
			else
			{
				lut.exclude[i] = true;
				lut.direction[i].x = 0.0;
				lut.direction[i].y = 0.0;
				lut.direction[i].z = 0.0;

				lut.offset[i].x = 0.0;
				lut.offset[i].y = 0.0;
				lut.offset[i].z = 0.0;
			}
		}

		// offsets due to beam origin
		lut.offset *= lidar_origin_to_beam_origin_mm;

/*
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
*/

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


///////////////////////////////////////////////////////////////////////////////
// S t a t i c   V a r i a b l e s   a n d   M e t h o d s
///////////////////////////////////////////////////////////////////////////////

double cLidar2PointCloud::mMinDistance_m = 0.001;
double cLidar2PointCloud::mMaxDistance_m = 1000.0;

bool cLidar2PointCloud::mAggregatePointCloud = false;
bool cLidar2PointCloud::mSaveReducedPointCloud = false;


void cLidar2PointCloud::setValidRange_m(double min_dist_m, double max_dist_m)
{
	mMinDistance_m = std::max(min_dist_m, 0.001);
	mMaxDistance_m = std::min(max_dist_m, 1000.0);
}

void cLidar2PointCloud::setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg)
{
	min_encoder_rad = min_azimuth_deg * std::numbers::pi / 180.0;
	max_encoder_rad = max_azimuth_deg * std::numbers::pi / 180.0;
}

void cLidar2PointCloud::setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg)
{
	min_altitude_rad = min_altitude_deg * std::numbers::pi / 180.0;
	max_altitude_rad = max_altitude_deg * std::numbers::pi / 180.0;
}

void cLidar2PointCloud::saveAggregatePointCloud()
{
	mAggregatePointCloud = true;
}

void cLidar2PointCloud::saveReducedPointCloud()
{
	mSaveReducedPointCloud = true;
}


///////////////////////////////////////////////////////////////////////////////
// M a i n   C l a s s
///////////////////////////////////////////////////////////////////////////////

cLidar2PointCloud::cLidar2PointCloud() : cPointCloudSerializer(1024)
{
	mKinematic = std::make_unique<cKinematics_None>();
}

cLidar2PointCloud::~cLidar2PointCloud()
{
}

void cLidar2PointCloud::setKinematicModel(std::unique_ptr<cKinematics> model)
{
	mKinematic.reset(model.release());
}

const cKinematics* cLidar2PointCloud::getcKinematicModel() const
{
	return mKinematic.get();
}

bool cLidar2PointCloud::requiresTelemetryPass()
{
	assert(mKinematic);

	return mKinematic->requiresTelemetryPass();
}

void cLidar2PointCloud::telemetryPassComplete()
{
	assert(mKinematic);

	mKinematic->telemetryPassComplete();
}

void cLidar2PointCloud::attachKinematicParsers(cBlockDataFileReader& file)
{
	mKinematic->attachKinematicParsers(file);
}

void cLidar2PointCloud::detachKinematicParsers(cBlockDataFileReader& file)
{
	mKinematic->detachKinematicParsers(file);
}

void cLidar2PointCloud::attachTransformParsers(cBlockDataFileReader& file)
{
	mKinematic->attachTransformParsers(file);
}

void cLidar2PointCloud::detachTransformParsers(cBlockDataFileReader& file)
{
	mKinematic->detachTransformParsers(file);
}

void cLidar2PointCloud::attachTransformSerializers(cBlockDataFileWriter& file)
{
	mKinematic->attachTransformSerializers(file);
}

void cLidar2PointCloud::detachTransformSerializers(cBlockDataFileWriter& file)
{
	mKinematic->detachTransformSerializers(file);
}

void cLidar2PointCloud::writeHeader()
{
	if (mKinematic->rotateToSEU())
		write(pointcloud::eCOORDINATE_SYSTEM::SENSOR_SEU);
	else
		write(pointcloud::eCOORDINATE_SYSTEM::SENSOR);

	writeSensorAngles(mKinematic->getSensorPitch_deg(), 
		mKinematic->getSensorRoll_deg(), mKinematic->getSensorYaw_deg());

	mKinematic->writeHeader(*this);
}

void cLidar2PointCloud::writeAndClearData()
{
	if (mPointCloud.empty()) return;

	writeDimensions(mPointCloud.minX(), mPointCloud.maxX(),
		mPointCloud.minY(), mPointCloud.maxY(),
		mPointCloud.minZ(), mPointCloud.maxZ());

	write(mPointCloud);
	mPointCloud.clear();
}

void cLidar2PointCloud::createXyzLookupTable(const beam_intrinsics_2_t& beam,
    const lidar_intrinsics_2_t& lidar, const lidar_data_format_2_t& format)
{
    cTransformMatrix<double> transform(lidar.lidar_to_sensor_transform, true);

    auto xyz = make_xyz_lut(format.columns_per_frame, format.pixels_per_column, range_unit_mm,
        beam.lidar_to_beam_origins_mm, transform, beam.azimuth_angles_deg, beam.altitude_angles_deg);

	mExcludePoint = xyz.exclude;
    mUnitVectors = xyz.direction;
    mOffsets = xyz.offset;

	write(pointcloud::eCOORDINATE_SYSTEM::SENSOR_ENU);
}

void cLidar2PointCloud::computePointCloud(const cOusterLidarData& data)
{
	double minDistance_mm = mMinDistance_m * m_to_mm;
	double maxDistance_mm = mMaxDistance_m * m_to_mm;

	auto frameID = data.frame_id();
	auto timestamp_ns = data.timestamp_ns();

	if (mStartTimestamp_ns == 0)
		mStartTimestamp_ns = timestamp_ns;

	auto time_us = static_cast<double>(timestamp_ns - mStartTimestamp_ns)/1000.0;

    auto columns_per_frame = data.columnsPerFrame();
    auto pixels_per_column = data.pixelsPerColumn();
    mCloud.resize(pixels_per_column, columns_per_frame);

    // Their example code seems to indicate that we need to destagger the image, but
    // that does not seem to be true!
    //auto lidar_data = destagger(data, mPixelShiftByRow);
    auto lidar_data = ouster::to_matrix_row_major(data.data());

	pointcloud::sCloudPoint_t point;

    for (int c = 0; c < columns_per_frame; ++c)
    {
        auto column = lidar_data.column(c);
        for (int p = 0; p < pixels_per_column; ++p)
        {
            std::size_t i = p * columns_per_frame + c;

			auto range_mm = column[p].range_mm;

			if (mExcludePoint[i])
			{
				point.X_m = 0.0;
				point.Y_m = 0.0;
				point.Z_m = 0.0;
			}
			else
			{
				if ((range_mm < minDistance_mm) || (range_mm > maxDistance_mm))
				{
					point.X_m = 0.0;
					point.Y_m = 0.0;
					point.Z_m = 0.0;
				}
				else
				{
					auto unit_vec = mUnitVectors[i];
					auto offset = mOffsets[i];

					double x_mm = unit_vec.x * range_mm;
					double y_mm = unit_vec.y * range_mm;
					double z_mm = unit_vec.z * range_mm;

					x_mm += offset.x;
					y_mm += offset.y;
					z_mm += offset.z;

					point.X_m = x_mm * mm_to_m;
					point.Y_m = y_mm * mm_to_m;
					point.Z_m = z_mm * mm_to_m;
				}
			}

            point.range_mm = range_mm;
            point.signal = column[p].signal;
            point.reflectivity = column[p].reflectivity;
            point.nir = column[p].nir;

            mCloud.set(p, c, point);
        }
    }

	mKinematic->transform(time_us, mCloud);

	if (mAggregatePointCloud)
	{
		for (int c = 0; c < columns_per_frame; ++c)
		{
			auto column = mCloud.column(c);
			for (int p = 0; p < pixels_per_column; ++p)
			{
				auto point = column[p];

				mPointCloud.addPoint(point);
			}
		}

		if (mKinematic->atEndOfPass())
		{
			writeAndClearData();
		}
	}
	else if (mSaveReducedPointCloud)
	{
		cReducedPointCloudByFrame pc;
		pc.frameID(frameID);
		pc.timestamp_ns(timestamp_ns);

		for (int c = 0; c < columns_per_frame; ++c)
		{
			auto column = mCloud.column(c);
			for (int p = 0; p < pixels_per_column; ++p)
			{
				auto point = column[p];
				pc.addPoint(point);
			}
		}

		writeDimensions(pc.minX(), pc.maxX(), pc.minY(),
			pc.maxY(), pc.minZ(), pc.maxZ());

		write(pc);
	}
	else
	{
		cSensorPointCloudByFrame pc;
		pc.frameID(frameID);
		pc.timestamp_ns(timestamp_ns);
		pc.resize(mCloud.num_rows(), mCloud.num_columns());

		for (int c = 0; c < columns_per_frame; ++c)
		{
			auto column = mCloud.column(c);
			for (int p = 0; p < pixels_per_column; ++p)
			{
				auto point = column[p];
				pc.set(c, p, point);
			}
		}

		writeDimensions(pc.minX(), pc.maxX(), pc.minY(),
			pc.maxY(), pc.minZ(), pc.maxZ());

		write(pc);
	}
}

void cLidar2PointCloud::onConfigParam(ouster::config_param_2_t config_param) {}
void cLidar2PointCloud::onSensorInfo(ouster::sensor_info_2_t sensor_info) {}
void cLidar2PointCloud::onTimestamp(ouster::timestamp_2_t timestamp) {}
void cLidar2PointCloud::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) {}
void cLidar2PointCloud::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) {}
void cLidar2PointCloud::onMultipurposeIo(ouster::multipurpose_io_2_t io) {}
void cLidar2PointCloud::onNmea(ouster::nmea_2_t nmea) {}
void cLidar2PointCloud::onTimeInfo(ouster::time_info_2_t time_info) {}

void cLidar2PointCloud::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics)
{
	mBeamIntrinsics = intrinsics;

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidar2PointCloud::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics)
{
	mImuIntrinsics = intrinsics;
	mImuTransform.set(mImuIntrinsics.imu_to_sensor_transform, true);
	mImuToSensor = mImuTransform.rotation();
}

void cLidar2PointCloud::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics)
{
	mLidarIntrinsics = intrinsics;

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidar2PointCloud::onLidarDataFormat(ouster::lidar_data_format_2_t format)
{
	mLidarDataFormat = format;

	mPixelShiftByRow = format.pixel_shift_by_row;
	for (int i = 0; i < mPixelShiftByRow.size(); ++i)
		mPixelShiftByRow[i] -= 12;

	setBufferCapacity(static_cast<std::size_t>(format.pixels_per_column) *
		static_cast<std::size_t>(format.columns_per_frame) *
		sizeof(pointcloud::sCloudPoint_t) + 32);

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidar2PointCloud::onImuData(ouster::imu_data_t data)
{
	pointcloud::imu_data_t imu;
	imu.accelerometer_read_time_ns = data.accelerometer_read_time_ns;
	imu.gyroscope_read_time_ns = data.gyroscope_read_time_ns;
	imu.acceleration_X_g = data.acceleration_Xaxis_g;
	imu.acceleration_Y_g = data.acceleration_Yaxis_g;
	imu.acceleration_Z_g = data.acceleration_Zaxis_g;
	imu.angular_velocity_Xaxis_deg_per_sec = data.angular_velocity_Xaxis_deg_per_sec;
	imu.angular_velocity_Yaxis_deg_per_sec = data.angular_velocity_Yaxis_deg_per_sec;
	imu.angular_velocity_Zaxis_deg_per_sec = data.angular_velocity_Zaxis_deg_per_sec;

	rotate(imu.acceleration_X_g, imu.acceleration_Y_g, imu.acceleration_Z_g, mImuToSensor);

	mKinematic->rotate(imu.acceleration_X_g, imu.acceleration_Y_g, imu.acceleration_Z_g);

	rotate(imu.angular_velocity_Xaxis_deg_per_sec, imu.angular_velocity_Yaxis_deg_per_sec,
		imu.angular_velocity_Zaxis_deg_per_sec, mImuToSensor);

	mKinematic->rotate(imu.angular_velocity_Xaxis_deg_per_sec, imu.angular_velocity_Yaxis_deg_per_sec,
		imu.angular_velocity_Zaxis_deg_per_sec);

	write(imu);
}

void cLidar2PointCloud::onLidarData(cOusterLidarData data)
{
	if (mUnitVectors.empty())
	{
		return;
	}

	computePointCloud(data);
}

