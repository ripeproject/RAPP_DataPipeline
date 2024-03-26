
#include "PointCloudGenerator.hpp"

#include "Constants.hpp"

#include "FieldScanDataModel.hpp"
#include "RappFieldBoundary.hpp"
#include "KinematicUtils.hpp"
#include "FieldUtils.hpp"
#include "MathUtils.hpp"
#include "PointCloudUtils.hpp"
#include "GroundModelUtils.hpp"

#include "PointCloud.hpp"

#include <ouster/simple_blas.h>
#include <ouster/ouster_utils.h>

#include <eigen3/Eigen/Eigen>

#include <numbers>
#include <stdexcept>
#include <algorithm>
#include <valarray>
#include <cmath>


using namespace ouster;

extern void update_progress(const int id, const int progress_pct);


namespace
{
    /** Unit of range from sensor packet, in meters. */
    constexpr double range_unit_m = 0.001;
    constexpr double range_unit_mm = 1.0;

    /** Lookup table of beam directions and offsets. */
    struct sXYZ_Lut_t
    {
        std::vector<cPointCloudGenerator::sPoint_t> direction;
        std::vector<cPointCloudGenerator::sPoint_t> offset;
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
    inline void rotate(std::vector<cPointCloudGenerator::sPoint_t>& lhs, const ouster::cRotationMatrix<T>& r)
    {
        const auto& rX = r.column(0);
        const auto& rY = r.column(1);
        const auto& rZ = r.column(2);

        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            cPointCloudGenerator::sPoint_t p = lhs[i];

            double x = p.x * rX[0] + p.y * rX[1] + p.z * rX[2];
            double y = p.x * rY[0] + p.y * rY[1] + p.z * rY[2];
            double z = p.x * rZ[0] + p.y * rZ[1] + p.z * rZ[2];

            lhs[i] = cPointCloudGenerator::sPoint_t(x, y, z);
        }
    }

    template<typename T>
    inline void translate(std::vector<cPointCloudGenerator::sPoint_t>& lhs, const ouster::cTranslation<T>& t)
    {
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            cPointCloudGenerator::sPoint_t& p = lhs[i];

            if ((p.x == 0.0) && (p.y == 0.0) && (p.z == 0.0))
            {
                continue;
            }
            p.x += t.x();
            p.y += t.y();
            p.z += t.z();
        }
    }

    void operator*=(std::vector<cPointCloudGenerator::sPoint_t>& lhs, const double rhs)
    {
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            lhs[i].x *= rhs;
            lhs[i].y *= rhs;
            lhs[i].z *= rhs;
        }
    }

    ouster::cRotationMatrix<double> computeSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
    {
        double pitch_rad = -pitch_deg * nConstants::DEG_TO_RAD;
        double roll_rad  = -roll_deg * nConstants::DEG_TO_RAD;
        double yaw_rad   = -yaw_deg * nConstants::DEG_TO_RAD;

        Eigen::AngleAxisd rollAngle(roll_rad, Eigen::Vector3d::UnitX());
        Eigen::AngleAxisd yawAngle(yaw_rad, Eigen::Vector3d::UnitZ());
        Eigen::AngleAxisd pitchAngle(pitch_rad, Eigen::Vector3d::UnitY());

        //	Eigen::Quaternion<double> q = rollAngle * pitchAngle * yawAngle;
        Eigen::Quaternion<double> q = pitchAngle * rollAngle * yawAngle;
        Eigen::Matrix3d rotationMatrix = q.matrix();

        ouster::cRotationMatrix<double> result;
        result.identity();

        double e; // Used for debugging;

        auto c1 = result.column(0);
        c1[0] = e = rotationMatrix.col(0)[0];
        c1[1] = e = rotationMatrix.col(0)[1];
        c1[2] = e = rotationMatrix.col(0)[2];

        auto c2 = result.column(1);
        c2[0] = e = rotationMatrix.col(1)[0];
        c2[1] = e = rotationMatrix.col(1)[1];
        c2[2] = e = rotationMatrix.col(1)[2];

        auto c3 = result.column(2);
        c3[0] = e = rotationMatrix.col(2)[0];
        c3[1] = e = rotationMatrix.col(2)[1];
        c3[2] = e = rotationMatrix.col(2)[2];

        return result;
    }

    void fillGroundData(ouster::matrix_col_major<rfm::sPoint3D_t>& cloud_frame)
    {
        for (int c = 0; c < cloud_frame.num_columns(); ++c)
        {
            auto column = cloud_frame.column(c);
            for (int p = 0; p < cloud_frame.num_rows(); ++p)
            {
                auto& point = column[p];

                if ((point.x_mm == 0) && (point.y_mm == 0) && (point.z_mm == 0))
                    continue;

                double h = getMeshHeight_mm(point.x_mm, point.y_mm);
                point.h_mm = h;
            }
        }
    }
}

