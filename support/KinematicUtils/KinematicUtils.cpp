
#include "KinematicUtils.hpp"

#include "Constants.hpp"
#include "MathUtils.hpp"

#include "RappFieldBoundary.hpp"

#include <cbdf/PointCloud.hpp>

#include <ouster/simple_blas.h>
#include <ouster/ouster_utils.h>

#include <eigen3/Eigen/Eigen>

#ifdef USE_UPDATE_PROGRESS
    extern void update_progress(const int id, const int progress_pct);
#endif


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


    ouster::cRotationMatrix<double> computeSensorOrientation(double yaw_deg, double pitch_deg, double roll_deg)
    {
        double pitch_rad = nMathUtils::wrap_neg_pi_to_pi(- pitch_deg * nConstants::DEG_TO_RAD);
        double roll_rad  = nMathUtils::wrap_neg_pi_to_pi(- roll_deg * nConstants::DEG_TO_RAD);
        double yaw_rad   = nMathUtils::wrap_0_to_two_pi(- yaw_deg * nConstants::DEG_TO_RAD);

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

}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
    const rfm::rappSpeeds_t& speed, double* scan_time_sec)
{
    return computeDollyKinematics(-1, start, end, speed, scan_time_sec);
}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const std::deque<nSpiderCamTypes::sPosition_t>& data, uint32_t startIndex, uint32_t endIndex)
{
    return computeDollyKinematics(-1, data, startIndex, endIndex);
}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const std::deque<nSpiderCamTypes::sPosition_t>& data, rfm::rappPoint_t startPos, rfm::rappPoint_t endPos)
{
    uint32_t start_index = 0;
    uint32_t end_index = 0;

    for (start_index = 0; start_index < data.size(); ++start_index)
    {
        const auto& p = data[start_index];
        if ((std::abs(static_cast<int32_t>(p.X_mm) - startPos.x_mm) < 10)
            && (std::abs(static_cast<int32_t>(p.Y_mm) - startPos.y_mm) < 10)
            && (std::abs(static_cast<int32_t>(p.Z_mm) - startPos.z_mm) < 10))
        {
            break;
        }
    }

    for (end_index = start_index; end_index < data.size(); ++end_index)
    {
        const auto& p = data[end_index];
        if ((std::abs(static_cast<int32_t>(p.X_mm) - endPos.x_mm) < 10)
            && (std::abs(static_cast<int32_t>(p.Y_mm) - endPos.y_mm) < 10)
            && (std::abs(static_cast<int32_t>(p.Z_mm) - endPos.z_mm) < 10))
        {
            break;
        }
    }

    return computeDollyKinematics(-1, data, start_index, end_index);
}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const std::deque<nSsnxTypes::sPvtGeodetic_t>& data)
{
    return computeDollyKinematics(-1, data);
}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
    const std::deque<nSsnxTypes::sPvtGeodetic_t>& vel, bool ignoreInvalidGroundTrackData, double* scan_time_sec)
{
    return computeDollyKinematics(-1, start, end, vel, ignoreInvalidGroundTrackData, scan_time_sec);
}

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    double pitch_offset_deg, double roll_offset_deg, double yaw_offset_deg, double scan_time_sec)
{
    return computeDollyOrientationKinematics(-1, mount_pitch_deg, mount_roll_deg, mount_yaw_deg, 
        pitch_offset_deg, roll_offset_deg, yaw_offset_deg, scan_time_sec);
}

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    kdt::sDollyAtitude_t start, kdt::sDollyAtitude_t end, double scan_time_sec)
{
    return computeDollyOrientationKinematics(-1, mount_pitch_deg, mount_roll_deg, mount_yaw_deg, start, end, scan_time_sec);
}

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    std::vector<kdt::sDollyOrientationInterpPoint_t> table, double scan_time_sec)
{
    return computeDollyOrientationKinematics(-1, mount_pitch_deg, mount_roll_deg, mount_yaw_deg, table, scan_time_sec);
}

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    const std::deque<nOusterTypes::imu_data_t>& imu, nOusterTypes::imu_intrinsics_2_t transform)
{
    return computeDollyOrientationKinematics(-1, mount_pitch_deg, mount_roll_deg, mount_yaw_deg, imu, transform);
}

