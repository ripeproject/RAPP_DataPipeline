
#include "lidar2pointcloud.hpp"

#include <ouster/simple_blas.h>

#include <algorithm>
#include <numbers>
#include <cmath>
#include <valarray>

using namespace ouster;

namespace
{
	/** Unit of range from sensor packet, in meters. */
	constexpr double range_unit_m = 0.001;
	constexpr double range_unit_mm = 1.0;

	constexpr double m_to_mm = 1000.0;
	constexpr double mm_to_m = 0.001;

	/** Lookup table of beam directions and offsets. */
	struct sXYZ_Lut_t
	{
		std::vector<sPoint_t> direction;
		std::vector<sPoint_t> offset;
	};

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

double cLidar2PointCloud::mMinDistance_m = 0.0;
double cLidar2PointCloud::mMaxDistance_m = 1000.0;
ouster::cRotationMatrix<double> cLidar2PointCloud::mSensorToENU;

void cLidar2PointCloud::setValidRange_m(double min_dist_m, double max_dist_m)
{
	mMinDistance_m = std::max(min_dist_m, 0.0);
	mMaxDistance_m = std::min(max_dist_m, 1000.0);
}

void cLidar2PointCloud::setSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
{
	auto pitch = pitch_deg * std::numbers::pi / 180.0;
	auto roll = roll_deg * std::numbers::pi / 180.0;
	auto yaw = yaw_deg * std::numbers::pi / 180.0;

	auto c1 = mSensorToENU.column(0);
	c1[0] = cos(yaw) * cos(pitch);
	c1[1] = sin(yaw) * cos(pitch);
	c1[2] = -sin(pitch);

	auto c2 = mSensorToENU.column(1);
	c2[0] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
	c2[1] = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
	c2[2] = cos(pitch) * sin(roll);

	auto c3 = mSensorToENU.column(2);
	c3[0] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
	c3[1] = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
	c3[2] = cos(pitch) * cos(roll);
}


cLidar2PointCloud::cLidar2PointCloud()
{}

cLidar2PointCloud::~cLidar2PointCloud()
{}

void cLidar2PointCloud::createXyzLookupTable(const beam_intrinsics_2_t& beam,
    const lidar_intrinsics_2_t& lidar, const lidar_data_format_2_t& format)
{
    cTransformMatrix<double> transform(lidar.lidar_to_sensor_transform, true);

    auto xyz = make_xyz_lut(format.columns_per_frame, format.pixels_per_column, range_unit_m,
        beam.lidar_to_beam_origins_mm, transform, beam.azimuth_angles_deg, beam.altitude_angles_deg);

    mUnitVectors = xyz.direction;
    mOffsets = xyz.offset;
}


void cLidar2PointCloud::computerPointCloud(const cOusterLidarData& data)
{
    auto columns_per_frame = data.columnsPerFrame();
    auto pixels_per_column = data.pixelsPerColumn();
    mCloud.resize(pixels_per_column, columns_per_frame);

    // Their example code seems to indicate that we need to destagger the image, but
    // that does not seem to be true!
    //    auto lidar_data = destagger(data, mPixelShiftByRow);
    auto lidar_data = ouster::to_matrix_row_major(data.data());


    for (int c = 0; c < columns_per_frame; ++c)
    {
        auto column = lidar_data.column(c);
        for (int p = 0; p < pixels_per_column; ++p)
        {
            std::size_t i = p * columns_per_frame + c;

            auto unit_vec = mUnitVectors[i];
            auto offset = mOffsets[i];

            sCloudPoint_t point;
            auto range_mm = column[p].range_mm;
            double x_mm = point.x = unit_vec.x * range_mm;
            double y_mm = point.y = unit_vec.y * range_mm;
            double z_mm = point.z = unit_vec.z * range_mm;

            x_mm += offset.x;
            y_mm += offset.y;
            z_mm += offset.z;

            point.x += offset.x;
            point.y += offset.y;
            point.z += offset.z;

//            point.x = x_mm * mm_to_m;
//            point.y = y_mm * mm_to_m;
//            point.z = z_mm * mm_to_m;

            point.range_mm = range_mm;
            point.signal = column[p].signal;
            point.reflectivity = column[p].reflectivity;
            point.nir = column[p].nir;

            mCloud.set(p, c, point);
        }
    }

/*
    for (int c = 0; c < cols; ++c)
    {
        auto column = cloud_data.column(c);
        for (int p = 0; p < rows; ++p)
        {
            if (column[p].range_mm == 0)
                continue;

            auto x_m = column[p].x;
            auto y_m = column[p].y;
            auto z_m = column[p].z;

            // Translate due to moving sensor...
            x_m += mX_m;
            y_m += mY_m;
            z_m += mZ_m;

            auto d_m = sqrt(x_m * x_m + y_m * y_m);

            if (d_m < mSensorMinDistance_m)
                continue;

            if (d_m > mSensorMaxDistance_m)
                continue;


            // Apply rotation matrix
            auto r1 = mRotation.row(0);
            auto r2 = mRotation.row(1);
            auto r3 = mRotation.row(2);

            float3 xyz;
            xyz.x = r1[0] * x_m + r1[1] * y_m + r1[2] * z_m;
            xyz.y = r2[0] * x_m + r2[1] * y_m + r2[2] * z_m;
            xyz.z = r3[0] * x_m + r3[1] * y_m + r3[2] * z_m;

            vertices.push_back(xyz);

            ranges.push_back(column[p].range_mm);

            uint3 data;
            data.a = column[p].nir;
            data.s = column[p].signal;
            data.r = column[p].reflectivity;
            returns.push_back(data);

            frameIDs.push_back(source.lidarFrameCount());
        }
	}
*/
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

	if (mLidarDataFormat.has_value() && mLidarIntrinsics.has_value() && mBeamIntrinsics.has_value())
	{
		createXyzLookupTable(mBeamIntrinsics.value(), mLidarIntrinsics.value(), mLidarDataFormat.value());
	}
}

void cLidar2PointCloud::onImuData(ouster::imu_data_t data)
{}

void cLidar2PointCloud::onLidarData(cOusterLidarData data)
{
	if (mUnitVectors.empty())
	{
		return;
	}

	computerPointCloud(data);
}

