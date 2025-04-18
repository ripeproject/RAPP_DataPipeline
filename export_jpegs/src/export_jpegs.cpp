
#include "export_jpegs.hpp"

#include "Constants.hpp"

#include "RappFieldBoundary.hpp"

#include <exiv2/exiv2.hpp>

#include <iostream>

namespace
{
    std::string latToExifString(double lat_deg, bool bRational)
    {
        const char* NS = lat_deg >= 0.0 ? "N" : "S";

        if (lat_deg < 0)
            lat_deg = -lat_deg;
        auto deg = static_cast<int>(lat_deg);
        lat_deg -= deg;
        lat_deg *= 60;
        auto min = static_cast<int>(lat_deg);
        lat_deg -= min;
        lat_deg *= 60;
        auto sec = static_cast<int>(lat_deg);

        char result[200];
        if (bRational)
            snprintf(result, sizeof(result), "%d/1 %d/1 %d/1", deg, min, sec);
        else
            snprintf(result, sizeof(result), "%03d%s%02d'%02d\"%s", deg, "deg", min, sec, NS);

        return result;
    }

    std::string lonToExifString(double lon_deg, bool bRational)
    {
        const char* EW = lon_deg >= 0.0 ? "E" : "W";

        if (lon_deg < 0)
            lon_deg = -lon_deg;
        auto deg = static_cast<int>(lon_deg);
        lon_deg -= deg;
        lon_deg *= 60;
        auto min = static_cast<int>(lon_deg);
        lon_deg -= min;
        lon_deg *= 60;
        auto sec = static_cast<int>(lon_deg);

        char result[200];
        if (bRational)
            snprintf(result, sizeof(result), "%d/1 %d/1 %d/1", deg, min, sec);
        else
            snprintf(result, sizeof(result), "%03d%s%02d'%02d\"%s", deg, "deg", min, sec, EW);

        return result;
    }

    std::string toExifString(double d) 
    {
        char result[200];
        d *= 100;
        snprintf(result, sizeof(result), "%d/100", abs(static_cast<int>(d)));
        return result;
    }

    std::string toExifDateStamp(int YYYY, int MM, int DD)
    {
        char result[200];
        snprintf(result, sizeof(result), "%04d:%02d:%02d", YYYY, MM, DD);
        return result;
    }

    std::string toExifTimeStamp(int HH, int mm, int SS1)
    {
        char result[200];
        snprintf(result, sizeof(result), "%d/1 %d/1 %d/1", HH, mm, SS1);
        return result;
    }

}


cExportJpegs::cExportJpegs() : cAxisCommunicationsParser()
{
}

cExportJpegs::~cExportJpegs()
{
}

void cExportJpegs::setOutputPath(std::filesystem::path out)
{
    mOutputPath = out;
}

void cExportJpegs::setRootFileName(const std::string& filename)
{
    mRootFileName = filename;
}

void cExportJpegs::setPlotFile(std::shared_ptr<cPlotConfigFile>& plot_file)
{
    mPlotConfigData = plot_file;
}


// Experiment Info handlers
void cExportJpegs::onBeginHeader() {}
void cExportJpegs::onEndOfHeader() {}

void cExportJpegs::onBeginFooter() {}
void cExportJpegs::onEndOfFooter() {}

void cExportJpegs::onTitle(const std::string& title)
{
    mTitle = title;

    if (mPlotConfigData)
    {
        auto it = mPlotConfigData->find_by_experiment_name(mTitle);

        if (it != mPlotConfigData->end())
        {
            mPlots = it->data();
        }
    }
}

void cExportJpegs::onPrincipalInvestigator(const std::string& investigator) {}

void cExportJpegs::onBeginResearcherList() {}
void cExportJpegs::onEndOfResearcherList() {}
void cExportJpegs::onResearcher(const std::string& researcher) {}

void cExportJpegs::onSpecies(const std::string& species) {}
void cExportJpegs::onCultivar(const std::string& cultivar) {}
void cExportJpegs::onPermitInfo(const std::string& permit) {}
void cExportJpegs::onExperimentDoc(const std::string& doc) {}

