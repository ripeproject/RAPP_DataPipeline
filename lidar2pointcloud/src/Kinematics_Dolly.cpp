

#include "Kinematics_Dolly.hpp"
#include "PointCloudSerializer.hpp"

#include <cbdf/BlockDataFile.hpp>


cKinematics_Dolly::cKinematics_Dolly()
:
    mSerializer(4096)
{
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
}

void cKinematics_Dolly::detachKinematicParsers(cBlockDataFileReader& file)
{
    file.detach(static_cast<cSpidercamParser*>(this)->blockID());
    file.detach(static_cast<cExperimentParser*>(this)->blockID());
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
        sDollyInfo_t info = mDollyInfo.front();

        // The dolly x movement is the north/south direction
        mSouth_Offset_m = info.x_m;

        // The dolly y movement is the east/west direction
        mEast_Offset_m = info.y_m;

        mHeight_Offset_m = info.z_m;

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

    if (mDollyInfoIndex == mDollyPassIndex)
    {
        ++mPassNumber;
        mDollyPassIndex = mPassIndex[mPassNumber];
    }

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

        sDollyInfo_t info = mDollyInfo[mDollyInfoIndex];

        // The dolly x movement is the north/south direction
        mSouth_Offset_m = info.x_m;

        // The dolly y movement is the east/west direction
        mEast_Offset_m = info.y_m;

        mHeight_Offset_m = info.z_m;
    }

    sDollyInfo_t info = mDollyInfo[mDollyInfoIndex];

    double time_sec = (time_us - info.timestamp_us) / 1000000.0;

    // The dolly x movement is the north/south direction
    mSouth_m = mSouth_Offset_m + info.vx_mps * time_sec;

    // The dolly y movement is the east/west direction
    mEast_m = mEast_Offset_m + info.vy_mps * time_sec;

    mHeight_m = mHeight_Offset_m + info.vz_mps * time_sec;

    auto cols = cloud.num_columns();
    auto rows = cloud.num_rows();

    for (int c = 0; c < cols; ++c)
    {
        for (int r = 0; r < rows; ++r)
        {
            auto point = cloud.get(r, c);

            if ((point.X_m == 0.0) && (point.Y_m == 0.0) && (point.Z_m == 0.0))
                continue;

            point.X_m += mSouth_m;
            point.Y_m += mEast_m;
            point.Z_m = mHeight_m - point.Z_m;

            cloud.set(r, c, point);
        }
    }
}

void cKinematics_Dolly::onPosition(spidercam::sPosition_1_t position)
{
    if (!mDataActive)
        return;

    sDollyInfo_t info;

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

        double dts = position.timestamp - mStartTimestamp;
        info.timestamp_us = dts * 0.1;
    }
    else
    {
        auto& previous = mDollyInfo.back();

        double dx = (position.X_mm * 0.001) - previous.x_m;
        double dy = (position.Y_mm * 0.001) - previous.y_m;
        double dz = (position.Z_mm * 0.001) - previous.z_m;

        double dts = position.timestamp - mStartTimestamp;
        double dt = dts * 0.0000001;

        info.timestamp_us = dt * 1'000'000.0;

        previous.vx_mps = dx / dt;
        previous.vy_mps = dy / dt;
        previous.vz_mps = dz / dt;
    }

    info.x_m = position.X_mm * 0.001;
    info.y_m = position.Y_mm * 0.001;
    info.z_m = position.Z_mm * 0.001;

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
    mPassIndex.push_back(mDollyInfo.size());
    mDataActive = false;
    ++mNumPasses;

    if (mSerializer)
        mSerializer.endRecordingTimestamp(timestamp_ns);
}

void cKinematics_Dolly::onHeartbeatTimestamp(uint64_t timestamp_ns)
{
    if (mSerializer)
        mSerializer.heartbeatTimestamp(timestamp_ns);
}
