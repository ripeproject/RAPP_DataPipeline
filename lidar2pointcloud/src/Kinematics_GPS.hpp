
#pragma once

#include "Kinematics.hpp"

#include <cbdf/SsnxParser.hpp>


class cKinematics_GPS : public cKinematics, public cSsnxParser
{
public:
	cKinematics_GPS();

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

	void attachTransformSerializers(cBlockDataFileWriter& file) override;
	void detachTransformSerializers(cBlockDataFileWriter& file) override;

	/*
	 * Transform the point cloud coordinated based on kinematic model
	 */
	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud) override;

protected:
	void onPVT_Cartesian(ssnx::gps::PVT_Cartesian_1_t pos) override {}
	void onPVT_Cartesian(ssnx::gps::PVT_Cartesian_2_t pos) override {}
	void onPVT_Geodetic(ssnx::gps::PVT_Geodetic_1_t pos) override {}
	void onPVT_Geodetic(ssnx::gps::PVT_Geodetic_2_t pos) override {}
	void onPosCovGeodetic(ssnx::gps::PosCovGeodetic_1_t cov) override {}
	void onVelCovGeodetic(ssnx::gps::VelCovGeodetic_1_t cov) override {}
	void onDOP(ssnx::gps::DOP_1_t dop) override {}
	void onPVT_Residuals(ssnx::gps::PVT_Residuals_1_t residuals) override {}
	void onRAIMStatistics(ssnx::gps::RAIMStatistics_1_t raim) override {}
	void onPOS_Local(ssnx::gps::POS_Local_1_t pos) override {}
	void onPOS_Projected(ssnx::gps::POS_Projected_1_t pos) override {}
	void onReceiverTime(ssnx::gps::ReceiverTime_1_t time) override {}
	void onRtcmDatum(ssnx::gps::RtcmDatum_1_t datum) override {}

private:

};


inline bool cKinematics_GPS::requiresTelemetryPass() { return true; }
inline void cKinematics_GPS::telemetryPassComplete() {};


