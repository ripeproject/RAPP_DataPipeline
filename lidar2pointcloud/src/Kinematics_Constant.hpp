
#pragma once

#include "Kinematics.hpp"

#include <cstdint>


class cKinematics_Constant : public cKinematics
{
public:
	cKinematics_Constant(double Vx_mmps, double Vy_mmps, double Vz_mmps);

	/*
	 * Return false, no need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;

	/*
	 * Attach any parsers to the data file.
 	 */
	void attachParsers(cBlockDataFileReader& file) override {};
	void detachParsers(cBlockDataFileReader& file) override {};

	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override;

protected:

private:
	double mEast_m = 0.0;
	double mY_m = 0.0;
	double mZ_m = 0.0;

	double mEastSpeed_mps = 0.0;
	double mVy_mps = 0.0;
	double mVz_mps = 0.0;

	uint64_t mStartTimestamp_ns = 0;
};


inline bool cKinematics_Constant::requiresTelemetryPass() { return false; }