void cExportJpegs::onBeginTreatmentList() {}
void cExportJpegs::onEndOfTreatmentList() {}
void cExportJpegs::onTreatment(const std::string& treatment) {}

void cExportJpegs::onConstructName(const std::string& name) {}

void cExportJpegs::onBeginEventNumberList() {}
void cExportJpegs::onEndOfEventNumberList() {}
void cExportJpegs::onEventNumber(const std::string& event) {}

void cExportJpegs::onFieldDesign(const std::string& design) {}
void cExportJpegs::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) {}
void cExportJpegs::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) {}

void cExportJpegs::onBeginCommentList() {}
void cExportJpegs::onEndOfCommentList() {}
void cExportJpegs::onComment(const std::string& comment) {}

void cExportJpegs::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) {}
void cExportJpegs::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) {}

void cExportJpegs::onDayOfYear(std::uint16_t day_of_year) {}

void cExportJpegs::onBeginSensorList() {}
void cExportJpegs::onEndOfSensorList() {}
void cExportJpegs::onSensorBlockInfo(uint16_t class_id, const std::string& name) {}

void cExportJpegs::onStartTime(sExperimentTime_t time) {}
void cExportJpegs::onEndTime(sExperimentTime_t time) {}

void cExportJpegs::onStartRecordingTimestamp(uint64_t timestamp_ns) {}
void cExportJpegs::onEndRecordingTimestamp(uint64_t timestamp_ns) {}
void cExportJpegs::onHeartbeatTimestamp(uint64_t timestamp_ns) {}

// Axis Commications handlers
void cExportJpegs::onFramesPerSecond(uint8_t device_id, int frames_per_sec) {}
void cExportJpegs::onActiveCameraId(uint8_t device_id, int id) {}

void cExportJpegs::onImageSize(uint8_t device_id, int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void cExportJpegs::onBitmap(uint8_t device_id, const cBitmapBuffer& buffer)
{}

void cExportJpegs::onJPEG(uint8_t device_id, const cJpegBuffer& buffer)
{
}

void cExportJpegs::onMpegFrame(uint8_t device_id, const cMpegFrameBuffer& buffer)
{
    cPlotConfigPlotInfo info;

    for (auto& plot : mPlots)
    {
        if (plot.contains(mX_mm, mY_mm))
        {
            info = plot;
            break;
        }
    }

    if (!mPlots.empty() && info.empty())
        return;

    std::filesystem::path filename = mOutputPath / mRootFileName;

    std::string ext;
    if (info.empty())
    {
        ext = std::to_string(mFrameCount);
    }
    else
    {
        ext = "plot_" + std::to_string(info.getPlotNumber());

        ext += "_";
        ext += std::to_string(mFrameCount);
    }
        
    ext += ".";

    ext += "jpeg";

    filename.replace_extension(ext);

    if (mPositionValid)
    {
        using namespace Exiv2;

        auto image = ImageFactory::open(reinterpret_cast<const byte*>(buffer.data()), buffer.size());

        if (image)
        {
            image->readMetadata();
            Exiv2::ExifData& exifData = image->exifData();

            exifData["Exif.GPSInfo.GPSProcessingMethod"] = "charset=Ascii HYBRID-FIX";
            exifData["Exif.GPSInfo.GPSVersionID"] = "2 2 0 0";
            exifData["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";

            exifData["Exif.GPSInfo.GPSLatitude"] = latToExifString(mLat_deg, true);
            exifData["Exif.GPSInfo.GPSLongitude"] = lonToExifString(mLon_deg, true);
            exifData["Exif.GPSInfo.GPSAltitude"] = toExifString(mElevation_m);

            exifData["Exif.GPSInfo.GPSAltitudeRef"] = mElevation_m < 0.0 ? "1" : "0";
            exifData["Exif.GPSInfo.GPSLatitudeRef"] = mLat_deg > 0 ? "N" : "S";
            exifData["Exif.GPSInfo.GPSLongitudeRef"] = mLon_deg > 0 ? "E" : "W";

            if (mDateTimeValid)
            {
                exifData["Exif.GPSInfo.GPSDateStamp"] = toExifDateStamp(mUtcYear, mUtcMonth, mUtcDay);
                exifData["Exif.GPSInfo.GPSTimeStamp"] = toExifTimeStamp(mUtcHour, mUtcMinute, mUtcSecond);
            }
            exifData["Exif.Image.GPSTag"] = 4908;

            //            printf("%s %s % 2d\n", path.c_str(), pPos->toString().c_str(), pPos->delta());

            image->writeMetadata();

            Exiv2::FileIo file(filename.string());

            file.open("wb");
            file.write(image->io());
            file.close();
        }
    }
    else
    {
        std::filebuf binary_buffer;
        binary_buffer.open(filename, std::ios::out | std::ios::binary);
        std::ostream outstream_binary(&binary_buffer);
        if (outstream_binary.fail())
            throw std::runtime_error("failed to open " + filename.string());

        outstream_binary.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        binary_buffer.close();
    }

    ++mFrameCount;
}

void cExportJpegs::writeJpeg(std::filesystem::path filename)
{
}

// Spidercam handlers
void cExportJpegs::onPosition(spidercam::sPosition_1_t position)
{
    mX_mm = position.X_mm;
    mY_mm = position.Y_mm;
    mZ_mm = position.Z_mm;
}

// GPS Handlers - Used to GeoTag images
void cExportJpegs::onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_1_t pos) {}
void cExportJpegs::onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_2_t pos) {}

