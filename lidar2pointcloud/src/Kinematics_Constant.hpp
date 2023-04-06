
#pragma once

#include "Kinematics.hpp"


class cKinematics_Constant : public cKinematics
{
public:
	cKinematics_Constant();

	/*
	 * Return false, no need to precompute telemetry data for this model.
	 */
	bool requiresTelemetryPass() override;

	/*
	 * Attach any parsers to the data file.
 	 */
	void attachParsers(cBlockDataFileReader& file) override {};
	void detachParsers(cBlockDataFileReader& file) override {};

protected:

private:

};


inline bool cKinematics_Constant::requiresTelemetryPass() { return false; }


