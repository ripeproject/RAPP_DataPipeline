

#include "Kinematics_Dolly.hpp"
#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <ouster/simple_blas.h>

#include <numbers>
#include <cmath>


namespace
{
    constexpr double RAD_TO_DEG = 180.0 / std::numbers::pi;
    constexpr double DEG_TO_RAD = std::numbers::pi / 180.0;

    constexpr double SEC_TO_MS = 1000.0;
    constexpr double MS_TO_SEC = 1.0 / SEC_TO_MS;

    constexpr double SEC_TO_US = 1'000'000.0;
    constexpr double US_TO_SEC = 1.0 / SEC_TO_US;

    template<typename T>
    inline void rotate(ouster::imu_data_t& lhs, const ouster::cRotationMatrix<T>& r)
    {
/*
        const auto& rX = r.column(0);
        const auto& rY = r.column(1);
        const auto& rZ = r.column(2);

        data.acceleration_Xaxis_g, data.acceleration_Yaxis_g, data.acceleration_Zaxis_g
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            sPoint_t p = lhs[i];

            double x = p.x * rX[0] + p.y * rX[1] + p.z * rX[2];
            double y = p.x * rY[0] + p.y * rY[1] + p.z * rY[2];
            double z = p.x * rZ[0] + p.y * rZ[1] + p.z * rZ[2];

            lhs[i] = sPoint_t(x, y, z);
        }
*/
    }

    template<typename T>
    inline void translate(ouster::imu_data_t& lhs, const ouster::cTranslation<T>& t)
    {
/*
        data.acceleration_Xaxis_g
        data.acceleration_Yaxis_g
        data.acceleration_Zaxis_g
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
*/
    }

}

cKinematics_Dolly::cKinematics_Dolly()
:
    mSerializer(4096)
{
}

bool cKinematics_Dolly::usingImuData() const
{
    return mUseImuData;
}

void cKinematics_Dolly::useImuData(bool useImuData)
{
    mUseImuData = useImuData;
}

bool cKinematics_Dolly::averagingImuData() const
{
    return mUseAverageOrientation;
}

void cKinematics_Dolly::averageImuData(bool average)
{
    mUseAverageOrientation = average;
}

double cKinematics_Dolly::sensorPitchOffset_deg() const
{
    return mOffsetPitch_deg;
}

void cKinematics_Dolly::setSensorPitchOffset_deg(double offset_deg)
{
    mOffsetPitch_deg = offset_deg;
}

double cKinematics_Dolly::sensorRollOffset_deg() const
{
    return mOffsetRoll_deg;
}

void cKinematics_Dolly::setSensorRollOffset_deg(double offset_deg)
{
    mOffsetRoll_deg = offset_deg;
}

