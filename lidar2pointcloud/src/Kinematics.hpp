
#pragma once

#include "PointCloudTypes.hpp"

#include <ouster/simple_blas.h>


// Forward Declarations
class cBlockDataFileReader;

class cKinematics
{
public:
	cKinematics() = default;
	virtual ~cKinematics() = default;

	/*
	 * Does the kinematics model require a pass through the
	 * data file to precompute telemetry data.
	 */
	virtual bool requiresTelemetryPass() = 0;

	/*
	 * Attach/Detach any parsers to the data file.
	 */
	virtual void attachParsers(cBlockDataFileReader& file) = 0;
	virtual void detachParsers(cBlockDataFileReader& file) = 0;

	virtual void transform(double time_us, 
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) = 0;
};


class cKinematics_None : public cKinematics
{
public:
	cKinematics_None() = default;
	virtual ~cKinematics_None() = default;

	bool requiresTelemetryPass() override { return false; };
	void attachParsers(cBlockDataFileReader& file) override {};
	void detachParsers(cBlockDataFileReader& file) override {};
	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override {};
};
