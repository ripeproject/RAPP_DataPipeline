
#pragma once

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentParser.hpp>

#include <filesystem>
#include <string>
#include <memory>

// Forward Declarations
class cHySpexVNIR3000N_2_Png;
class cHySpexSWIR384_2_Png;


class cFileProcessor : public cExperimentParser
{
public:
	cFileProcessor(int id, std::filesystem::directory_entry in,
				std::filesystem::path out);
	~cFileProcessor();

	void setVnirRgb(float red_nm, float green_nm, float blue_nm);
	void setSwirRgb(float red_nm, float green_nm, float blue_nm);

	bool open(std::filesystem::path out);

	void process_file();
	void run();

private:
	void onBeginHeader() override;
	void onEndOfHeader() override;

	void onBeginFooter() override;
	void onEndOfFooter() override;

	void onTitle(const std::string& title) override;
	void onPrincipalInvestigator(const std::string& investigator) override;

	void onBeginResearcherList() override;
	void onEndOfResearcherList() override;
	void onResearcher(const std::string& researcher) override;

	void onSpecies(const std::string& species) override;
	void onCultivar(const std::string& cultivar) override;
	void onPermitInfo(const std::string& permit) override;
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

	void onStartTime(sExperimentTime_t time) override;
	void onEndTime(sExperimentTime_t time) override;

	void onStartRecordingTimestamp(uint64_t timestamp_ns) override;
	void onEndRecordingTimestamp(uint64_t timestamp_ns) override;
	void onHeartbeatTimestamp(uint64_t timestamp_ns) override;

private:
	const int mID;

	std::uintmax_t mFileSize;
	cBlockDataFileReader mFileReader;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	std::unique_ptr<cHySpexVNIR3000N_2_Png> mVnirConverter;
	std::unique_ptr<cHySpexSWIR384_2_Png>   mSwirConverter;
};