void cKinematics_Dolly::writeHeader(cPointCloudSerializer& serializer)
{
    serializer.write(pointcloud::eKINEMATIC_MODEL::DOLLY);
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::attachKinematicParsers(cBlockDataFileReader& file)
{
    file.attach(static_cast<cSpidercamParser*>(this));
    file.attach(static_cast<cExperimentParser*>(this));
    file.attach(static_cast<cOusterParser*>(this));
}

void cKinematics_Dolly::detachKinematicParsers(cBlockDataFileReader& file)
{
    file.detach(static_cast<cSpidercamParser*>(this)->blockID());
    file.detach(static_cast<cExperimentParser*>(this)->blockID());
    file.detach(static_cast<cOusterParser*>(this)->blockID());
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::attachTransformParsers(cBlockDataFileReader& file)
{
//    file.attach(static_cast<cExperimentParser*>(this));
}

void cKinematics_Dolly::detachTransformParsers(cBlockDataFileReader& file)
{
//    file.detach(static_cast<cExperimentParser*>(this)->blockID());
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::attachTransformSerializers(cBlockDataFileWriter& file)
{
//    mSerializer.attach(&file);
}

void cKinematics_Dolly::detachTransformSerializers(cBlockDataFileWriter& file)
{
//    mSerializer.detach();
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::telemetryPassComplete()
{
    mDollyInfoIndex = 0;
    mDollyInfoMax = 0;

    mPassNumber = 0;
    mDollyPassIndex = static_cast<std::size_t>(-1);

    if (!mDollyInfo.empty())
    {
        if (mUseAverageOrientation)
        {
            double pitch_deg = mPitchSum / mImuCount;
            double roll_deg = mRollSum / mImuCount;

            for (auto& info : mDollyInfo)
            {
                info.pitch_deg = pitch_deg;
                info.roll_deg = roll_deg;
                info.pitchRate_dps = 0.0;
                info.rollRate_dps = 0.0;
            }

            mPitchSum = 0.0;
            mRollSum = 0.0;
            mImuCount = 0;
        }

        sSensorInfo_t info = mDollyInfo.front();

        // Set the sensor orientation
        mInitPitch_deg = info.pitch_deg;
        mInitRoll_deg = info.roll_deg;

        // Set the initial position of the dolly in the SEU system
        mInitSouthPos_m = info.x_m;
        mInitEastPos_m = info.y_m;
        mInitHeightPos_m = info.z_m;

        mDollyInfoMax = mDollyInfo.size() - 1;
    }

    if (!mPassIndex.empty())
        mDollyPassIndex = mPassIndex[0];
}

//-----------------------------------------------------------------------------
void cKinematics_Dolly::transform(double time_us,
    ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud)
{
    if (mDollyInfo.empty())
    {
        throw cKinematicNoData("File does not contain dolly information.");
    }

    mAtEndOfPass = false;

    auto lower = mDollyInfoIndex;

    while (lower < mDollyInfoMax)
    {
        if (time_us < mDollyInfo[lower+1].timestamp_us)
            break;
        ++lower;
    }

    if (lower != mDollyInfoIndex)
    {
        mDollyInfoIndex = lower;

        sSensorInfo_t info = mDollyInfo[mDollyInfoIndex];

        // Set the sensor orientation
        mInitPitch_deg = info.pitch_deg;
        mInitRoll_deg = info.roll_deg;

        // Set the initial position of the dolly in the SEU system
        mInitSouthPos_m = info.x_m;
        mInitEastPos_m = info.y_m;
        mInitHeightPos_m = info.z_m;
    }

    sSensorInfo_t info = mDollyInfo[mDollyInfoIndex];

    double dtime_sec = (time_us - info.timestamp_us) * US_TO_SEC;

    double pitch_deg = mInitPitch_deg + mOffsetPitch_deg + info.pitchRate_dps * dtime_sec;
    double roll_deg  = mInitRoll_deg + mOffsetRoll_deg + info.rollRate_dps * dtime_sec;

    if (mUseImuData)
    {
        setSensorOrientation(pitch_deg, roll_deg);
    }

    // The dolly x movement is the north/south direction
    mSouthPos_m = mInitSouthPos_m + info.vx_mps * dtime_sec;

    // The dolly y movement is the east/west direction
    mEastPos_m = mInitEastPos_m + info.vy_mps * dtime_sec;

    mHeightPos_m = mInitHeightPos_m + info.vz_mps * dtime_sec;

    auto cols = cloud.num_columns();
    auto rows = cloud.num_rows();

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            auto point = cloud.get(r, c);

            if ((point.X_m == 0.0) && (point.Y_m == 0.0) && (point.Z_m == 0.0))
                continue;

            if (rotateToSEU())
                rotate(point);

            point.X_m += mSouthPos_m;
            point.Y_m += mEastPos_m;
            point.Z_m  = mHeightPos_m - point.Z_m;

            cloud.set(r, c, point);
        }
    }

    // Check for an end of pass event.
    // Note: we use >= because it is possible for the dolly info index
    // to jump passed the end of pass index
    if (mDollyInfoIndex >= mDollyPassIndex)
    {
        ++mPassNumber;

        if (mPassNumber < mPassIndex.size())
            mDollyPassIndex = mPassIndex[mPassNumber];
        else
            mDollyPassIndex = static_cast<std::size_t>(-1);

        mAtEndOfPass = true;
    }
}

void cKinematics_Dolly::onPosition(spidercam::sPosition_1_t position)
{
    if (!mDataActive)
        return;

    sSensorInfo_t info;

    double pitch_deg = 0.0;
    double roll_deg = 0.0;

    if (!mUseAverageOrientation)
    {
        pitch_deg = mPitchSum / mImuCount;
        roll_deg = mRollSum / mImuCount;
    }

    if (mDollyInfo.empty())
    {
        mStartTimestamp = position.timestamp;
        info.timestamp_us = 0;
    }

    if (mStartPath)
    {
        mStartPath = false;
        info.vx_mps = 0;
        info.vy_mps = 0;
        info.vz_mps = 0;

        info.pitchRate_dps = 0;
        info.rollRate_dps = 0;

        double dts = position.timestamp - mStartTimestamp;
        info.timestamp_us = dts * 0.1;
    }
    else
    {
        auto& previous = mDollyInfo.back();

        double dx = (position.X_mm * MM_TO_M) - previous.x_m;
        double dy = (position.Y_mm * MM_TO_M) - previous.y_m;
        double dz = (position.Z_mm * MM_TO_M) - previous.z_m;

        double dp = pitch_deg - previous.pitch_deg;
        double dr = roll_deg - previous.roll_deg;

        double dt_us = (position.timestamp - mStartTimestamp) * 0.1;
        info.timestamp_us = dt_us;

        double dt = (dt_us - previous.timestamp_us) * US_TO_SEC;

        previous.vx_mps = dx / dt;
        previous.vy_mps = dy / dt;
        previous.vz_mps = dz / dt;

        previous.pitchRate_dps = dp / dt;
        previous.rollRate_dps  = dr / dt;
    }

    info.x_m = position.X_mm * MM_TO_M;
    info.y_m = position.Y_mm * MM_TO_M;
    info.z_m = position.Z_mm * MM_TO_M;

    info.pitch_deg = pitch_deg;
    info.roll_deg = roll_deg;

    if (!mUseAverageOrientation)
    {
        mPitchSum = 0;
        mRollSum = 0;
        mImuCount = 0;
    }

    mDollyInfo.push_back(info);
}

void cKinematics_Dolly::onBeginHeader()
{
    if (mSerializer)
        mSerializer.writeBeginHeader();
}

void cKinematics_Dolly::onEndOfHeader()
{
    if (mSerializer)
        mSerializer.writeEndOfHeader();
}

void cKinematics_Dolly::onBeginFooter()
{
    if (mSerializer)
        mSerializer.writeBeginFooter();
}

void cKinematics_Dolly::onEndOfFooter()
{
    if (mSerializer)
        mSerializer.writeEndOfFooter();
}

void cKinematics_Dolly::onTitle(const std::string& title)
{
    if (mSerializer)
        mSerializer.writeTitle(title);
}

void cKinematics_Dolly::onPrincipalInvestigator(const std::string& investigator)
{
    if (mSerializer)
        mSerializer.writePrincipalInvestigator(investigator);
}

void cKinematics_Dolly::onBeginResearcherList()
{
    if (mSerializer)
        mSerializer.writeBeginResearchers();
}

void cKinematics_Dolly::onEndOfResearcherList()
{
    if (mSerializer)
        mSerializer.writeEndOfResearchers();
}

void cKinematics_Dolly::onResearcher(const std::string& researcher)
{
    if (mSerializer)
        mSerializer.writeResearcher(researcher);
}

void cKinematics_Dolly::onSpecies(const std::string& species)
{
    if (mSerializer)
        mSerializer.writeSpecies(species);
}

void cKinematics_Dolly::onCultivar(const std::string& cultivar)
{
    if (mSerializer)
        mSerializer.writeCultivar(cultivar);
}

void cKinematics_Dolly::onPermitInfo(const std::string& permit)
{
    if (mSerializer)
        mSerializer.writePermitInfo(permit);
}

void cKinematics_Dolly::onExperimentDoc(const std::string& doc)
{
    if (mSerializer)
        mSerializer.writeExperimentDoc(doc);
}

void cKinematics_Dolly::onBeginTreatmentList()
{
    if (mSerializer)
        mSerializer.writeBeginTreatments();
}

void cKinematics_Dolly::onEndOfTreatmentList()
{
    if (mSerializer)
        mSerializer.writeEndOfTreatments();
}

void cKinematics_Dolly::onTreatment(const std::string& treatment)
{
    if (mSerializer)
        mSerializer.writeTreatment(treatment);
}

void cKinematics_Dolly::onConstructName(const std::string& name)
{
    if (mSerializer)
        mSerializer.writeConstructName(name);
}

void cKinematics_Dolly::onBeginEventNumberList()
{}

void cKinematics_Dolly::onEndOfEventNumberList()
{}

void cKinematics_Dolly::onEventNumber(const std::string& event)
{
    if (mSerializer)
        mSerializer.writeEventNumber(event);
}

void cKinematics_Dolly::onFieldDesign(const std::string& design)
{
    if (mSerializer)
        mSerializer.writeFieldDesign(design);
}

void cKinematics_Dolly::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
    if (mSerializer)
        mSerializer.writePlantingDate(year,month,day,doy);
}

void cKinematics_Dolly::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
    if (mSerializer)
        mSerializer.writeHarvestDate(year, month, day, doy);
}

