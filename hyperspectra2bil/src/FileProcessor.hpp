
#pragma once

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentParser.hpp>
#include <cbdf/SpidercamParser.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cHySpexVNIR3000N_File;
class cHySpexSWIR384_File;

enum class eExportFormat {BIL, BIP, BSQ};
enum class eHeaderFormat {ArcMap, ENVI};

class cFileProcessor : public cExperimentParser, public cSpidercamParser
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void setFormat(eExportFormat file_format, eHeaderFormat header_format);

	void process_file();
	void run();

protected:
	bool open(std::filesystem::path out);

protected:
	void onBeginHeader() override;
	void onEndOfHeader() override;

	void onBeginFooter() override;
	void onEndOfFooter() override;

	void onExperimentTitle(const std::string& title) override;
	void onMeasurementTitle(const std::string& title) override;
	void onPrincipalInvestigator(const std::string& investigator) override;

	void onBeginResearcherList() override;
	void onEndOfResearcherList() override;
	void onResearcher(const std::string& researcher) override;

	void onSpecies(const std::string& species) override;
	void onCultivar(const std::string& cultivar) override;
	void onTrialInfo(const std::string& trial) override;
	void onPermitInfo(const std::string& permit) override;
	void onPermitInfo(const std::string& authorization, const std::string& permit) override;
	void onExperimentDoc(const std::string& doc) override;

	void onBeginTreatmentList() override;
	void onEndOfTreatmentList() override;
	void onTreatment(const std::string& treatment) override;

	void onConstructName(const std::string& name) override;

	void onBeginEventNumberList() override;
	void onEndOfEventNumberList() override;
	void onEventNumber(const std::string& event) override;

	void onFieldDesign(const std::string& design) override;
	void onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;
	void onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override;

	void onBeginCommentList() override;
	void onEndOfCommentList() override;
	void onComment(const std::string& comment) override;

	void onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) override;
	void onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) override;

	void onDayOfYear(std::uint16_t day_of_year) override;

	void onBeginSensorList() override;
	void onEndOfSensorList() override;
	void onSensorBlockInfo(uint16_t class_id, const std::string& name) override;
	void onSensorBlockInfo(uint16_t class_id, const std::string& name, const std::string& instance,
		const std::string& manufacturer, const std::string& model, const std::string& serial_number, uint8_t device_id) override;

	void onStartTime(sExperimentTime_t time) override;
	void onEndTime(sExperimentTime_t time) override;

	void onStartRecordingTimestamp(uint64_t timestamp_ns) override;
	void onEndRecordingTimestamp(uint64_t timestamp_ns) override;
	void onHeartbeatTimestamp(uint64_t timestamp_ns) override;


	void onPosition(spidercam::sPosition_1_t pos) override;
	void onStartPosition(spidercam::sPosition_1_t position) override;
	void onEndPosition(spidercam::sPosition_1_t position) override;

private:
	const int mID;

	std::uintmax_t mFileSize = 0;
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cHySpexVNIR3000N_File> mVnirConverter;
	std::unique_ptr<cHySpexSWIR384_File>   mSwirConverter;
};
