
#pragma once

#include "PointCloudInfo.hpp"
#include "ProcessingInfoSerializer.hpp"
#include "PointCloudSerializer.hpp"

#include "RappPointCloud.hpp"

#include <cbdf/BlockDataFile.hpp>
#include <cbdf/ExperimentParser.hpp>
#include <cbdf/ExperimentSerializer.hpp>
#include <cbdf/OusterParser.hpp>

#include <QThread>
#include <QObject>

#include <string>
#include <filesystem>


class cFlattenPointCloudSaver : public QThread, /*public cExperimentParser,*/ public cOusterParser
{
	Q_OBJECT

public:
	cFlattenPointCloudSaver(const cRappPointCloud& pointCloud);
	~cFlattenPointCloudSaver();

	void setInputFile(const std::string& in);
	void setOutputFile(const std::string& out);

	void close();

signals:
	void progressUpdated(int progress);
	void savingingComplete();
	void onError(QString str);

public slots:
	void abort();

protected:
	bool open();
	void run() override;

/** Experiment Info **/
/*
protected:
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
	void onExperimentDoc(const std::string& doc) override;

	void onPermitInfo(const std::string& permit) override;

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
	void onComment(const std::string& comments) override;

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
*/

/** Ouster Info **/
protected:
	void onConfigParam(ouster::config_param_2_t config_param) override;
	void onSensorInfo(ouster::sensor_info_2_t sensor_info) override;
	void onTimestamp(ouster::timestamp_2_t timestamp) override;
	void onSyncPulseIn(ouster::sync_pulse_in_2_t pulse_info) override;
	void onSyncPulseOut(ouster::sync_pulse_out_2_t pulse_info) override;
	void onMultipurposeIo(ouster::multipurpose_io_2_t io) override;
	void onNmea(ouster::nmea_2_t nmea) override;
	void onTimeInfo(ouster::time_info_2_t time_info) override;
	void onBeamIntrinsics(ouster::beam_intrinsics_2_t intrinsics) override;
	void onImuIntrinsics(ouster::imu_intrinsics_2_t intrinsics) override;
	void onLidarIntrinsics(ouster::lidar_intrinsics_2_t intrinsics) override;
	void onLidarDataFormat(ouster::lidar_data_format_2_t format) override;
	void onImuData(ouster::imu_data_t data) override;
	void onLidarData(cOusterLidarData data) override;


private:
	void processBlock(const cBlockID& id);
	void processBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

private:
	bool mAbort = false;

	cRappPointCloud mPointCloud;

	int mID = 0;
	std::uintmax_t mFileSize = 0;

	cBlockDataFileReader mFileReader;
	cBlockDataFileWriter mFileWriter;

	std::filesystem::path mInputFile;
	std::filesystem::path mOutputFile;

	cExperimentSerializer		mExperimentSerializer;
	cProcessingInfoSerializer	mProcessingInfoSerializer;
	cPointCloudSerializer		mPointCloudSerializer;
};

