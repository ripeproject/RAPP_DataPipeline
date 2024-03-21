
#include "PointCloudSaver.hpp"
#include "RappPointCloud.hpp"
#include "PointCloud.hpp"

#include "Constants.hpp"

#include "FieldUtils.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>


extern void update_progress(const int id, const int progress_pct);


void to_pointcloud(const cRappPointCloud& in, cPointCloud& out)
{
    assert(in.size() == out.size());

    double x_min_m = in.minX_mm() * nConstants::MM_TO_M;
    double x_max_m = in.maxX_mm() * nConstants::MM_TO_M;
    double y_min_m = in.minY_mm() * nConstants::MM_TO_M;
    double y_max_m = in.maxY_mm() * nConstants::MM_TO_M;
    double z_min_m = in.minZ_mm() * nConstants::MM_TO_M;
    double z_max_m = in.maxZ_mm() * nConstants::MM_TO_M;

    out.setExtents(x_min_m, x_max_m, y_min_m, y_max_m, z_min_m, z_max_m);

    auto n = out.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        auto& p1 = out[i];
        auto p2 = in[i];

        p1.X_m = p2.x_mm * nConstants::MM_TO_M;
        p1.Y_m = p2.y_mm * nConstants::MM_TO_M;
        p1.Z_m = p2.z_mm * nConstants::MM_TO_M;

        p1.range_mm = p2.range_mm;
        p1.signal = p2.signal;
        p1.reflectivity = p2.reflectivity;
        p1.nir = p2.nir;
    }
}



cPointCloudSaver::cPointCloudSaver(int id, const cRappPointCloud& pointCloud)
    : mID(id), mProcessingInfoSerializer(1024), mPointCloudSerializer(sizeof(pointcloud::sCloudPoint_t) * mPointCloud.size())
{
    mPointCloud = pointCloud;
}

cPointCloudSaver::~cPointCloudSaver()
{
	mFileWriter.close();
	mFileReader.close();
}

void cPointCloudSaver::setInputFile(const std::string& in)
{
    mInputFile = in;
}

void cPointCloudSaver::setOutputFile(const std::string& out)
{
    mOutputFile = out;
}


void cPointCloudSaver::close()
{
    mFileWriter.close();
    mFileReader.close();
}

bool cPointCloudSaver::open()
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

bool cPointCloudSaver::save(bool isFlattened)
{
    if (!open())
    {
        return false;
    }

    mProcessingInfoSerializer.attach(&mFileWriter);
    mPointCloudSerializer.attach(&mFileWriter);

    mFileReader.attach(static_cast<cOusterParser*>(this));

    mFileReader.registerCallback([this](const cBlockID& id) { this->processBlock(id); });
    mFileReader.registerCallback([this](const cBlockID& id, const std::byte* buf, std::size_t len) { this->processBlock(id, buf, len); });

    if (isFlattened)
        mProcessingInfoSerializer.write("Lidar2PointCloud", processing_info::ePROCESSING_TYPE::FLAT_POINT_CLOUD_GENERATION);
    else
        mProcessingInfoSerializer.write("Lidar2PointCloud", processing_info::ePROCESSING_TYPE::POINT_CLOUD_GENERATION);

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                break;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_progress(mID, static_cast<int>(file_pos));
        }

        {
            cPointCloud point_cloud;
            point_cloud.resize(mPointCloud.size());
            to_pointcloud(mPointCloud, point_cloud);

            mPointCloudSerializer.writeDimensions(point_cloud.minX_m(), point_cloud.maxX_m(),
                point_cloud.minY_m(), point_cloud.maxY_m(), point_cloud.minZ_m(), point_cloud.maxZ_m());

            mPointCloudSerializer.write(point_cloud);
        }

    }
    catch (const bdf::stream_error& e)
    {
        return false;
    }
    catch (const bdf::crc_error& e)
    {
        return false;
    }
    catch (const bdf::unexpected_eof& e)
    {
        return false;
    }
    catch (const std::exception& e)
    {
        return false;
    }

    close();

    update_progress(mID, 100);

    return true;
}


/** Ouster Info **/
void cPointCloudSaver::onConfigParam(ouster::config_param_2_t config_param) {}
void cPointCloudSaver::onSensorInfo(ouster::sensor_info_2_t sensor_info) {}
void cPointCloudSaver::onTimestamp(ouster::timestamp_2_t timestamp) {}
void cPointCloudSaver::onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) {}
void cPointCloudSaver::onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) {}
void cPointCloudSaver::onMultipurposeIo(ouster::multipurpose_io_2_t io) {}
void cPointCloudSaver::onNmea(ouster::nmea_2_t nmea) {}
void cPointCloudSaver::onTimeInfo(ouster::time_info_2_t time_info) {}
void cPointCloudSaver::onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) {}
void cPointCloudSaver::onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics) {}
void cPointCloudSaver::onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) {}
void cPointCloudSaver::onLidarDataFormat(ouster::lidar_data_format_2_t format) {}
void cPointCloudSaver::onImuData(ouster::imu_data_t data) {}
void cPointCloudSaver::onLidarData(cOusterLidarData data) {}

void cPointCloudSaver::processBlock(const cBlockID& id)
{
	mFileWriter.writeBlock(id);
}

void cPointCloudSaver::processBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
	mFileWriter.writeBlock(id, buf, len);
}


