
#pragma once

#include "Kinematics.hpp"


class cKinematics_SLAM : public cKinematics
{
public:
	cKinematics_SLAM();

	void writeHeader(cPointCloudSerializer& serializer) override;

	/*
	 * Return false, no need to precompute telemetry data for this model.
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

private:

};


inline bool cKinematics_SLAM::requiresTelemetryPass() { return false; }
inline void cKinematics_SLAM::telemetryPassComplete() {};


