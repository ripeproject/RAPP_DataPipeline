
#pragma once

#include "Kinematics.hpp"

#include <cstdint>


class cKinematics_Constant : public cKinematics
{
public:
	cKinematics_Constant(double Vx_mmps, double Vy_mmps, double Vz_mmps);

	void writeHeader(cPointCloudSerializer& serializer) override;

	/*
	 * Return false, no need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;
	void telemetryPassComplete() override;

	/*
	 * Attach any parsers to the data file.
 	 */
	void attachKinematicParsers(cBlockDataFileReader& file) override {};
	void detachKinematicParsers(cBlockDataFileReader& file) override {};

	void attachTransformParsers(cBlockDataFileReader& file) override {};
	void detachTransformParsers(cBlockDataFileReader& file) override {};

	void attachTransformSerializers(cBlockDataFileWriter& file) override {};
	void detachTransformSerializers(cBlockDataFileWriter& file) override {};

	/*
	 * Transform the point cloud coordinated based on kinematic model
	 */
	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override;

protected:

private:
	double mX_m = 0.0;
	double mY_m = 0.0;
	double mZ_m = 0.0;

	double mVx_mps = 0.0;
	double mVy_mps = 0.0;
	double mVz_mps = 0.0;

	uint64_t mStartTimestamp_ns = 0;
};


inline bool cKinematics_Constant::requiresTelemetryPass() { return false; }
inline void cKinematics_Constant::telemetryPassComplete() {};