void cKinematics_Dolly::onBeginCommentList()
{
    if (mSerializer)
        mSerializer.writeBeginComments();
}

void cKinematics_Dolly::onEndOfCommentList()
{
    if (mSerializer)
        mSerializer.writeEndOfComments();
}

void cKinematics_Dolly::onComment(const std::string& comment)
{
    if (mSerializer)
        mSerializer.writeComment(comment);
}

void cKinematics_Dolly::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day)
{
    if (mSerializer)
        mSerializer.writeFileDate(year, month, day);
}

void cKinematics_Dolly::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds)
{
    if (mSerializer)
        mSerializer.writeFileTime(hour, minute, seconds);
}

void cKinematics_Dolly::onDayOfYear(std::uint16_t day_of_year)
{
    if (mSerializer)
        mSerializer.writeDayOfYear(day_of_year);
}

void cKinematics_Dolly::onBeginSensorList()
{
    if (mSerializer)
        mSerializer.writeBeginSensorList();
}

void cKinematics_Dolly::onEndOfSensorList()
{
    if (mSerializer)
        mSerializer.writeEndOfSensorList();
}

void cKinematics_Dolly::onSensorBlockInfo(uint16_t class_id, const std::string& name)
{
    if (mSerializer)
        mSerializer.writeSensorBlockInfo(class_id, name);
}