bool cPointCloudGenerator::hasData() const
{
    return !mLidarData.empty();
}

double cPointCloudGenerator::getScanTime_sec() const
{
    if (mLidarData.empty())
        return 0.0;

    return (mLidarData.back().timestamp_ns() - mLidarData.front().timestamp_ns()) * nConstants::NS_TO_SEC;
}

const std::vector<rfm::sDollyInfo_t>& cPointCloudGenerator::getComputedDollyPath() const
{
    return mComputedDollyPath;
}

const cRappPointCloud& cPointCloudGenerator::getPointCloud() const
{
    return mPointCloud;
}

void cPointCloudGenerator::fixTimestampInfo(double updateRate_Hz, uint64_t start_timestamp_ns)
{
    if (updateRate_Hz <= 0.0)
        return;

    double updateTime_sec = 1.0 / updateRate_Hz;
    uint64_t delta_time_ns = static_cast<uint64_t>(updateTime_sec * nConstants::SEC_TO_NS);

    uint16_t frameId = 1;
    uint64_t timestamp_ns = start_timestamp_ns;

    for (auto& data : mLidarData)
    {
        data.frame_id(frameId++);
        data.timestamp_ns(timestamp_ns);
        timestamp_ns += delta_time_ns;
    }
}


void cPointCloudGenerator::resetValidRange()
{
    mMinDistance_mm = 1;
    mMaxDistance_mm = 1'000'000.0;
}

void cPointCloudGenerator::setValidRange_m(double min_dist_m, double max_dist_m)
{
    mMinDistance_mm = std::max(min_dist_m, 0.001) * nConstants::M_TO_MM;
    mMaxDistance_mm = std::min(max_dist_m, 1000.0) * nConstants::M_TO_MM;
}

void cPointCloudGenerator::resetAltitudeWindow()
{
    mMinAltitude_rad = -std::numbers::pi;
    mMaxAltitude_rad = +std::numbers::pi;
    mAltitudeRangeValid = true;
    mAllowRotationToGroundData = mAltitudeRangeValid && mAzimuthRangeValid;
}

void cPointCloudGenerator::setAltitudeWindow_deg(double min_altitude_deg, double max_altitude_deg)
{
    mMinAltitude_rad = min_altitude_deg * nConstants::DEG_TO_RAD;
    mMaxAltitude_rad = max_altitude_deg * nConstants::DEG_TO_RAD;
    mAltitudeRangeValid = (max_altitude_deg - min_altitude_deg) > 3.0;
    mAllowRotationToGroundData = mAltitudeRangeValid && mAzimuthRangeValid;
}

void cPointCloudGenerator::resetAzimuthWindow()
{
    mMinEncoder_rad = 0.0;
    mMaxEncoder_rad = 2.0 * std::numbers::pi;
    mAzimuthRangeValid = true;
    mAllowRotationToGroundData = mAltitudeRangeValid && mAzimuthRangeValid;
}

