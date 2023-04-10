
#include "LidarData2CeresConverter.hpp"
#include "bdf_v1/BlockDataFileExceptions.hpp"

#include <iostream>


extern void console_message(const std::string& msg);


cLidarData2CeresConverter::cLidarData2CeresConverter()
{
    mOusterSerializer.setVersion(2, 3);
    mOusterSerializer.setBufferCapacity(256*1024*1024);

    mExperimentSerializer.setVersion(1, 0);
    mExperimentSerializer.setBufferCapacity(2048);
}

cLidarData2CeresConverter::~cLidarData2CeresConverter()
{
    mFileWriter.close();
    mFileReader.close();
}

bool cLidarData2CeresConverter::open(std::filesystem::directory_entry in,
    std::filesystem::path out)
{
    mInputFile = in;
    mOutputFile = out;

    if (std::filesystem::exists(mOutputFile))
    {
        return false;
    }

    mFileWriter.open(mOutputFile.string());
    mFileReader.open(mInputFile.string());

    return mFileReader.isOpen() && mFileWriter.isOpen();
}

void cLidarData2CeresConverter::process_file(std::filesystem::directory_entry in,
    std::filesystem::path out)
{
    if (open(in, out))
    {
        std::string msg = "Processing ";
        msg += in.path().string();
        msg += "...";
        console_message(msg);

        run();
    }
}

void cLidarData2CeresConverter::run()
{
    if (!mFileReader.isOpen())
    {
        throw std::logic_error("No file is open for reading.");
    }

    mFileReader.registerCallback([this](const v1::cBlockID& id) { this->processBlock(id); });

    mFileReader.registerCallback([this](const v1::cBlockID& id, const std::byte* buf, std::size_t len) { this->processBlock(id, buf, len); });

    mFileReader.attach(static_cast<v1::cOusterVerificationParser*>(this));
    mFileReader.attach(static_cast<v1::cPvtVerificationParser*>(this));

    mOusterSerializer.attach(&mFileWriter);
    mExperimentSerializer.attach(&mFileWriter);

    mExperimentSerializer.writeBeginHeader();
    mExperimentSerializer.writeEndOfHeader();

    try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                mFileWriter.close();
                return;
            }

            mFileReader.processBlock();
        }

        mExperimentSerializer.writeBeginFooter();
        mExperimentSerializer.writeEndOfFooter();
    }
    catch (const v1::bdf::stream_error& e)
    {
        std::string msg = "Stream Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const v1::bdf::crc_error& e)
    {
        std::string msg = "CRC Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const std::exception& e)
    {
        std::string msg = "Unknown Exception: ";
        msg += e.what();
        console_message(msg);
    }
}

void cLidarData2CeresConverter::onConfigParam(::ouster::config_param_2_t config_param)
{
    mOusterSerializer.write(config_param);
}

void cLidarData2CeresConverter::onSensorInfo(::ouster::sensor_info_2_t sensor_info)
{
    mOusterSerializer.write(sensor_info);
}

void cLidarData2CeresConverter::onTimestamp(::ouster::timestamp_2_t timestamp)
{
    mOusterSerializer.write(timestamp);
}

void cLidarData2CeresConverter::onSyncPulseIn(::ouster::sync_pulse_in_2_t pulse_info)
{
    mOusterSerializer.write(pulse_info);
}

void cLidarData2CeresConverter::onSyncPulseOut(::ouster::sync_pulse_out_2_t pulse_info)
{
    mOusterSerializer.write(pulse_info);
}

void cLidarData2CeresConverter::onMultipurposeIo(::ouster::multipurpose_io_2_t io)
{
    mOusterSerializer.write(io);
}

void cLidarData2CeresConverter::onNmea(::ouster::nmea_2_t nmea)
{
    mOusterSerializer.write(nmea);
}

void cLidarData2CeresConverter::onTimeInfo(::ouster::time_info_2_t time_info)
{
    mOusterSerializer.write(time_info);
}

void cLidarData2CeresConverter::onBeamIntrinsics(::ouster::beam_intrinsics_2_t intrinsics)
{
    mOusterSerializer.write(intrinsics);
}

void cLidarData2CeresConverter::onImuIntrinsics(::ouster::imu_intrinsics_2_t intrinsics)
{
    mOusterSerializer.write(intrinsics);
}

void cLidarData2CeresConverter::onLidarIntrinsics(::ouster::lidar_intrinsics_2_t intrinsics)
{
    mOusterSerializer.write(intrinsics);
}

void cLidarData2CeresConverter::onLidarDataFormat(::ouster::lidar_data_format_2_t format)
{
    mOusterSerializer.write(format);
}

void cLidarData2CeresConverter::onImuData(::ouster::imu_data_t data)
{
    mOusterSerializer.write(data);
}

void cLidarData2CeresConverter::onLidarData(cOusterLidarData data)
{
    mOusterSerializer.write(data.frame_id(), data);
}

void cLidarData2CeresConverter::onPositionUnits(v1::pvt::ePOSTION_UNITS  positionUnit)
{}

void cLidarData2CeresConverter::onVelocityUnits(v1::pvt::eVELOCITY_UNITS velocityUnit)
{}

void cLidarData2CeresConverter::onTimeUnits(v1::pvt::eTIME_UNITS timeUnit)
{
    using namespace v1::pvt;

    switch (timeUnit)
    {
    case eTIME_UNITS::SECONDS:
        mTimeScaler = 1'000'000'000.0;
        break;
    case eTIME_UNITS::MICROSECONDS:
        mTimeScaler = 1'000'000.0;
        break;
    case eTIME_UNITS::MILLISECONDS:
        mTimeScaler = 1'000.0;
        break;
    case eTIME_UNITS::NANOSECONDS:
        mTimeScaler = 1.0;
        break;
    }
}

void cLidarData2CeresConverter::onPosition1D(double x)
{}

void cLidarData2CeresConverter::onPosition2D(double x, double y)
{}

void cLidarData2CeresConverter::onPosition3D(double x, double y, double z)
{}

void cLidarData2CeresConverter::onVelocity1D(double Vx)
{}

void cLidarData2CeresConverter::onVelocity2D(double Vx, double Vy)
{}

void cLidarData2CeresConverter::onVelocity3D(double Vx, double Vy, double Vz)
{}

void cLidarData2CeresConverter::onTimestamp(std::uint64_t timeStamp)
{
    std::uint64_t timestamp_ns = static_cast<std::uint64_t>(timeStamp * mTimeScaler);

    if (timestamp_ns > 0)
        mExperimentSerializer.heartbeatTimestamp(timestamp_ns);
}


void cLidarData2CeresConverter::processBlock(const v1::cBlockID& id)
{
    cBlockID new_id(id.classID(), id.majorVersion(), id.minorVersion());
    new_id.dataID(id.dataID());

    mFileWriter.writeBlock(new_id);
}

void cLidarData2CeresConverter::processBlock(const v1::cBlockID& id, const std::byte* buf, std::size_t len)
{
    cBlockID new_id(id.classID(), id.majorVersion(), id.minorVersion());
    new_id.dataID(id.dataID());

    mFileWriter.writeBlock(new_id, buf, len);
}