void cKinematics_Dolly::onStartTime(sExperimentTime_t time)
{
    if (mSerializer)
    {
        mSerializer.startTime(time.year, time.month, time.day,
            time.hour, time.minutes, time.seconds);
    }
}

void cKinematics_Dolly::onEndTime(sExperimentTime_t time)
{
    if (mSerializer)
    {
        mSerializer.endTime(time.year, time.month, time.day,
            time.hour, time.minutes, time.seconds);
    }
}

void cKinematics_Dolly::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    mStartPath = true;
    mDataActive = true;

    if (mSerializer)
        mSerializer.startRecordingTimestamp(timestamp_ns);
}

void cKinematics_Dolly::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mDataActive = false;

    if (!mDollyInfo.empty())
    {
        mPassIndex.push_back(mDollyInfo.size() - 1);
        ++mNumPasses;
    }

    if (mSerializer)
        mSerializer.endRecordingTimestamp(timestamp_ns);
}

void cKinematics_Dolly::onHeartbeatTimestamp(uint64_t timestamp_ns)
{
    if (mSerializer)
        mSerializer.heartbeatTimestamp(timestamp_ns);
}

void cKinematics_Dolly::onConfigParam(ouster::config_param_2_t config_param) {};
void cKinematics_Dolly::onSensorInfo(ouster::sensor_info_2_t sensor_info) {};
void cKinematics_Dolly::onTimestamp(ouster::timestamp_2_t timestamp) {};
void cKinematics_Dolly::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) {};
void cKinematics_Dolly::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) {};
void cKinematics_Dolly::onMultipurposeIo(ouster::multipurpose_io_2_t io) {};
void cKinematics_Dolly::onNmea(ouster::nmea_2_t nmea) {};
void cKinematics_Dolly::onTimeInfo(ouster::time_info_2_t time_info) {};
void cKinematics_Dolly::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) {};

void cKinematics_Dolly::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics)
{
    mImuIntrinsics = intrinsics;
    mImuTransform.set(mImuIntrinsics.imu_to_sensor_transform, true);
    mImuToSensor = mImuTransform.rotation();
}

void cKinematics_Dolly::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) {};
void cKinematics_Dolly::onLidarDataFormat(ouster::lidar_data_format_2_t format) {};

void cKinematics_Dolly::onImuData(ouster::imu_data_t data)
{
/*
    auto rot = mImuTransform.rotation();
    auto trans = mImuTransform.translation();

    ::rotate(data, rot);
    ::translate(data, trans);

    rotate(data.acceleration_Xaxis_g, data.acceleration_Yaxis_g, data.acceleration_Zaxis_g, mImuToSensor);

    rotate(data.angular_velocity_Xaxis_deg_per_sec, data.angular_velocity_Yaxis_deg_per_sec,
        data.angular_velocity_Zaxis_deg_per_sec, mImuToSensor);
*/

    double x2 = data.acceleration_Xaxis_g * data.acceleration_Xaxis_g;
    double y2 = data.acceleration_Yaxis_g * data.acceleration_Yaxis_g;
    double z2 = data.acceleration_Zaxis_g * data.acceleration_Zaxis_g;

    double pitch_deg = atan(data.acceleration_Xaxis_g / sqrt(y2 + z2)) * RAD_TO_DEG;
    double roll_deg  = atan(data.acceleration_Yaxis_g / sqrt(x2 + z2)) * RAD_TO_DEG;

    mPitchSum += pitch_deg;
    mRollSum  += roll_deg;

    ++mImuCount;
};

void cKinematics_Dolly::onLidarData(cOusterLidarData data) {};