void cPointCloudGenerator::setAzimuthWindow_deg(double min_azimuth_deg, double max_azimuth_deg)
{
    mMinEncoder_rad = min_azimuth_deg * nConstants::DEG_TO_RAD;
    mMaxEncoder_rad = max_azimuth_deg * nConstants::DEG_TO_RAD;
    mAzimuthRangeValid = (max_azimuth_deg - min_azimuth_deg) > 3.0;
    mAllowRotationToGroundData = mAltitudeRangeValid && mAzimuthRangeValid;
}

void cPointCloudGenerator::enableTranslateToGroundData(bool enable)
{
    mEnableTranslateToGroundData = enable;
}

void cPointCloudGenerator::setTranslateThreshold_pct(double threshold_pct)
{
    if (threshold_pct < 1.0)
        mTranslationThreshold_pct = 1.0;
    else if (threshold_pct > 100.0)
        mTranslationThreshold_pct = 100.0;
    else
        mTranslationThreshold_pct = threshold_pct;
}

void cPointCloudGenerator::enableRotationToGroundData(bool enable)
{
    mEnableRotationToGroundData = enable;
}

void cPointCloudGenerator::setRotationThreshold_pct(double threshold_pct)
{
    if (threshold_pct < 1.0)
        mRotationThreshold_pct = 1.0;
    else if (threshold_pct > 100.0)
        mRotationThreshold_pct = 100.0;
    else
        mRotationThreshold_pct = threshold_pct;
}

void cPointCloudGenerator::setDollyPath(const std::vector<rfm::sDollyInfo_t>& path)
{
    mDollyPath = path;
}

void cPointCloudGenerator::clear()
{
    mPointCloud.clear();
    mComputedDollyPath.clear();

    resetValidRange();
    resetAzimuthWindow();
    resetAltitudeWindow();

    mDollyPath.clear();

    mColumnsPerFrame = 0;
    mPixelsPerColumn = 0;

    mLidarToBeamOrigins_mm = 0.0;

    mAzimuthAngles_rad.clear();
    mAltitudeAngles_rad.clear();

    mLidarToSensorTransform.clear();

    mLidarData.clear();
}

void cPointCloudGenerator::setDimensions(uint16_t columns_per_frame, uint16_t pixels_per_column)
{
    mColumnsPerFrame = columns_per_frame;
    mPixelsPerColumn = pixels_per_column;
}

void cPointCloudGenerator::setBeamOffset(double lidar_to_beam_origins_mm)
{
    mLidarToBeamOrigins_mm = lidar_to_beam_origins_mm;
}

void cPointCloudGenerator::setBeamAzimuthAngles_deg(const std::vector<double>& azimuth_angles_deg)
{
    mAzimuthAngles_rad.resize(azimuth_angles_deg.size());

    for (std::size_t i = 0; i < azimuth_angles_deg.size(); ++i)
    {
        mAzimuthAngles_rad[i] = azimuth_angles_deg[i] * nConstants::DEG_TO_RAD;
    }
}

void cPointCloudGenerator::setBeamAltitudeAngles_deg(const std::vector<double>& altitude_angles_deg)
{
    mAltitudeAngles_rad.resize(altitude_angles_deg.size());

    for (std::size_t i = 0; i < altitude_angles_deg.size(); ++i)
    {
        mAltitudeAngles_rad[i] = altitude_angles_deg[i] * nConstants::DEG_TO_RAD;
    }

    double minAlt_deg = altitude_angles_deg.front();
    double maxAlt_deg = altitude_angles_deg.back();

//    emit onAltitudeWindowChange(minAlt_deg, maxAlt_deg);
}

void cPointCloudGenerator::setLidarToSensorTransform(const std::vector<double>& lidar_to_sensor_transform)
{
    mLidarToSensorTransform = lidar_to_sensor_transform;
}

void cPointCloudGenerator::addLidarData(const cOusterLidarData& data)
{
    mLidarData.push_back(data);
}


/**********************************************************
* Utility Methods
***********************************************************/

