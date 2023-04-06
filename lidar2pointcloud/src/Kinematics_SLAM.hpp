
#pragma once

#include "Kinematics.hpp"


class cKinematics_SLAM : public cKinematics
{
public:
	cKinematics_SLAM();

	/*
	 * Return false, no need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;

	/*
	 * Attach/detach any parsers to the data file.
	 */
	void attachParsers(cBlockDataFileReader& file) override;
	void detachParsers(cBlockDataFileReader& file) override;

protected:

private:

};


inline bool cKinematics_SLAM::requiresTelemetryPass() { return false; }


