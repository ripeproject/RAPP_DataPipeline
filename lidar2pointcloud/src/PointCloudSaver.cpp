
#include "PointCloudSaver.hpp"
#include "RappPointCloud.hpp"
#include "PointCloud.hpp"

#include "Constants.hpp"

#include "FieldUtils.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>


namespace
{
    void save_flatten_to_pointcloud(const cRappPointCloud& in, cPointCloud& out)
    {
        assert(in.size() == out.size());

        double x_min_m = in.minX_mm() * nConstants::MM_TO_M;
        double x_max_m = in.maxX_mm() * nConstants::MM_TO_M;
        double y_min_m = in.minY_mm() * nConstants::MM_TO_M;
        double y_max_m = in.maxY_mm() * nConstants::MM_TO_M;
        double z_min_m = in.minZ_mm() * nConstants::MM_TO_M;
        double z_max_m = in.maxZ_mm() * nConstants::MM_TO_M;

        out.setExtents(x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m);

        //        std::cout << "Minimum z is " << z_min_m << std::endl;

        auto n = out.size();
        for (std::size_t i = 0; i < n; ++i)
        {
            auto& p1 = out[i];
            auto p2 = in[i];

            p1.X_m = p2.x_mm * nConstants::MM_TO_M;
            p1.Y_m = p2.y_mm * nConstants::MM_TO_M;
            p1.Z_m = p2.z_mm * nConstants::MM_TO_M;

            p1.range_mm     = p2.range_mm;
            p1.signal       = p2.signal;
            p1.reflectivity = p2.reflectivity;
            p1.nir          = p2.nir;
        }
    }
}


cFlattenPointCloudSaver::cFlattenPointCloudSaver(const cRappPointCloud& pointCloud)
    : mProcessingInfoSerializer(1024), mPointCloudSerializer(sizeof(pointcloud::sCloudPoint_t) * mPointCloud.size())
{
    mPointCloud = pointCloud;
    shiftPointCloudToAGL(mPointCloud);
    mPointCloud.recomputeBounds();
}

cFlattenPointCloudSaver::~cFlattenPointCloudSaver()
{
	mFileWriter.close();
	mFileReader.close();
}

void cFlattenPointCloudSaver::setInputFile(const std::string& in)
{
    mInputFile = in;
}

void cFlattenPointCloudSaver::setOutputFile(const std::string& out)
{
    mOutputFile = out;
}


void cFlattenPointCloudSaver::close()
{
    mFileWriter.close();
    mFileReader.close();
}

bool cFlattenPointCloudSaver::open()
{
    if (std::filesystem::exists(mOutputFile))
    {
        return false;
    }

    mFileWriter.open(mOutputFile.string());
    mFileReader.open(mInputFile.string());

    mFileSize = mFileReader.file_size();

    return mFileWriter.isOpen() && mFileReader.isOpen();
}

void cFlattenPointCloudSaver::abort()
{
    mAbort = true;
}

void cFlattenPointCloudSaver::run()
{
    mAbort = false;

    if (!open())
    {
        QString msg = tr("No file is open for reading.");
        emit onError(msg);

        emit savingingComplete();
        return;
    }

    mProcessingInfoSerializer.attach(&mFileWriter);
    mPointCloudSerializer.attach(&mFileWriter);

    mFileReader.attach(static_cast<cOusterParser*>(this));

    mFileReader.registerCallback([this](const cBlockID& id) { this->processBlock(id); });
    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len) { this->processBlock(id, buf, len); });

    mProcessingInfoSerializer.write("FlattenPointCloud", processing_info::ePROCESSING_TYPE::FLAT_POINT_CLOUD_GENERATION);

    {
        cPointCloud point_cloud;
        point_cloud.resize(mPointCloud.size());
        save_flatten_to_pointcloud(mPointCloud, point_cloud);
        mPointCloudSerializer.write(point_cloud);
    }

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail() || mAbort)
            {
                close();

                emit savingingComplete();
                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            emit progressUpdated(static_cast<int>(file_pos));
        }

    }
    catch (const bdf::stream_error& e)
    {
        QString msg = "Stream Error: ";
        msg += e.what();

        emit onError(msg);
    }
    catch (const bdf::crc_error& e)
    {
        QString msg = "CRC Error: ";
        msg += e.what();

        emit onError(msg);
    }
    catch (const bdf::unexpected_eof& e)
    {
        QString msg = "Unexpected EOF: ";
        msg += e.what();

        emit onError(msg);
    }
    catch (const std::exception& e)
    {
        QString msg = "Unknown Exception: ";
        msg += e.what();

        emit onError(msg);
    }

    close();

    emit savingingComplete();
}



