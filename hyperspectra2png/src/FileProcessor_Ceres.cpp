
#include "FileProcessor_Ceres.hpp"
#include "HySpexVNIR3000N_2_PNG.hpp"
#include "HySpexSWIR384_2_PNG.hpp"

#include <cbdf/BlockDataFileExceptions.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <numbers>


extern void console_message(const std::string& msg);
extern void new_file_progress(const int id, std::string filename);
extern void update_file_progress(const int id, const int progress_pct);


cFileProcessor_Ceres::cFileProcessor_Ceres(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    cFileProcessor(id, in, out)
{}

cFileProcessor_Ceres::~cFileProcessor_Ceres()
{
    mFileReader.close();
}

void cFileProcessor_Ceres::setVnirRgb(float red_nm, float green_nm, float blue_nm)
{
    mVnirConverter->setRgbWavelengths_nm(red_nm, green_nm, blue_nm);
}

void cFileProcessor_Ceres::setSwirRgb(float red_nm, float green_nm, float blue_nm)
{
    mSwirConverter->setRgbWavelengths_nm(red_nm, green_nm, blue_nm);
}

bool cFileProcessor_Ceres::open(std::filesystem::path out)
{
    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    mVnirConverter->setOutputPath(outFile);
    mSwirConverter->setOutputPath(outFile);

    mFileReader.open(mInputFile.string());
    mFileSize = mFileReader.file_size();

    return mFileReader.isOpen();
}

void cFileProcessor_Ceres::process_file()
{
    if (open(mOutputFile))
    {
        new_file_progress(mID, mInputFile.string());

        run();
    }
}

void cFileProcessor_Ceres::run()
{
	if (!mFileReader.isOpen())
	{
        throw std::logic_error("No file is open for reading.");
	}

    mFileReader.attach(static_cast<cExperimentParser*>(this));
    mFileReader.attach(static_cast<cSpidercamParser*>(this));

    cHySpexVNIR3000N_2_Png* pVnir = mVnirConverter.get();
	mFileReader.attach(static_cast<cHySpexVNIR_3000N_Parser*>(pVnir));

    cHySpexSWIR384_2_Png* pSwir = mSwirConverter.get();
    mFileReader.attach(static_cast<cHySpexSWIR_384_Parser*>(pSwir));

	try
    {
        while (!mFileReader.eof())
        {
            if (mFileReader.fail())
            {
                mFileReader.close();
                return;
            }

            mFileReader.processBlock();

            auto file_pos = static_cast<double>(mFileReader.filePosition());
            file_pos = 100.0 * (file_pos / mFileSize);
            update_file_progress(mID, static_cast<int>(file_pos));
        }
    }
    catch (const bdf::stream_error& e)
    {
        std::string msg = "Stream Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::crc_error& e)
    {
        std::string msg = "CRC Error: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const bdf::unexpected_eof& e)
    {
        std::string msg = "Unexpected EOF: ";
        msg += e.what();
        console_message(msg);
    }
    catch (const std::exception& e)
    {
        std::string msg = "Unknown Exception: ";
        msg += e.what();
        console_message(msg);
    }

    update_file_progress(mID, 100);
}


void cFileProcessor_Ceres::onBeginHeader() {}
void cFileProcessor_Ceres::onEndOfHeader() {}

void cFileProcessor_Ceres::onBeginFooter() {}
void cFileProcessor_Ceres::onEndOfFooter() {}

void cFileProcessor_Ceres::onExperimentTitle(const std::string& title) {}
void cFileProcessor_Ceres::onMeasurementTitle(const std::string& title) {}
void cFileProcessor_Ceres::onPrincipalInvestigator(const std::string& investigator) {}

void cFileProcessor_Ceres::onBeginResearcherList() {}
void cFileProcessor_Ceres::onEndOfResearcherList() {}
void cFileProcessor_Ceres::onResearcher(const std::string& researcher) {}

void cFileProcessor_Ceres::onSpecies(const std::string& species) {}
void cFileProcessor_Ceres::onCultivar(const std::string& cultivar) {}
void cFileProcessor_Ceres::onTrialInfo(const std::string& trial) {}
void cFileProcessor_Ceres::onPermitInfo(const std::string& permit) {}
void cFileProcessor_Ceres::onPermitInfo(const std::string& authorization, const std::string& permit) {}
void cFileProcessor_Ceres::onExperimentDoc(const std::string& doc) {}

void cFileProcessor_Ceres::onBeginTreatmentList() {}
void cFileProcessor_Ceres::onEndOfTreatmentList() {}
void cFileProcessor_Ceres::onTreatment(const std::string& treatment) {}

void cFileProcessor_Ceres::onConstructName(const std::string& name) {}

void cFileProcessor_Ceres::onBeginEventNumberList() {}
void cFileProcessor_Ceres::onEndOfEventNumberList() {}
void cFileProcessor_Ceres::onEventNumber(const std::string& event) {}

void cFileProcessor_Ceres::onFieldDesign(const std::string& design) {}
void cFileProcessor_Ceres::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) {}
void cFileProcessor_Ceres::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) {}

void cFileProcessor_Ceres::onBeginCommentList() {}
void cFileProcessor_Ceres::onEndOfCommentList() {}
void cFileProcessor_Ceres::onComment(const std::string& comment) {}

void cFileProcessor_Ceres::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) {}
void cFileProcessor_Ceres::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) {}

void cFileProcessor_Ceres::onDayOfYear(std::uint16_t day_of_year) {}

void cFileProcessor_Ceres::onBeginSensorList() {}
void cFileProcessor_Ceres::onEndOfSensorList() {}
void cFileProcessor_Ceres::onSensorBlockInfo(uint16_t class_id, const std::string& name) {}
void cFileProcessor_Ceres::onSensorBlockInfo(uint16_t class_id, const std::string& name,
    const std::string& instance, const std::string& manufacturer, const std::string& model,
    const std::string& serial_number, uint8_t device_id) {};

void cFileProcessor_Ceres::onStartTime(sExperimentTime_t time) {}
void cFileProcessor_Ceres::onEndTime(sExperimentTime_t time) {}

void cFileProcessor_Ceres::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    mVnirConverter->onStartRecordingTimestamp(timestamp_ns);
    mSwirConverter->onStartRecordingTimestamp(timestamp_ns);
}

void cFileProcessor_Ceres::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mVnirConverter->onEndRecordingTimestamp(timestamp_ns);
    mSwirConverter->onEndRecordingTimestamp(timestamp_ns);
}

void cFileProcessor_Ceres::onHeartbeatTimestamp(uint64_t timestamp_ns) {}


void cFileProcessor_Ceres::onPosition(spidercam::sPosition_1_t pos)
{
    mVnirConverter->onPosition(pos.X_mm, pos.Y_mm, pos.Z_mm, pos.speed_mmps);
    mSwirConverter->onPosition(pos.X_mm, pos.Y_mm, pos.Z_mm, pos.speed_mmps);
}

void cFileProcessor_Ceres::onStartPosition(spidercam::sPosition_1_t position)
{}

void cFileProcessor_Ceres::onEndPosition(spidercam::sPosition_1_t position)
{}