void mergeDollyOrientation(std::vector<kdt::sDollyInfo_t>& dolly, const std::vector<kdt::sDollyOrientation_t>& orientation)
{
    mergeDollyOrientation(-1, dolly, orientation);
}





std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
    const rfm::rappSpeeds_t& speeds, double* scan_time_sec)
{
    std::vector<kdt::sDollyInfo_t> result;

    double speed = sqrt(speeds.vx_mmps * speeds.vx_mmps + speeds.vy_mmps * speeds.vy_mmps + speeds.vz_mmps * speeds.vz_mmps);

    if (speed == 0)
    {
        return result;
    }

    kdt::sDollyInfo_t entry;
    entry.timestamp_us = 0;
    entry.x_mm = start.x_mm;
    entry.y_mm = start.y_mm;
    entry.z_mm = start.z_mm;
    entry.vx_mmps = speeds.vx_mmps;
    entry.vy_mmps = speeds.vy_mmps;
    entry.vz_mmps = speeds.vz_mmps;

    result.push_back(entry);

#ifdef USE_UPDATE_PROGRESS
    if (id >= 0)
        update_progress(id, 50);
#endif

    double dx = end.x_mm - start.x_mm;
    double dy = end.y_mm - start.y_mm;
    double dz = end.z_mm - start.z_mm;

    double max_distance = sqrt(dx * dx + dy * dy + dz * dz);
    double time_sec = max_distance / speed;

    if (scan_time_sec)
        *scan_time_sec = time_sec;

    entry.timestamp_us = time_sec * nConstants::SEC_TO_US;
    entry.x_mm = end.x_mm;
    entry.y_mm = end.y_mm;
    entry.z_mm = end.z_mm;
    entry.vx_mmps = 0.0;
    entry.vy_mmps = 0.0;
    entry.vz_mmps = 0.0;

    result.push_back(entry);

#ifdef USE_UPDATE_PROGRESS
    if (id >= 0)
        update_progress(id, 100);
#endif

    return result;
}


