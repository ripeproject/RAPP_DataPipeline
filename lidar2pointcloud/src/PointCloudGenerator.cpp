
#include "PointCloudGenerator.hpp"

#include "Constants.hpp"

#include "FieldScanDataModel.hpp"
#include "RappFieldBoundary.hpp"
#include "KinematicUtils.hpp"
#include "FieldUtils.hpp"
#include "MathUtils.hpp"
#include "PointCloudUtils.hpp"
#include "GroundModelUtils.hpp"

#include <cbdf/PointCloud.hpp>

#include <ouster/simple_blas.h>
#include <ouster/ouster_utils.h>

#include <eigen3/Eigen/Eigen>

#include <numbers>
#include <stdexcept>
#include <algorithm>
#include <valarray>
#include <cmath>


using namespace ouster;
using namespace pointcloud;

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

double	cPointCloudGenerator::getComputedStartYaw_deg() const
{
    return mComputedStartYaw_deg;
}

double	cPointCloudGenerator::getComputedEndYaw_deg() const
{
    return mComputedEndYaw_deg;
}

const std::vector<kdt::sDollyInfo_t>& cPointCloudGenerator::getComputedDollyPath() const
{
    return mComputedDollyPath;
}

bool cPointCloudGenerator::hasPointCloud() const
{
    return !mPointCloud.empty();
}

const cRappPointCloud& cPointCloudGenerator::getPointCloud() const
{
    return mPointCloud;
}

cRappPointCloud& cPointCloudGenerator::getPointCloud()
{
    return mPointCloud;
}

bool cPointCloudGenerator::recordingFittingData() const
{
    return mRecordFittingData;
}

void cPointCloudGenerator::recordFittingData(bool record)
{
    mRecordFittingData = record;
}

double cPointCloudGenerator::getAvgTranslationHeight_m() const
{
    if (mHeights.empty())
        return 0.0;
        
    double height_mm = 0;

    for (const auto& info : mHeights)
    {
        height_mm += info.height_mm;
    }

    height_mm /= mHeights.size();

    return height_mm * nConstants::MM_TO_M;
}

const std::vector<cPointCloudGenerator::sTranslationInfo_t>& cPointCloudGenerator::getHeights() const
{
    return mHeights;
}

double cPointCloudGenerator::getAvgRotationPitch_deg() const
{
    if (mAngles.empty())
        return 0.0;

    double pitch_deg = 0;

    for (const auto& info : mAngles)
    {
        pitch_deg += info.pitch_deg;
    }

    pitch_deg /= mAngles.size();

    return pitch_deg;
}

double cPointCloudGenerator::getAvgRotationRoll_deg() const
{
    if (mAngles.empty())
        return 0.0;

    double roll_deg = 0;

    for (const auto& info : mAngles)
    {
        roll_deg += info.roll_deg;
    }

    roll_deg /= mAngles.size();

    return roll_deg;
}

const std::vector<cPointCloudGenerator::sRotationInfo_t>& cPointCloudGenerator::getAngles() const
{
    return mAngles;
}

bool cPointCloudGenerator::referencePointValid() const
{
    return mPointCloud.referenceValid();
}

rfm::rappPoint_t cPointCloudGenerator::referencePoint() const
{
    return mPointCloud.referencePoint();
}

