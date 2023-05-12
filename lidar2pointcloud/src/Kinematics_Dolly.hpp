
#pragma once

#include "Kinematics.hpp"

#include <cbdf/SpidercamParser.hpp>

#include <vector>


class cKinematics_Dolly : public cKinematics, public cSpidercamParser
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
	void attachParsers(cBlockDataFileReader& file) override;
	void detachParsers(cBlockDataFileReader& file) override;

	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override;

protected:
	void onPosition(spidercam::sPosition_1_t position) override;

private:

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