std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const std::deque<nSpiderCamTypes::sPosition_t>& data,
    uint32_t startIndex, uint32_t endIndex)
{
    std::vector<kdt::sDollyInfo_t> result;

    if (data.empty())
        return result;

    auto it = data.begin();

    std::advance(it, startIndex);

    auto startTimestamp = it->timestamp;

    kdt::sDollyInfo_t entry;
    entry.timestamp_us = 0;
    entry.x_mm = it->X_mm;
    entry.y_mm = it->Y_mm;
    entry.z_mm = it->Z_mm;
    entry.vx_mmps = 0.0;
    entry.vy_mmps = 0.0;
    entry.vz_mmps = 0.0;

    result.push_back(entry);

    auto last = data.begin();
    if ((endIndex == 0) || (endIndex >= (data.size() - 1)))
    {
        last = data.end();
    }
    else
    {
        std::advance(last, endIndex + 1);
    }

    auto n = std::abs(static_cast<int>(endIndex) - static_cast<int>(startIndex));
    if (n == 0)
        n = static_cast<int>(data.size());

    int i = 0;

    for (++it; it != last; ++it)
    {
#ifdef USE_UPDATE_PROGRESS
        if (id >= 0)
        {
            int progress_pct = static_cast<int>(100.0 * i++ / n);
            update_progress(id, progress_pct);
        }
#endif
        entry.x_mm = it->X_mm;
        entry.y_mm = it->Y_mm;
        entry.z_mm = it->Z_mm;
        entry.vx_mmps = 0.0;
        entry.vy_mmps = 0.0;
        entry.vz_mmps = 0.0;

        auto& previous = result.back();

        double dx = it->X_mm - previous.x_mm;
        double dy = it->Y_mm - previous.y_mm;
        double dz = it->Z_mm - previous.z_mm;

        double dt_us = (it->timestamp - startTimestamp) * 0.1;
        if (dt_us < 0.0)
            continue;

        entry.timestamp_us = dt_us;

        double dt = (dt_us - previous.timestamp_us) * nConstants::US_TO_SEC;

        previous.vx_mmps = dx / dt;
        previous.vy_mmps = dy / dt;
        previous.vz_mmps = dz / dt;

        result.push_back(entry);
    }

    return result;
}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const std::deque<nSsnxTypes::sPvtGeodetic_t>& data)
{
    std::vector<kdt::sDollyInfo_t> result;

    if (data.empty())
        return result;

    auto it = data.begin();

    auto startTimestamp_s = it->timestamp_s;

    double height_m = it->Height_m - it->Undulation_m;

    auto point = rfb::fromGPS(it->Lat_rad, it->Lon_rad, height_m);

    kdt::sDollyInfo_t entry;
    entry.timestamp_us = 0;
    entry.x_mm = point.x_mm;
    entry.y_mm = point.y_mm;
    entry.z_mm = point.z_mm;
    entry.vx_mmps = -(it->Vn_mps * nConstants::M_TO_MM);
    entry.vy_mmps = it->Ve_mps * nConstants::M_TO_MM;
    entry.vz_mmps = it->Vu_mps * nConstants::M_TO_MM;

    result.push_back(entry);

    auto n = data.size();
    int i = 0;

    for (++it; it != data.end(); ++it)
    {
#ifdef USE_UPDATE_PROGRESS
        if (id >= 0)
        {
            int progress_pct = static_cast<int>(100.0 * i++ / n);
            update_progress(id, progress_pct);
        }
#endif

        double dt_s = (it->timestamp_s - startTimestamp_s);

        if (dt_s < 0)
            continue;

        entry.timestamp_us = dt_s * nConstants::SEC_TO_US;

        height_m = it->Height_m - it->Undulation_m;

        point = rfb::fromGPS(it->Lat_rad, it->Lon_rad, height_m);

        entry.x_mm = point.x_mm;
        entry.y_mm = point.y_mm;
        entry.z_mm = point.z_mm;
        entry.vx_mmps = -(it->Vn_mps * nConstants::M_TO_MM);
        entry.vy_mmps = it->Ve_mps * nConstants::M_TO_MM;
        entry.vz_mmps = it->Vu_mps * nConstants::M_TO_MM;

        result.push_back(entry);
    }

    return result;
}

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
    const std::deque<nSsnxTypes::sPvtGeodetic_t>& vel, bool ignoreInvalidGroundTrackData, double* scan_time_sec)
{
    std::vector<kdt::sDollyInfo_t> result;

    if (vel.empty())
        return result;

    double dx = end.x_mm - start.x_mm;
    double dy = end.y_mm - start.y_mm;
    double dz = end.z_mm - start.z_mm;

    double max_distance = sqrt(dx * dx + dy * dy + dz * dz);

    kdt::sDollyInfo_t entry;
    entry.timestamp_us = 0;
    entry.x_mm = start.x_mm;
    entry.y_mm = start.y_mm;
    entry.z_mm = start.z_mm;

    auto it = vel.begin();
    auto startTimestamp_s = it->timestamp_s;
    auto prevTimestamp_s = startTimestamp_s;
    entry.vx_mmps = -(it->Vn_mps * nConstants::M_TO_MM);
    entry.vy_mmps = it->Ve_mps * nConstants::M_TO_MM;
    entry.vz_mmps = it->Vu_mps * nConstants::M_TO_MM;

    result.push_back(entry);

    auto n = vel.size();
    int i = 0;

    for (++it; it != vel.end(); ++it)
    {
#ifdef USE_UPDATE_PROGRESS
        if (id >= 0)
        {
            int progress_pct = static_cast<int>(100.0 * i++ / n);
            update_progress(id, progress_pct);
        }
#endif

        entry.vx_mmps = -(it->Vn_mps * nConstants::M_TO_MM);
        entry.vy_mmps = it->Ve_mps * nConstants::M_TO_MM;
        entry.vz_mmps = it->Vu_mps * nConstants::M_TO_MM;

        auto& previous = result.back();

        double dt_s = (it->timestamp_s - startTimestamp_s);
        if (dt_s < 0.0)
            continue;

        entry.timestamp_us = dt_s * nConstants::SEC_TO_US;

        dt_s = (it->timestamp_s - prevTimestamp_s);

        entry.x_mm = previous.x_mm + entry.vx_mmps * dt_s;
        entry.y_mm = previous.y_mm + entry.vy_mmps * dt_s;
        entry.z_mm = previous.z_mm + entry.vz_mmps * dt_s;

        prevTimestamp_s = it->timestamp_s;
        result.push_back(entry);

        double dx = entry.x_mm - start.x_mm;
        double dy = entry.y_mm - start.y_mm;
        double dz = entry.z_mm - start.z_mm;

        double distance = sqrt(dx * dx + dy * dy + dz * dz);

        if (distance > max_distance)
            break;
    }

    if (scan_time_sec)
    {
        double time_sec = prevTimestamp_s - startTimestamp_s;

        if (time_sec > 0)
            *scan_time_sec = time_sec;
    }

    return result;
}

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    double pitch_offset_deg, double roll_offset_deg, double yaw_offset_deg, double scan_time_sec)
{
    std::vector<kdt::sDollyOrientation_t> result;

    if (scan_time_sec <= 0.0)
        return result;

    auto pitch = nMathUtils::wrap_neg_180_to_180(mount_pitch_deg + pitch_offset_deg);
    auto roll = nMathUtils::wrap_neg_180_to_180(mount_roll_deg + roll_offset_deg);
    auto yaw = nMathUtils::wrap_0_to_360(mount_yaw_deg + yaw_offset_deg);

    kdt::sDollyOrientation_t entry;
    entry.timestamp_us = 0;
    entry.pitch_deg = pitch;
    entry.roll_deg = roll;
    entry.yaw_deg = yaw;

    entry.pitchRate_dps = 0;
    entry.rollRate_dps = 0;
    entry.yawRate_dps = 0;

    result.push_back(entry);

#ifdef USE_UPDATE_PROGRESS
    if (id >= 0)
        update_progress(id, 50);
#endif

    entry.timestamp_us = scan_time_sec * nConstants::SEC_TO_US;
    result.push_back(entry);

#ifdef USE_UPDATE_PROGRESS
    if (id >= 0)
        update_progress(id, 100);
#endif

    return result;
}

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    kdt::sDollyAtitude_t start, kdt::sDollyAtitude_t end, double scan_time_sec)
{
    std::vector<kdt::sDollyOrientation_t> result;

    if (scan_time_sec <= 0.0)
        return result;

    auto start_pitch = nMathUtils::wrap_neg_180_to_180(mount_pitch_deg + start.pitch_deg);
    auto start_roll = nMathUtils::wrap_neg_180_to_180(mount_roll_deg + start.roll_deg);
    auto start_yaw = nMathUtils::wrap_0_to_360(mount_yaw_deg + start.yaw_deg);

    auto end_pitch = nMathUtils::wrap_neg_180_to_180(mount_pitch_deg + end.pitch_deg);
    auto end_roll = nMathUtils::wrap_neg_180_to_180(mount_roll_deg + end.roll_deg);
    auto end_yaw = nMathUtils::wrap_0_to_360(mount_yaw_deg + end.yaw_deg);

    auto pitch_rate = (end_pitch - start_pitch) / scan_time_sec;
    auto roll_rate = (end_roll - start_roll) / scan_time_sec;
    auto yaw_rate = (end_yaw - start_yaw) / scan_time_sec;

    kdt::sDollyOrientation_t entry;
    entry.timestamp_us = 0;
    entry.pitch_deg = start_pitch;
    entry.roll_deg = start_roll;
    entry.yaw_deg = start_yaw;

    entry.pitchRate_dps = pitch_rate;
    entry.rollRate_dps = roll_rate;
    entry.yawRate_dps = yaw_rate;

    result.push_back(entry);

#ifdef USE_UPDATE_PROGRESS
    if (id >= 0)
        update_progress(id, 50);
#endif

    entry.timestamp_us = scan_time_sec * nConstants::SEC_TO_US;
    entry.pitch_deg = end_pitch;
    entry.roll_deg = end_roll;
    entry.yaw_deg = end_yaw;

    entry.pitchRate_dps = 0;
    entry.rollRate_dps = 0;
    entry.yawRate_dps = 0;

    result.push_back(entry);

#ifdef USE_UPDATE_PROGRESS
    if (id >= 0)
        update_progress(id, 100);
#endif

    return result;
}