cPointCloudGenerator::sLUT_t cPointCloudGenerator::generateLookupTable()
{
    if (mColumnsPerFrame <= 0 || mPixelsPerColumn <= 0)
        throw std::invalid_argument("lut dimensions must be greater than zero");

    if (mAzimuthAngles_rad.size() != mPixelsPerColumn || mAltitudeAngles_rad.size() != mPixelsPerColumn)
        throw std::invalid_argument("unexpected scan dimensions");

    cTransformMatrix<double> transform(mLidarToSensorTransform, true);

    std::valarray<double> encoder_rad(mColumnsPerFrame * mPixelsPerColumn);   // theta_e
    std::valarray<double> azimuth_rad(mColumnsPerFrame * mPixelsPerColumn);   // theta_a
    std::valarray<double> altitude_rad(mColumnsPerFrame * mPixelsPerColumn);  // phi

    const double azimuth_radians = nConstants::TWO_PI / mColumnsPerFrame;

    // populate angles for each pixel
    for (size_t v = 0; v < mColumnsPerFrame; ++v)
    {
        for (size_t u = 0; u < mPixelsPerColumn; ++u)
        {
            size_t i = u * mColumnsPerFrame + v;
            encoder_rad[i] = nConstants::TWO_PI - (v * azimuth_radians);
            azimuth_rad[i] = -mAzimuthAngles_rad[u];
            altitude_rad[i] = mAltitudeAngles_rad[u];
        }
    }

    sXYZ_Lut_t lut;
    lut.direction.resize(mColumnsPerFrame * mPixelsPerColumn);
    lut.offset.resize(mColumnsPerFrame * mPixelsPerColumn);

    auto n = encoder_rad.size();
    for (size_t i = 0; i < n; ++i)
    {
        double encoder = encoder_rad[i];
        double azimuth = azimuth_rad[i];
        double altitude = altitude_rad[i];

        if ((mMinEncoder_rad <= encoder) && (encoder <= mMaxEncoder_rad)
            &&
            (mMinAltitude_rad <= altitude) && (altitude <= mMaxAltitude_rad))
        {
            auto altitude_cos = cos(altitude);
            auto x = cos(encoder + azimuth) * altitude_cos;
            auto y = sin(encoder + azimuth) * altitude_cos;
            auto z = sin(altitude);

            lut.direction[i].x = x;
            lut.direction[i].y = y;
            lut.direction[i].z = z;

            lut.offset[i].x = cos(encoder) - x;
            lut.offset[i].y = sin(encoder) - y;
            lut.offset[i].z = -z;
        }
        else
        {
            lut.direction[i].x = 0;
            lut.direction[i].y = 0;
            lut.direction[i].z = 0;

            lut.offset[i].x = 0;
            lut.offset[i].y = 0;
            lut.offset[i].z = 0;
        }
    }

    // offsets due to beam origin
    lut.offset *= mLidarToBeamOrigins_mm;

    // apply the supplied transform
    auto rot = transform.rotation();
    auto trans = transform.translation();

    rotate(lut.direction, rot);
    rotate(lut.offset, rot);

    translate(lut.offset, trans);

    // apply scaling factor
    lut.direction *= range_unit_mm;
    lut.offset *= range_unit_mm;

    return { lut.direction, lut.offset };
}

