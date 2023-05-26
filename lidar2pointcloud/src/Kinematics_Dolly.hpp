
#pragma once

#include "Kinematics.hpp"

#include <cbdf/ExperimentParser.hpp>
#include <cbdf/SpidercamParser.hpp>

#include <vector>


class cKinematics_Dolly : public cKinematics, public cSpidercamParser, public cExperimentParser
{
public:
	cKinematics_Dolly();

	void writeHeader(cPointCloudSerializer& serializer) override;

	/*
	 * Return true, we need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;
	void telemetryPassComplete() override;

	/*
	 * Attach/detach any parsers to the data file.
	 */
	void attachKinematicParsers(cBlockDataFileReader& file) override;
	void detachKinematicParsers(cBlockDataFileReader& file) override;

	void attachTransformParsers(cBlockDataFileReader& file) override;
	void detachTransformParsers(cBlockDataFileReader& file) override;

	/*
	 * Transform the point cloud coordinated based on kinematic model
	 */
	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override;

protected:
	/** Spidercam Parser **/
	void onPosition(spidercam::sPosition_1_t position) override;

	/** Experiment Parser **/
	void onBeginHeader() override {};
	void onEndOfHeader() override {};

	void onBeginFooter() override {};
	void onEndOfFooter() override {};

	void onTitle(const std::string& title) override {};
	void onPrincipalInvestigator(const std::string& investigator) override {};
	void onResearcher(const std::string& researcher) override {};
	void onSpecies(const std::string& species) override {};
	void onCultivar(const std::string& cultivar) override {};
	void onExperimentDoc(const std::string& doc) override {};

	void onBeginTreatmentList() override {};
	void onEndOfTreatmentList() override {};
	void onTreatment(const std::string& name) override {};

	void onConstructName(const std::string& name) override {};
	void onEventNumber(const std::string& event) override {};
	void onFieldDesign(const std::string& design) override {};
	void onPlantingDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override {};
	void onHarvestDate(std::uint16_t year, std::uint8_t month, std::uint8_t day, std::uint16_t doy) override {};

	void onBeginCommentList() override {};
	void onEndOfCommentList() override {};
	void onComment(const std::string& comments) override {};

	void onFileDate(std::uint16_t year, std::uint8_t month, std::uint8_t day) override {};
	void onFileTime(std::uint8_t hour, std::uint8_t minute, std::uint8_t seconds) override {};

	void onDayOfYear(std::uint16_t day_of_year) override {};

	void onBeginSensorList() override {};
	void onEndOfSensorList() override {};
	void onSensorBlockInfo(uint16_t class_id, const std::string& name) override {};

	void onStartTime(sExperimentTime_t time) override {};
	void onEndTime(sExperimentTime_t time) override {};

	void onStartRecordingTimestamp(uint64_t timestamp_ns) override;
	void onEndRecordingTimestamp(uint64_t timestamp_ns) override;
	void onHeartbeatTimestamp(uint64_t timestamp_ns) override {};

private:

	bool mStartPath = false;
	bool mDataActive = false;
	uint64_t mStartTimestamp = 0;

	double mSouth_Offset_m  = 0.0;
	double mEast_Offset_m   = 0.0;
	double mHeight_Offset_m = 0.0;

	double mSouth_m  = 0.0;
	double mEast_m   = 0.0;
	double mHeight_m = 0.0;

	/*
	 * The dolly uses a south/east/up coordinate system
	 * x axis is positive heading south
	 * y axis is positive heading east
	 * z axis is positive going up
	 */
	struct sDollyInfo_t
	{
		double timestamp_us = 0.0;
		double x_m = 0.0;
		double y_m = 0.0;
		double z_m = 0.0;
		double vx_mps = 0.0;
		double vy_mps = 0.0;
		double vz_mps = 0.0;
	};

	std::vector<sDollyInfo_t> mDollyInfo;
	std::vector<sDollyInfo_t>::size_type mDollyInfoIndex = 0;
	std::vector<sDollyInfo_t>::size_type mDollyInfoMax = 0;
};


inline bool cKinematics_Dolly::requiresTelemetryPass() { return true; }

