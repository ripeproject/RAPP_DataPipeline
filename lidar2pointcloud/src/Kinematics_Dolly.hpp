
#pragma once

#include "Kinematics.hpp"


class cKinematics_Dolly : public cKinematics
{
public:
	cKinematics_Dolly();

	/*
	 * Return true, we need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;

	/*
	 * Attach/detach any parsers to the data file.
	 */
	void attachParsers(cBlockDataFileReader& file) override;
	void detachParsers(cBlockDataFileReader& file) override;

	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override;

protected:

private:

};


inline bool cKinematics_Dolly::requiresTelemetryPass() { return true; }