/** Experiment Info **/
/*
void cFlattenPointCloudSaver::onBeginHeader()
{
    mExperimentSerializer.writeBeginHeader();
}

void cFlattenPointCloudSaver::onEndOfHeader()
{
    mExperimentSerializer.writeEndOfHeader();
}

void cFlattenPointCloudSaver::onBeginFooter()
{
    mExperimentSerializer.writeBeginFooter();
}

void cFlattenPointCloudSaver::onEndOfFooter()
{
    mExperimentSerializer.writeEndOfFooter();
}

void cFlattenPointCloudSaver::onTitle(const std::string& title)
{
    mExperimentSerializer.writeTitle(title);
}

void cFlattenPointCloudSaver::onPrincipalInvestigator(const std::string& investigator)
{
    mExperimentSerializer.writePrincipalInvestigator(investigator);
}

void cFlattenPointCloudSaver::onBeginResearcherList()
{
    mExperimentSerializer.writeBeginResearchers();
}

void cFlattenPointCloudSaver::onEndOfResearcherList()
{
    mExperimentSerializer.writeEndOfResearchers();
}

void cFlattenPointCloudSaver::onResearcher(const std::string& researcher)
{
    mExperimentSerializer.writeResearcher(researcher);
}

void cFlattenPointCloudSaver::onSpecies(const std::string& species)
{
    mExperimentSerializer.writeSpecies(species);
}

void cFlattenPointCloudSaver::onCultivar(const std::string& cultivar)
{
    mExperimentSerializer.writeCultivar(cultivar);
}

void cFlattenPointCloudSaver::onExperimentDoc(const std::string& doc)
{
    mExperimentSerializer.writeExperimentDoc(doc);
}

void cFlattenPointCloudSaver::onPermitInfo(const std::string& permit)
{
    mExperimentSerializer.writePermitInfo(permit);
}

void cFlattenPointCloudSaver::onBeginTreatmentList()
{
    mExperimentSerializer.writeBeginTreatments();
}

void cFlattenPointCloudSaver::onEndOfTreatmentList()
{
    mExperimentSerializer.writeEndOfTreatments();
}

void cFlattenPointCloudSaver::onTreatment(const std::string& treatment)
{
    mExperimentSerializer.writeTreatment(treatment);
}

void cFlattenPointCloudSaver::onConstructName(const std::string& name)
{
    mExperimentSerializer.writeConstructName(name);
}

void cFlattenPointCloudSaver::onBeginEventNumberList()
{
    mExperimentSerializer.writeBeginEventNumbers();
}

void cFlattenPointCloudSaver::onEndOfEventNumberList()
{
    mExperimentSerializer.writeEndOfEventNumbers();
}

void cFlattenPointCloudSaver::onEventNumber(const std::string& event)
{
    mExperimentSerializer.writeEventNumber(event);
}

void cFlattenPointCloudSaver::onFieldDesign(const std::string& design)
{
    mExperimentSerializer.writeFieldDesign(design);
}

void cFlattenPointCloudSaver::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
    mExperimentSerializer.writePlantingDate(year, month, day, doy);
}

void cFlattenPointCloudSaver::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy)
{
    mExperimentSerializer.writeHarvestDate(year, month, day, doy);
}

void cFlattenPointCloudSaver::onBeginCommentList()
{
    mExperimentSerializer.writeBeginComments();
}

void cFlattenPointCloudSaver::onEndOfCommentList()
{
    mExperimentSerializer.writeEndOfComments();
}

void cFlattenPointCloudSaver::onComment(const std::string& comments)
{
    mExperimentSerializer.writeComment(comments);
}

void cFlattenPointCloudSaver::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day)
{
    mExperimentSerializer.writeFileDate(year, month, day);
}

void cFlattenPointCloudSaver::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds)
{
    mExperimentSerializer.writeFileTime(hour, minute, seconds);
}

void cFlattenPointCloudSaver::onDayOfYear(std::uint16_t day_of_year)
{
    mExperimentSerializer.writeDayOfYear(day_of_year);
}

void cFlattenPointCloudSaver::onBeginSensorList()
{
    mExperimentSerializer.writeBeginSensorList();
}

void cFlattenPointCloudSaver::onEndOfSensorList()
{
    mExperimentSerializer.writeEndOfSensorList();
}

void cFlattenPointCloudSaver::onSensorBlockInfo(uint16_t class_id, const std::string& name)
{
    mExperimentSerializer.writeSensorBlockInfo(class_id, name);
}

void cFlattenPointCloudSaver::onStartTime(sExperimentTime_t time)
{
    mExperimentSerializer.startTime(time.year, time.month, time.day, time.hour, time.minutes, time.seconds);
}

void cFlattenPointCloudSaver::onEndTime(sExperimentTime_t time)
{
    mExperimentSerializer.endTime(time.year, time.month, time.day, time.hour, time.minutes, time.seconds);
}

void cFlattenPointCloudSaver::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    mExperimentSerializer.startRecordingTimestamp(timestamp_ns);
}

void cFlattenPointCloudSaver::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mExperimentSerializer.endRecordingTimestamp(timestamp_ns);
}

void cFlattenPointCloudSaver::onHeartbeatTimestamp(uint64_t timestamp_ns)
{
    mExperimentSerializer.heartbeatTimestamp(timestamp_ns);
}
*/

/** Ouster Info **/
void cFlattenPointCloudSaver::onConfigParam(ouster::config_param_2_t config_param) {}
void cFlattenPointCloudSaver::onSensorInfo(ouster::sensor_info_2_t sensor_info) {}
void cFlattenPointCloudSaver::onTimestamp(ouster::timestamp_2_t timestamp) {}
void cFlattenPointCloudSaver::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) {}
void cFlattenPointCloudSaver::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) {}
void cFlattenPointCloudSaver::onMultipurposeIo(ouster::multipurpose_io_2_t io) {}
void cFlattenPointCloudSaver::onNmea(ouster::nmea_2_t nmea) {}
void cFlattenPointCloudSaver::onTimeInfo(ouster::time_info_2_t time_info) {}
void cFlattenPointCloudSaver::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) {}
void cFlattenPointCloudSaver::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics) {}
void cFlattenPointCloudSaver::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) {}
void cFlattenPointCloudSaver::onLidarDataFormat(ouster::lidar_data_format_2_t format) {}
void cFlattenPointCloudSaver::onImuData(ouster::imu_data_t data) {}
void cFlattenPointCloudSaver::onLidarData(cOusterLidarData data) {}

void cFlattenPointCloudSaver::processBlock(const cBlockID& id)
{
	mFileWriter.writeBlock(id);
}

void cFlattenPointCloudSaver::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
	mFileWriter.writeBlock(id, buf, len);
}