std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    std::vector<kdt::sDollyOrientationInterpPoint_t> table, double scan_time_sec)
{
    std::vector<kdt::sDollyOrientation_t> result;

    if ((scan_time_sec <= 0.0) || (table.empty()))
        return result;

    if (table.size() == 1)
    {
        auto point = table.front();

        kdt::sDollyOrientation_t entry;
        entry.timestamp_us = 0;
        entry.pitch_deg = point.pitch_deg;
        entry.roll_deg = point.roll_deg;
        entry.yaw_deg = point.yaw_deg;

        entry.pitchRate_dps = 0;
        entry.rollRate_dps = 0;
        entry.yawRate_dps = 0;

        result.push_back(entry);

        entry.timestamp_us = scan_time_sec * nConstants::SEC_TO_US;
        result.push_back(entry);

        result.push_back(entry);

        return result;
    }

    // Make sure the table is sorted from 0% to 100%
    std::sort(table.begin(), table.end(), [](const auto& a, const auto& b)
        {
            return a.distance_pct < b.distance_pct;
        });

    auto point = table.front();
    if (point.distance_pct > 0.0)
    {
        point.distance_pct = 0.0;
        table.insert(table.begin(), point);
    }

    point = table.back();
    if (point.distance_pct < 100.0)
    {
        point.distance_pct = 100.0;
        table.push_back(point);
    }

    auto n = table.size() - 1;

    for (int i = 0; i < n; ++i)
    {
        auto start = table[i];
        auto start_pitch = nMathUtils::wrap_neg_180_to_180(mount_pitch_deg + start.pitch_deg);
        auto start_roll = nMathUtils::wrap_neg_180_to_180(mount_roll_deg + start.roll_deg);
        auto start_yaw = nMathUtils::wrap_0_to_360(mount_yaw_deg + start.yaw_deg);

        auto end = table[i + 1];
        auto end_pitch = nMathUtils::wrap_neg_180_to_180(mount_pitch_deg + end.pitch_deg);
        auto end_roll = nMathUtils::wrap_neg_180_to_180(mount_roll_deg + end.roll_deg);
        auto end_yaw = nMathUtils::wrap_0_to_360(mount_yaw_deg + end.yaw_deg);

        double delta_time_sec = scan_time_sec * (end.distance_pct / 100.0) - scan_time_sec * (start.distance_pct / 100.0);
        auto pitch_rate = (end_pitch - start_pitch) / delta_time_sec;
        auto roll_rate = (end_roll - start_roll) / delta_time_sec;
        auto yaw_rate = (end_yaw - start_yaw) / delta_time_sec;

        kdt::sDollyOrientation_t entry;
        entry.timestamp_us = (scan_time_sec * (start.distance_pct / 100.0)) * nConstants::SEC_TO_US;
        entry.pitch_deg = start_pitch;
        entry.roll_deg = start_roll;
        entry.yaw_deg = start_yaw;

        entry.pitchRate_dps = pitch_rate;
        entry.rollRate_dps = roll_rate;
        entry.yawRate_dps = yaw_rate;

        result.push_back(entry);
    }

    auto last = table.back();

    kdt::sDollyOrientation_t entry;
    entry.timestamp_us = scan_time_sec * nConstants::SEC_TO_US;
    entry.pitch_deg = nMathUtils::wrap_neg_180_to_180(mount_pitch_deg + last.pitch_deg);;
    entry.roll_deg = nMathUtils::wrap_neg_180_to_180(mount_roll_deg + last.roll_deg);
    entry.yaw_deg = nMathUtils::wrap_0_to_360(mount_yaw_deg + last.yaw_deg);

    entry.pitchRate_dps = 0;
    entry.rollRate_dps = 0;
    entry.yawRate_dps = 0;

    result.push_back(entry);

    return result;
}


