
#pragma once


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

protected:

private:

};


class cKinematics_None : public cKinematics
{
public:
	cKinematics_None() = default;
	virtual ~cKinematics_None() = default;

	bool requiresTelemetryPass() override { return false; };
	void attachParsers(cBlockDataFileReader& file) override {};
	void detachParsers(cBlockDataFileReader& file) override {};
};