void cExportJpegs::onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_1_t pos)
{
    mPositionValid = pos.dataValid;
    mLat_deg = pos.Lat_rad * nConstants::RAD_TO_DEG;
    mLon_deg = pos.Lon_rad * nConstants::RAD_TO_DEG;
    mHeight_m = pos.Alt_m;
    mUndulation_m = 0;
    mElevation_m = mHeight_m + mUndulation_m;
}

void cExportJpegs::onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_2_t pos)
{
    mPositionValid = pos.dataValid;
    mLat_deg = pos.Lat_rad * nConstants::RAD_TO_DEG;
    mLon_deg = pos.Lon_rad * nConstants::RAD_TO_DEG;
    mHeight_m = pos.Height_m;
    mUndulation_m = pos.Undulation_m;
    mElevation_m = mHeight_m - mUndulation_m;

    auto point1 = rfb::fromGPS(pos.Lat_rad, pos.Lon_rad, mElevation_m);
    auto point2 = rfb::fromGPS(pos.Lat_rad, pos.Lon_rad, mHeight_m);
}

void cExportJpegs::onPosCovGeodetic(uint8_t device_id, ssnx::gps::PosCovGeodetic_1_t cov) {}
void cExportJpegs::onVelCovGeodetic(uint8_t device_id, ssnx::gps::VelCovGeodetic_1_t cov) {}
void cExportJpegs::onDOP(uint8_t device_id, ssnx::gps::DOP_1_t dop) {}
void cExportJpegs::onPVT_Residuals(uint8_t device_id, ssnx::gps::PVT_Residuals_1_t residuals) {}
void cExportJpegs::onRAIMStatistics(uint8_t device_id, ssnx::gps::RAIMStatistics_1_t raim) {}
void cExportJpegs::onPOS_Local(uint8_t device_id, ssnx::gps::POS_Local_1_t pos) {}
void cExportJpegs::onPOS_Projected(uint8_t device_id, ssnx::gps::POS_Projected_1_t pos) {}

void cExportJpegs::onReceiverTime(uint8_t device_id, ssnx::gps::ReceiverTime_1_t time)
{
    mDateTimeValid = time.dataValid;
    mUtcYear = 2000 + time.utcYear;
    mUtcMonth = time.utcMonth;
    mUtcDay = time.utcDay;
    mUtcHour = time.utcHour;
    mUtcMinute = time.utcMinute;
    mUtcSecond = time.utcSecond;
    mDeltaLS = time.deltaLS;
}

void cExportJpegs::onRtcmDatum(uint8_t device_id, ssnx::gps::RtcmDatum_1_t datum) {}

void cExportJpegs::onReferencePoint(uint8_t device_id, double avgLat_rad, double avgLng_rad, double avgHeight_m,
    double stdLat_rad, double stdLng_rad, double stdHeight_m, bool heightComputed)
{ }