std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
    const std::deque<nOusterTypes::imu_data_t>& imu, nOusterTypes::imu_intrinsics_2_t transform)
{
    std::vector<kdt::sDollyOrientation_t> result;

    if (imu.empty())
        return result;

    ouster::cTransformMatrix<double>	imuTransform;
    imuTransform.set(transform.imu_to_sensor_transform, true);

    auto imuToSensor = imuTransform.rotation();
    auto sensorToDolly = computeSensorOrientation(mount_yaw_deg, mount_pitch_deg, mount_roll_deg);

    double angular_velocity_bias_Xaxis_deg_per_sec = 0.0;
    double angular_velocity_bias_Yaxis_deg_per_sec = 0.0;
    double angular_velocity_bias_Zaxis_deg_per_sec = 0.0;

    for (const auto& data : imu)
    {
        angular_velocity_bias_Xaxis_deg_per_sec += data.angular_velocity_Xaxis_deg_per_sec;
        angular_velocity_bias_Yaxis_deg_per_sec += data.angular_velocity_Yaxis_deg_per_sec;
        angular_velocity_bias_Zaxis_deg_per_sec += data.angular_velocity_Zaxis_deg_per_sec;
    }

    angular_velocity_bias_Xaxis_deg_per_sec /= imu.size();
    angular_velocity_bias_Yaxis_deg_per_sec /= imu.size();
    angular_velocity_bias_Zaxis_deg_per_sec /= imu.size();

    auto it = imu.begin();

    nOusterTypes::imu_data_t data = *it;

    rotate(data.acceleration_Xaxis_g, data.acceleration_Yaxis_g, data.acceleration_Zaxis_g, imuToSensor);
    rotate(data.acceleration_Xaxis_g, data.acceleration_Yaxis_g, data.acceleration_Zaxis_g, sensorToDolly);

    rotate(data.angular_velocity_Xaxis_deg_per_sec, data.angular_velocity_Yaxis_deg_per_sec,
        data.angular_velocity_Zaxis_deg_per_sec, imuToSensor);

    rotate(data.angular_velocity_Xaxis_deg_per_sec, data.angular_velocity_Yaxis_deg_per_sec,
        data.angular_velocity_Zaxis_deg_per_sec, sensorToDolly);

    double startGyroTime_ns = data.gyroscope_read_time_ns;
    double prevGyroTime_ns = data.gyroscope_read_time_ns;

    double x2 = data.acceleration_Xaxis_g * data.acceleration_Xaxis_g;
    double y2 = data.acceleration_Yaxis_g * data.acceleration_Yaxis_g;
    double z2 = data.acceleration_Zaxis_g * data.acceleration_Zaxis_g;

    kdt::sDollyOrientation_t entry;
    entry.timestamp_us = 0;
    entry.pitch_deg = atan(data.acceleration_Zaxis_g / sqrt(x2 + y2)) * nConstants::RAD_TO_DEG;
    entry.roll_deg  = atan(data.acceleration_Yaxis_g / sqrt(x2 + z2)) * nConstants::RAD_TO_DEG;
    entry.yaw_deg   = mount_yaw_deg;

    entry.pitchRate_dps = 0;
    entry.rollRate_dps  = 0;
    entry.yawRate_dps   = 0;

    result.push_back(entry);

    for (++it; it != imu.end(); ++it)
    {
        nOusterTypes::imu_data_t data = *it;

        // Remove the bias from the gryo
        data.angular_velocity_Xaxis_deg_per_sec -= angular_velocity_bias_Xaxis_deg_per_sec;
        data.angular_velocity_Yaxis_deg_per_sec -= angular_velocity_bias_Yaxis_deg_per_sec;
        data.angular_velocity_Zaxis_deg_per_sec -= angular_velocity_bias_Zaxis_deg_per_sec;

        rotate(data.acceleration_Xaxis_g, data.acceleration_Yaxis_g, data.acceleration_Zaxis_g, imuToSensor);
        rotate(data.acceleration_Xaxis_g, data.acceleration_Yaxis_g, data.acceleration_Zaxis_g, sensorToDolly);

        rotate(data.angular_velocity_Xaxis_deg_per_sec, data.angular_velocity_Yaxis_deg_per_sec,
            data.angular_velocity_Zaxis_deg_per_sec, imuToSensor);

        rotate(data.angular_velocity_Xaxis_deg_per_sec, data.angular_velocity_Yaxis_deg_per_sec,
            data.angular_velocity_Zaxis_deg_per_sec, sensorToDolly);

        entry.timestamp_us = (data.gyroscope_read_time_ns - startGyroTime_ns) * nConstants::NS_TO_US;

        double x2 = data.acceleration_Xaxis_g * data.acceleration_Xaxis_g;
        double y2 = data.acceleration_Yaxis_g * data.acceleration_Yaxis_g;
        double z2 = data.acceleration_Zaxis_g * data.acceleration_Zaxis_g;

        entry.pitch_deg = atan(data.acceleration_Zaxis_g / sqrt(x2 + y2)) * nConstants::RAD_TO_DEG;
        entry.roll_deg = atan(data.acceleration_Yaxis_g / sqrt(x2 + z2)) * nConstants::RAD_TO_DEG;

        auto& previous = result.back();

        double dt_s = (data.gyroscope_read_time_ns - prevGyroTime_ns) * nConstants::NS_TO_SEC;

        entry.yaw_deg = previous.yaw_deg + data.angular_velocity_Zaxis_deg_per_sec * dt_s;

        previous.pitchRate_dps = (entry.pitch_deg - previous.pitch_deg) / dt_s;
        previous.rollRate_dps  = (entry.roll_deg - previous.roll_deg) / dt_s;
        previous.yawRate_dps   = (entry.yaw_deg - previous.yaw_deg) / dt_s;

        prevGyroTime_ns = data.gyroscope_read_time_ns;

        result.push_back(entry);
    }

    return result;
}

