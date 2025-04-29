
#include "FileProcessor.hpp"
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


cFileProcessor::cFileProcessor(int id, std::filesystem::directory_entry in,
                                std::filesystem::path out) 
:
    mID(id), mVnirConverter{new cHySpexVNIR3000N_2_Png()}, mSwirConverter {new cHySpexSWIR384_2_Png()
}
{
    mInputFile = in;
    mOutputFile = out;
}

cFileProcessor::~cFileProcessor()
{
    mFileReader.close();
}

void cFileProcessor::setVnirRgb(float red_nm, float green_nm, float blue_nm)
{
    mVnirConverter->setRgbWavelengths_nm(red_nm, green_nm, blue_nm);
}

void cFileProcessor::setSwirRgb(float red_nm, float green_nm, float blue_nm)
{
    mSwirConverter->setRgbWavelengths_nm(red_nm, green_nm, blue_nm);
}

bool cFileProcessor::open(std::filesystem::path out)
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

void cFileProcessor::process_file()
{
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
void cFileProcessor::onPermitInfo(const std::string& permit) {}
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

void cFileProcessor::onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) {}
void cFileProcessor::onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) {}

void cFileProcessor::onDayOfYear(std::uint16_t day_of_year) {}

void cFileProcessor::onBeginSensorList() {}
void cFileProcessor::onEndOfSensorList() {}
void cFileProcessor::onSensorBlockInfo(uint16_t class_id, const std::string& name) {}

void cFileProcessor::onStartTime(sExperimentTime_t time) {}
void cFileProcessor::onEndTime(sExperimentTime_t time) {}

void cFileProcessor::onStartRecordingTimestamp(uint64_t timestamp_ns)
{
    mVnirConverter->onStartRecordingTimestamp(timestamp_ns);
    mSwirConverter->onStartRecordingTimestamp(timestamp_ns);
}

void cFileProcessor::onEndRecordingTimestamp(uint64_t timestamp_ns) 
{
    mVnirConverter->onEndRecordingTimestamp(timestamp_ns);
    mSwirConverter->onEndRecordingTimestamp(timestamp_ns);
}

void cFileProcessor::onHeartbeatTimestamp(uint64_t timestamp_ns) {}


void cFileProcessor::onPosition(spidercam::sPosition_1_t pos)
{
    mVnirConverter->onPosition(pos.X_mm, pos.Y_mm, pos.Z_mm, pos.speed_mmps);
    mSwirConverter->onPosition(pos.X_mm, pos.Y_mm, pos.Z_mm, pos.speed_mmps);
}