bool cPointCloudGenerator::computePointCloud(int id)
{
    mPointCloud.clear();
    mComputedDollyPath.clear();

    std::size_t i = 0;
    std::size_t n = mLidarData.size() + 1;    // Add one to account for generation of LUT

    update_progress(id, (100.0 * i++) / n);

    auto lut = generateLookupTable();

    auto frameID = mLidarData.front().frame_id();
    auto startTimestamp_ns = mLidarData.front().timestamp_ns();

    ouster::matrix_col_major<rfm::sPoint3D_t> cloud_frame;
    cloud_frame.resize(mPixelsPerColumn, mColumnsPerFrame);

    for (auto& lidar_frame : mLidarData)
    {
        update_progress(id, (100.0 * i++) / n);

        frameID = lidar_frame.frame_id();
        double timestamp_ns = lidar_frame.timestamp_ns();
        auto time_us = static_cast<double>(timestamp_ns - startTimestamp_ns) * nConstants::NS_TO_US;

        // Their example code seems to indicate that we need to destagger the image, but
        // that does not seem to be true!
        //auto lidar_data = destagger(data, mPixelShiftByRow);
        auto lidar_data = ouster::to_matrix_row_major(lidar_frame.data());

        rfm::sPoint3D_t point;

        point.frameID = frameID;

        for (int c = 0; c < mColumnsPerFrame; ++c)
        {
            point.chnNum = c;

            auto column = lidar_data.column(c);
            for (int p = 0; p < mPixelsPerColumn; ++p)
            {
                point.pixelNum = p;

                std::size_t i = p * mColumnsPerFrame + c;

                auto range_mm = column[p].range_mm;

                if ((range_mm < mMinDistance_mm) || (range_mm > mMaxDistance_mm))
                {
                    point.x_mm = 0;
                    point.y_mm = 0;
                    point.z_mm = 0;
                }
                else
                {
                    auto unit_vec = lut.unitVectors[i];
                    auto offset = lut.offsets[i];

                    double x_mm = unit_vec.x * range_mm;
                    double y_mm = unit_vec.y * range_mm;
                    double z_mm = unit_vec.z * range_mm;

                    x_mm += offset.x;
                    y_mm += offset.y;
                    z_mm += offset.z;

                    point.x_mm = static_cast<int32_t>(x_mm);
                    point.y_mm = -1 * static_cast<int32_t>(y_mm);
                    point.z_mm = static_cast<int32_t>(z_mm);
                }

                point.range_mm = range_mm;
                point.signal = column[p].signal;
                point.reflectivity = column[p].reflectivity;
                point.nir = column[p].nir;

                cloud_frame.set(p, c, point);
            }
        }

        if (!transform(time_us, mDollyPath, cloud_frame,  &mComputedDollyPath))
        {
            return true;
        }

        // for each point in the point cloud, find the corresponding ground point
        fillGroundData(cloud_frame);

        cRappPointCloud pointCloud;
        pointCloud.clear();

        for (int c = 0; c < mColumnsPerFrame; ++c)
        {
            auto column = cloud_frame.column(c);
            for (int p = 0; p < mPixelsPerColumn; ++p)
            {
                auto point = column[p];

                if ((point.x_mm == 0) && (point.y_mm == 0) && (point.z_mm == 0))
                    continue;

                pointCloud.addPoint(point);
            }
        }
        pointCloud.recomputeBounds();

        if (mEnableTranslateToGroundData)
        {
            auto offset = computePcToGroundMeshDistanceUsingGrid_mm(pointCloud, mTranslationThreshold_pct);
            if (offset.valid)
            {
                pointCloud.translate(0, 0, offset.offset_mm);
            }
        }

        if (mEnableRotationToGroundData && mAllowRotationToGroundData)
        {
            auto angles = computePcToGroundMeshRotationUsingGrid_deg(pointCloud, mRotationThreshold_pct);
            if (angles.valid)
            {
                pointCloud.rotate(0, angles.pitch_deg, angles.roll_deg);
            }
        }

        if (mEnableTranslateToGroundData)
        {
            auto offset = computePcToGroundMeshDistanceUsingGrid_mm(pointCloud, mTranslationThreshold_pct);
            if (offset.valid)
            {
                pointCloud.translate(0, 0, offset.offset_mm);
            }
        }

        mPointCloud += pointCloud;
    }

    return true;
}

bool cPointCloudGenerator::shiftPointCloudToAGL()
{
    std::size_t i = 0;
    std::size_t n = mPointCloud.size();

    double previousProgress_pct = 0.0;

    for (auto& point : mPointCloud)
    {
        double progress_pct = (100.0 * i++) / n;

        if (progress_pct > previousProgress_pct)
        {
            previousProgress_pct += 1.0;
        }

        if (point.h_mm != rfm::INVALID_HEIGHT)
        {
            point.z_mm -= point.h_mm;
            point.h_mm = 0;
        }
    }

    mPointCloud.recomputeBounds();

    return true;
}