void cPointCloudGenerator::setReferencePoint(rfm::rappPoint_t point, bool valid)
{
    mPointCloud.setReferencePoint(point, valid);
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

std::pair<double, double> cPointCloudGenerator::getValidRange_m() const
{
    return std::make_pair(mMinDistance_mm * nConstants::MM_TO_M, mMaxDistance_mm * nConstants::MM_TO_M);
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

std::pair<double, double> cPointCloudGenerator::getAltitudeWindow_deg() const
{
    return std::make_pair(mMinAltitude_rad * nConstants::RAD_TO_DEG, mMaxAltitude_rad * nConstants::RAD_TO_DEG);
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

std::pair<double, double> cPointCloudGenerator::getAzimuthWindow_deg() const
{
    return std::make_pair(mMinEncoder_rad * nConstants::RAD_TO_DEG, mMaxEncoder_rad * nConstants::RAD_TO_DEG);
}

eTranslateToGroundModel cPointCloudGenerator::getTranslateToGroundModel() const
{
    return mTranslateToGroundModel;
}

void cPointCloudGenerator::setTranslateToGroundModel(eTranslateToGroundModel model)
{
    mTranslateToGroundModel = model;
}

void cPointCloudGenerator::setTranslateDistance_m(double distance_m)
{
    mTranslationDistance_m = distance_m;
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

void cPointCloudGenerator::setTranslateInterpTable(const std::vector<pointcloud::sPointCloudTranslationInterpPoint_t>& table)
{
    mTranslateInterpTable = table;
}

eRotateToGroundModel cPointCloudGenerator::getRotationToGroundModel() const
{
    return mRotateToGroundModel;
}

void cPointCloudGenerator::setRotationToGroundModel(eRotateToGroundModel model)
{
    mRotateToGroundModel = model;
}

void cPointCloudGenerator::setRotationAngles_deg(double pitch_deg, double roll_deg)
{
    mRotationPitch_deg = pitch_deg;
    mRotationRoll_deg = roll_deg;
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

void cPointCloudGenerator::setRotateInterpTable(const std::vector<pointcloud::sPointCloudRotationInterpPoint_t>& table)
{
    mRotateInterpTable = table;
}

void cPointCloudGenerator::enableAutomaticYawCorrection(bool enable)
{
    mEnableAutomaticYawCorrection = enable;
}

void cPointCloudGenerator::setPlantHeightThreshold_pct(double threshold_pct)
{
    mPlantHeightThreshold_pct = threshold_pct;
}

void cPointCloudGenerator::setNearestNeighborThreshold_mm(double threshold_mm)
{
    mMaxSeparation_mm = threshold_mm;
}

void cPointCloudGenerator::setComputedYawLimits(double lower_limit_deg, double upper_limit_deg)
{
    mLowerLimitYaw_deg = lower_limit_deg;
    mUpperLimitYaw_deg = upper_limit_deg;

    if (mUpperLimitYaw_deg < mLowerLimitYaw_deg)
        std::swap(mLowerLimitYaw_deg, mUpperLimitYaw_deg);
}

void cPointCloudGenerator::setDollyPath(const std::vector<kdt::sDollyInfo_t>& path)
{
    mDollyPath = path;
}

//void cPointCloudGenerator::abort()
//{
//    mAbort = true;
//}

void cPointCloudGenerator::clearLidarData()
{
    mLidarData.clear();
}


void cPointCloudGenerator::clear()
{
    mAbort = false;

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

    mComputedStartYaw_deg = 0.0;
    mComputedEndYaw_deg = 0.0;

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

    if (mAbort)
    {
        return false;
    }

    double displacement_mm = 0.0;
    mHeights.clear();
    mAngles.clear();

    for (auto& lidar_frame : mLidarData)
    {
        if (mAbort)
        {
            return false;
        }

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

                point.chnNum = c;
                point.pixelNum = p;

                cloud_frame.set(p, c, point);
            }
        }

        if (!transform(time_us, mDollyPath, cloud_frame, &mComputedDollyPath, &displacement_mm))
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

        double height_mm = 0.0;

        switch (mTranslateToGroundModel)
        {
        case eTranslateToGroundModel::NONE:
            break;
        case eTranslateToGroundModel::CONSTANT:
            pointCloud.translate(0, 0, static_cast<int>(mTranslationDistance_m * nConstants::M_TO_MM));
            break;
        case eTranslateToGroundModel::FIT:
        {
            auto offset = computePcToGroundMeshDistanceUsingGrid_mm(pointCloud, mTranslationThreshold_pct);
            if (offset.valid)
            {
                pointCloud.translate(0, 0, offset.offset_mm);
                height_mm = offset.offset_mm;
            }
            break;
        }
        case eTranslateToGroundModel::INTREP_CURVE:
        {
            double lowerDisplacement_m = 0.0;
            double upperDisplacemant_m = 0.0;
            double lowerHeight_m = 0.0;
            double upperHeight_m = 0.0;
            double h_mm = 0.0;

            double displacement_m = displacement_mm * nConstants::MM_TO_M;

            for (const auto& point : mTranslateInterpTable)
            {
                if (displacement_m >= point.displacement_m)
                {
                    lowerDisplacement_m = upperDisplacemant_m = point.displacement_m;
                    lowerHeight_m = upperHeight_m = point.height_m;
                }
                else
                {
                    upperDisplacemant_m = point.displacement_m;
                    upperHeight_m = point.height_m;
                    break;
                }
            }

            if (lowerDisplacement_m == upperDisplacemant_m)
            {
                h_mm = lowerHeight_m * nConstants::M_TO_MM;
            }
            else
            {
                double m = (upperHeight_m - lowerHeight_m)/(upperDisplacemant_m - lowerDisplacement_m);

                h_mm = m * (displacement_m - lowerDisplacement_m) + lowerHeight_m;
                h_mm *= nConstants::M_TO_MM;
            }

            pointCloud.translate(0, 0, h_mm);
            break;
        }
        }

        if (mAllowRotationToGroundData)
        {

            switch (mRotateToGroundModel)
            {
            case eRotateToGroundModel::NONE:
                break;
            case eRotateToGroundModel::CONSTANT:
                pointCloud.rotate(0, mRotationPitch_deg, mRotationRoll_deg);
                break;
            case eRotateToGroundModel::FIT:
            {
                auto angles = computePcToGroundMeshRotationUsingGrid_deg(pointCloud, mRotationThreshold_pct);
                if (angles.valid)
                {
                    pointCloud.rotate(0, angles.pitch_deg, angles.roll_deg);

                    if (mRecordFittingData)
                    {
                        mAngles.push_back({ displacement_mm, angles.pitch_deg, angles.roll_deg });
                    }
                }
                break;
            }
            case eRotateToGroundModel::INTREP_CURVE:
            {
                double lowerDisplacement_m = 0.0;
                double upperDisplacemant_m = 0.0;
                double lowerPitch_deg = 0.0;
                double upperPitch_deg = 0.0;
                double lowerRoll_deg = 0.0;
                double upperRoll_deg = 0.0;
                double pitch_deg = 0.0;
                double roll_deg = 0.0;

                double displacement_m = displacement_mm * nConstants::MM_TO_M;

                for (const auto& point : mRotateInterpTable)
                {
                    if (displacement_m >= point.displacement_m)
                    {
                        lowerDisplacement_m = upperDisplacemant_m = point.displacement_m;
                        lowerPitch_deg = upperPitch_deg = point.pitch_deg;
                        lowerRoll_deg  = upperRoll_deg  = point.roll_deg;
                    }
                    else
                    {
                        upperDisplacemant_m = point.displacement_m;
                        upperPitch_deg = point.pitch_deg;
                        upperRoll_deg = point.roll_deg;
                        break;
                    }
                }

                if (lowerDisplacement_m == upperDisplacemant_m)
                {
                    pitch_deg = lowerPitch_deg;
                    roll_deg  = lowerRoll_deg;
                }
                else
                {
                    double mp = (upperPitch_deg - lowerPitch_deg) / (upperDisplacemant_m - lowerDisplacement_m);
                    double mr = (upperRoll_deg  - lowerRoll_deg)  / (upperDisplacemant_m - lowerDisplacement_m);

                    pitch_deg = mp * (displacement_m - lowerDisplacement_m) + lowerPitch_deg;
                    roll_deg  = mr * (displacement_m - lowerDisplacement_m) + lowerRoll_deg;
                }

                pointCloud.rotate(0, pitch_deg, roll_deg);
                break;
            }
            }
        }

        if (mTranslateToGroundModel == eTranslateToGroundModel::FIT)
        {
            auto offset = computePcToGroundMeshDistanceUsingGrid_mm(pointCloud, mTranslationThreshold_pct);
            if (offset.valid)
            {
                pointCloud.translate(0, 0, offset.offset_mm);
                height_mm += offset.offset_mm;
            }
        }

        if (mRecordFittingData)
        {
            mHeights.push_back({ displacement_mm, height_mm });
        }
        else
        {
            mPointCloud += pointCloud;
        }
    }

    return true;
}

bool cPointCloudGenerator::computeYawCorrections()
{
    using namespace nMathUtils;

    double dx = mDollyPath.back().x_mm - mDollyPath.front().x_mm;
    double dy = mDollyPath.back().y_mm - mDollyPath.front().y_mm;

    double groundTrack_deg = wrap_0_to_360(atan2(dy, dx) * nConstants::RAD_TO_DEG);

    int32_t tolerance_mm = 10;

    auto scan_cutoff = threshold(mPointCloud.minZ_mm(), mPointCloud.maxZ_mm(), mPlantHeightThreshold_pct);

    mComputedStartYaw_deg = 0.0;
    mComputedEndYaw_deg = 0.0;

    if (((88.0 < groundTrack_deg) && (groundTrack_deg < 92.0)) ||
        ((268.0 < groundTrack_deg) && (groundTrack_deg < 272.0)))
    {
        std::vector<rfm::rappPoint_t> east;
        std::vector<rfm::rappPoint_t> west;

        // Find the east and west boundaries
        double dx = (mPointCloud.maxX_mm() - mPointCloud.minX_mm()) / 100.0;
        double x = mPointCloud.minX_mm() + dx;

        auto east_limit = threshold(mPointCloud.minY_mm(), mPointCloud.maxY_mm(), 75.0);
        auto west_limit = threshold(mPointCloud.minY_mm(), mPointCloud.maxY_mm(), 25.0);

        std::size_t i = 0;

        for (; x < mPointCloud.maxX_mm(); x += dx)
        {
//            emit progressUpdated(i++);

            auto slice = pointcloud::sliceAtGivenX(mPointCloud.data(), x, tolerance_mm);

            std::sort(slice.begin(), slice.end(), [](auto a, auto b)
                {
                    return a.y_mm < b.y_mm;
                });

            // Start by looking for the west boundary
            int32_t y_west = -1;
            for (auto point : slice)
            {
                if (point.y_mm > west_limit)
                    break;

                if (point.z_mm > scan_cutoff)
                {
                    y_west = point.y_mm;
                    break;
                }
            }

            if (y_west > 0)
            {
                west.emplace_back(static_cast<int32_t>(x), y_west, scan_cutoff);
            }

            // Looking for the east boundary
            int32_t y_east = -1;
            for (auto it = slice.rbegin(); it != slice.rend(); ++it)
            {
                auto point = *it;

                if (point.y_mm < east_limit)
                    break;

                if (point.z_mm > scan_cutoff)
                {
                    y_east = point.y_mm;
                    break;
                }
            }

            if (y_east > 0)
            {
                east.emplace_back(static_cast<int32_t>(x), y_east, scan_cutoff);
            }
        }

        if (east.size() > 5)
        {
            std::vector<rfm::rappPoint2_t> nearest;

            rfm::rappPoint2_t point;
            for (int i = 0; i < (east.size() - 1); ++i)
            {
                point.x_mm = east[i].x_mm;
                point.y_mm = east[i].y_mm;
                point.z_mm = east[i].z_mm;

                point.h_mm = std::abs(east[i + 1].y_mm - east[i].y_mm);

                nearest.push_back(point);
            }

            point.x_mm = east.back().x_mm;
            point.y_mm = east.back().y_mm;
            point.z_mm = east.back().z_mm;
            nearest.push_back(point);

            std::sort(nearest.begin(), nearest.end(), [](auto a, auto b) { return a.h_mm < b.h_mm; });

            double min_d = nearest.front().h_mm;
            double max_d = nearest.back().h_mm;

            double spread = max_d - min_d;

            if (spread > mMaxSeparation_mm)
            {
                east.clear();
                for (auto point : nearest)
                {
                    if (point.h_mm > mMaxSeparation_mm)
                        break;

                    east.emplace_back(point.x_mm, point.y_mm, point.z_mm);
                }
            }

            auto east_side = fitHorizontalLine(east);
            mComputedEndYaw_deg = -1 * atan(east_side.m) * nConstants::RAD_TO_DEG;

            mComputedEndYaw_deg = bound(mComputedEndYaw_deg, mLowerLimitYaw_deg, mUpperLimitYaw_deg);
        }

        if (west.size() > 5)
        {
            std::vector<rfm::rappPoint2_t> nearest;

            rfm::rappPoint2_t point;
            for (int i = 0; i < (west.size() - 1); ++i)
            {
                point.x_mm = west[i].x_mm;
                point.y_mm = west[i].y_mm;
                point.z_mm = west[i].z_mm;

                point.h_mm = std::abs(west[i + 1].y_mm - west[i].y_mm);

                nearest.push_back(point);
            }

            point.x_mm = west.back().x_mm;
            point.y_mm = west.back().y_mm;
            point.z_mm = west.back().z_mm;
            nearest.push_back(point);

            std::sort(nearest.begin(), nearest.end(), [](auto a, auto b) { return a.h_mm < b.h_mm; });

            double min_d = nearest.front().h_mm;
            double max_d = nearest.back().h_mm;

            double spread = max_d - min_d;

            if (spread > mMaxSeparation_mm)
            {
                west.clear();
                for (auto point : nearest)
                {
                    if (point.h_mm > mMaxSeparation_mm)
                        break;

                    west.emplace_back(point.x_mm, point.y_mm, point.z_mm);
                }

            }

            auto west_side = fitHorizontalLine(west);
            mComputedStartYaw_deg = -1 * atan(west_side.m) * nConstants::RAD_TO_DEG;

            mComputedStartYaw_deg = bound(mComputedStartYaw_deg, mLowerLimitYaw_deg, mUpperLimitYaw_deg);
        }

        if (groundTrack_deg > 180)
            std::swap(mComputedStartYaw_deg, mComputedEndYaw_deg);

/*
        QString text = "Starting yaw = ";
        text += QString::number(mComputedStartYaw_deg, 'f', 3);
        text += " degrees, ending yaw = ";
        text += QString::number(mComputedEndYaw_deg, 'f', 3);
        text += " degrees.";

        emit statusUpdate(text);
*/
    }
    else
    {
        std::vector<rfm::rappPoint_t> north;
        std::vector<rfm::rappPoint_t> south;

        // Find the north and south boundaries
        double dy = (mPointCloud.maxY_mm() - mPointCloud.minY_mm()) / 100.0;
        double y = mPointCloud.minY_mm() + dy;

        auto north_limit = threshold(mPointCloud.minX_mm(), mPointCloud.maxX_mm(), 25.0);
        auto south_limit = threshold(mPointCloud.minX_mm(), mPointCloud.maxX_mm(), 75.0);

        for (; y < mPointCloud.maxY_mm(); y += dy)
        {
            auto slice = pointcloud::sliceAtGivenY(mPointCloud.data(), y, tolerance_mm);

            std::sort(slice.begin(), slice.end(), [](auto a, auto b)
                {
                    return a.x_mm < b.x_mm;
                });

            // Start by looking for the north boundary
            int32_t x_north = -1;
            for (auto point : slice)
            {
                if (point.x_mm > north_limit)
                    break;

                if (point.z_mm > scan_cutoff)
                {
                    x_north = point.x_mm;
                    break;
                }
            }

            if (x_north < 0)
                continue;

            // Looking for the south boundary
            int32_t x_south = -1;
            for (auto it = slice.rbegin(); it != slice.rend(); ++it)
            {
                auto point = *it;

                if (point.x_mm < south_limit)
                    break;

                if (point.z_mm > scan_cutoff)
                {
                    x_south = point.x_mm;
                    break;
                }
            }

            if (x_south > 0)
            {
                north.emplace_back(x_north, static_cast<int32_t>(y), scan_cutoff);
                south.emplace_back(x_south, static_cast<int32_t>(y), scan_cutoff);
            }
        }
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
        if (mAbort)
        {
            return false;
        }
        
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

bool cPointCloudGenerator::updateDollyPath()
{
    auto start = mDollyPath.front();
    auto end = mDollyPath.back();

    double scan_time_sec = (end.timestamp_us - start.timestamp_us) * nConstants::US_TO_SEC;

    double start_yaw = nMathUtils::wrap_0_to_360(start.yaw_deg + mComputedStartYaw_deg);
    double end_yaw = nMathUtils::wrap_0_to_360(end.yaw_deg + mComputedEndYaw_deg);

    auto yaw_rate = (end_yaw - start_yaw) / scan_time_sec;

    // Update the yaw information
    for (auto& pos : mDollyPath)
    {
        double time_sec = pos.timestamp_us * nConstants::US_TO_SEC;
        pos.yaw_deg = start_yaw + yaw_rate * time_sec;
        pos.yawRate_dps = yaw_rate;
    }

    return true;
}
