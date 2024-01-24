
#pragma once

#include "Kinematics.hpp"

#include <cstdint>


class cKinematics_Constant : public cKinematics
{
public:
	enum class eHEIGHT_AXIS { NONE, X, Y, Z };
public:
	cKinematics_Constant(double Vx_mmps, double Vy_mmps, double Vz_mmps);

	void setHeightAxis(eHEIGHT_AXIS axis);

	void setInitialPosition_m(double x_m, double y_m, double z_m);

//	void setX_Offset_m(double offset_m);
//	void setY_Offset_m(double offset_m);
//	void setZ_Offset_m(double offset_m);

	double Vx_mps() const;
	double Vy_mps() const;
	double Vz_mps() const;


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
	bool transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud) override;

protected:

private:
	double mX_Offset_m = 0.0;
	double mY_Offset_m = 0.0;
	double mZ_Offset_m = 0.0;

	eHEIGHT_AXIS mHeightAxis = eHEIGHT_AXIS::NONE;

	double mVx_mps = 0.0;
	double mVy_mps = 0.0;
	double mVz_mps = 0.0;

	uint64_t mStartTimestamp_ns = 0;
};


inline double cKinematics_Constant::Vx_mps() const { return mVx_mps; }
inline double cKinematics_Constant::Vy_mps() const { return mVy_mps; }
inline double cKinematics_Constant::Vz_mps() const { return mVz_mps; }

inline bool cKinematics_Constant::requiresTelemetryPass() { return false; }
inline void cKinematics_Constant::telemetryPassComplete() {};


