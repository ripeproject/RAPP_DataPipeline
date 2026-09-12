
#include "FileProcessor.hpp"

#include "TeledyneFLIR_File.hpp"

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


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id)
{
    mInputFile = in;
    mOutputFile = out;

    mFlirConverter = std::make_unique<cTeledyneFlir_File>();
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

bool cFileProcessor::open(std::filesystem::path out)
{
    std::filesystem::path outFile  = out.replace_extension();
    std::filesystem::path testFile = outFile;

    if (std::filesystem::exists(testFile))
    {
        return false;
    }

    mFlirConverter->setOutputPath(outFile);

    mFileReader.open(mInputFile.string());
    mFileSize = mFileReader.file_size();

    return mFileReader.isOpen();
}

void cFileProcessor::process_file()
{
    if (!mFlirConverter)
    {
        throw std::logic_error("File format was not set.");
    }

    if (open(mOutputFile))
    {
        new_file_progress(mID, mInputFile.string());

        run();
    }
}

void cFileProcessor::run()
{
	if (!mFileReader.isOpen())
	{
        throw std::logic_error("No file is open for reading.");
	}

    mFileReader.attach(static_cast<cExperimentParser*>(this));
    mFileReader.attach(static_cast<cSpidercamParser*>(this));

    cTeledyneFlir_File* pFLIR = mFlirConverter.get();
	mFileReader.attach(static_cast<cTeledyneFlirParser*>(pFLIR));

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


void cFileProcessor::onBeginHeader() {}
void cFileProcessor::onEndOfHeader() {}

void cFileProcessor::onBeginFooter() {}
void cFileProcessor::onEndOfFooter() {}

void cFileProcessor::onExperimentTitle(const std::string& title) {}
void cFileProcessor::onMeasurementTitle(const std::string& title) {}
void cFileProcessor::onPrincipalInvestigator(const std::string& investigator) {}

void cFileProcessor::onBeginResearcherList() {}
void cFileProcessor::onEndOfResearcherList() {}
void cFileProcessor::onResearcher(const std::string& researcher) {}

void cFileProcessor::onSpecies(const std::string& species) {}
void cFileProcessor::onCultivar(const std::string& cultivar) {}
void cFileProcessor::onTrialInfo(const std::string& trial) {}
void cFileProcessor::onPermitInfo(const std::string& permit) {}
void cFileProcessor::onPermitInfo(const std::string& authorization, const std::string& permit) {}
void cFileProcessor::onExperimentDoc(const std::string& doc) {}

void cFileProcessor::onBeginTreatmentList() {}
void cFileProcessor::onEndOfTreatmentList() {}
void cFileProcessor::onTreatment(const std::string& treatment) {}

void cFileProcessor::onConstructName(const std::string& name) {}

void cFileProcessor::onBeginEventNumberList() {}
void cFileProcessor::onEndOfEventNumberList() {}
void cFileProcessor::onEventNumber(const std::string& event) {}

void cFileProcessor::onFieldDesign(const std::string& design) {}
void cFileProcessor::onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) {}
void cFileProcessor::onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) {}

void cFileProcessor::onBeginCommentList() {}
void cFileProcessor::onEndOfCommentList() {}
void cFileProcessor::onComment(const std::string& comment) {}

void cFileProcessor::onBeginCustomInfoList() {}
void cFileProcessor::onEndOfCustomInfoList() {}
void cFileProcessor::onCustomInfo(const std::string& tag, const std::string& info) {}

void cFileProcessor::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) {}
void cFileProcessor::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) {}

void cFileProcessor::onDayOfYear(std::uint16_t day_of_year) {}

void cFileProcessor::onBeginSensorList() {}
void cFileProcessor::onEndOfSensorList() {}
void cFileProcessor::onSensorBlockInfo(uint16_t class_id, const std::string& name) {}
void cFileProcessor::onSensorBlockInfo(uint16_t class_id, const std::string& name, const std::string& instance,
    const std::string& manufacturer, const std::string& model, const std::string& serial_number, uint8_t device_id) {}

void cFileProcessor::onStartTime(sExperimentTime_t time) {}
void cFileProcessor::onEndTime(sExperimentTime_t time) {}

void cFileProcessor::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    mFlirConverter->onStartRecordingTimestamp(timestamp_ns);
}

void cFileProcessor::onEndRecordingTimestamp(uint64_t timestamp_ns)
{
    mFlirConverter->onEndRecordingTimestamp(timestamp_ns);
}

void cFileProcessor::onHeartbeatTimestamp(uint64_t timestamp_ns) {}


void cFileProcessor::onPosition(spidercam::sPosition_1_t pos)
{
    mFlirConverter->onPosition(pos.X_mm, pos.Y_mm, pos.Z_mm, pos.speed_mmps);
}

void cFileProcessor::onStartPosition(spidercam::sPosition_1_t position)
{}

void cFileProcessor::onEndPosition(spidercam::sPosition_1_t position)
{}