void mergeDollyOrientation(int id, std::vector<kdt::sDollyInfo_t>& dolly, const std::vector<kdt::sDollyOrientation_t>& orientation)
{
    int i = 0;
    auto n = 2*dolly.size();

    // Fill in the atitude information
    for (auto& pos : dolly)
    {
#ifdef USE_UPDATE_PROGRESS
        if (id >= 0)
        {
            int progress_pct = static_cast<int>(100.0 * i++ / n);
            update_progress(id, progress_pct);
        }
#endif

        auto time_us = pos.timestamp_us;

        auto it = std::upper_bound(orientation.begin(), orientation.end(), time_us, [](double time_us, const kdt::sDollyOrientation_t& p)
            {
                return time_us < p.timestamp_us;
            });

        if (it == orientation.end())
        {
            auto atitude = orientation.back();
            pos.pitch_deg = atitude.pitch_deg;
            pos.roll_deg = atitude.roll_deg;
            pos.yaw_deg = atitude.yaw_deg;

            pos.pitchRate_dps = 0;
            pos.rollRate_dps = 0;
            pos.yawRate_dps = 0;
        }
        else
        {
            // We want the dolly orientation just before the dolly position entry
            if (it != orientation.begin())
                --it;

            auto atitude = *it;

            auto dt_us = time_us - atitude.timestamp_us;
            auto dt_s = dt_us * nConstants::US_TO_SEC;

            pos.pitch_deg = atitude.pitch_deg + atitude.pitchRate_dps * dt_s;
            pos.roll_deg = atitude.roll_deg + atitude.rollRate_dps * dt_s;
            pos.yaw_deg = atitude.yaw_deg + atitude.yawRate_dps * dt_s;

            pos.pitchRate_dps = 0;
            pos.rollRate_dps = 0;
            pos.yawRate_dps = 0;
        }
    }

    // Compute atitude rates
    auto it = dolly.begin();
    for (++it; it != dolly.end(); ++it)
    {
#ifdef USE_UPDATE_PROGRESS
        if (id >= 0)
        {
            int progress_pct = static_cast<int>(100.0 * i++ / n);
            update_progress(id, progress_pct);
        }
#endif

        auto current = *it;

        auto prev = it;
        --prev;

        auto dt_us = current.timestamp_us - prev->timestamp_us;
        auto dt_s = dt_us * nConstants::US_TO_SEC;

        prev->pitchRate_dps = (current.pitch_deg - prev->pitch_deg) / dt_s;
        prev->rollRate_dps = (current.roll_deg - prev->roll_deg) / dt_s;
        prev->yawRate_dps = (current.yaw_deg - prev->yaw_deg) / dt_s;
    }

}

bool transform(double time_us, const std::vector<kdt::sDollyInfo_t>& path,
                ouster::matrix_col_major<rfm::sPoint3D_t>& cloud,
                std::vector<kdt::sDollyInfo_t>* pComputedPath, double* displacement_mm)
{
    if (path.empty()) return false;
    if (time_us < 0.0) return false;
    if (time_us > path.back().timestamp_us) return false;

    kdt::sDollyInfo_t dolly;

    if (time_us == 0.0)
    {
        dolly = path.front();
    }
    else
    {
        auto it = std::upper_bound(path.begin(), path.end(),time_us,
            [](double time_us, const kdt::sDollyInfo_t& p)
                {
                    return time_us < p.timestamp_us;
                });

        // The upper bound is one past what we want!
        --it;

        dolly = *it;
    }

    double dtime_sec = (time_us - dolly.timestamp_us) * nConstants::US_TO_SEC;

    double pitch_deg = dolly.pitch_deg + dolly.pitchRate_dps * dtime_sec;
    double roll_deg  = dolly.roll_deg + dolly.rollRate_dps * dtime_sec;
    double yaw_deg   = dolly.yaw_deg + dolly.yawRate_dps * dtime_sec;

    double yaw_south_deg = nMathUtils::wrap_0_to_360(180.0 + yaw_deg);

    auto sensorToSEU = computeSensorOrientation(yaw_south_deg, pitch_deg, roll_deg);

    // The dolly x movement is the north/south direction
    double southPos_mm = dolly.x_mm + dolly.vx_mmps * dtime_sec;

    // The dolly y movement is the east/west direction
    double eastPos_mm = dolly.y_mm + dolly.vy_mmps * dtime_sec;

    if (displacement_mm)
    {
        double dx = dolly.vx_mmps * dtime_sec;
        double dy = dolly.vy_mmps * dtime_sec;
        double d = sqrt(dx*dx + dy*dy);
        *displacement_mm += d;
    }

    double heightPos_m = dolly.z_mm + dolly.vz_mmps * dtime_sec;

    auto cols = cloud.num_columns();
    auto rows = cloud.num_rows();

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            auto point = cloud.get(r, c);

            if ((point.x_mm == 0.0) && (point.y_mm == 0.0) && (point.z_mm == 0.0))
                continue;

            rotate(point.x_mm, point.y_mm, point.z_mm, sensorToSEU);

            point.x_mm += southPos_mm;
            point.y_mm += eastPos_mm;
            point.z_mm = heightPos_m - point.z_mm;

            cloud.set(r, c, point);
        }
    }

    if (pComputedPath)
    {
        kdt::sDollyInfo_t info;

        info.timestamp_us = time_us;
        info.x_mm = southPos_mm;
        info.y_mm = eastPos_mm;
        info.z_mm = heightPos_m;

        info.vx_mmps = dolly.vx_mmps;
        info.vy_mmps = dolly.vy_mmps;
        info.vz_mmps = dolly.vz_mmps;

        info.pitch_deg = pitch_deg;
        info.roll_deg  = roll_deg;
        info.yaw_deg   = yaw_deg;

        info.pitchRate_dps = dolly.pitchRate_dps;
        info.rollRate_dps  = dolly.rollRate_dps;
        info.yawRate_dps   = dolly.yawRate_dps;

        pComputedPath->push_back(info);
    }

    return true;
}

