
#pragma once

#include "PointCloudTypes.hpp"

#include <ouster/simple_blas.h>

#include <stdexcept>
#include <fstream>


// Forward Declarations
class cBlockDataFileReader;
class cBlockDataFileWriter;
class cPointCloudSerializer;


class cKinematicException : public std::logic_error
{
public:
	using base = std::logic_error;

	explicit cKinematicException(const std::string& _message) : base(_message.c_str()) {}
	explicit cKinematicException(const char* _message) : base(_message) {}
};

class cKinematicNoData : public cKinematicException
{
public:
	using base = cKinematicException;

	cKinematicNoData() : base("No kinematic data.") {}
	explicit cKinematicNoData(const std::string& _message) : base(_message.c_str()) {}
	explicit cKinematicNoData(const char* _message) : base(_message) {}
};


enum class eKinematics { NONE, CONSTANT, DOLLY, GPS, SLAM };


class cKinematics
{
public:
	cKinematics();
	virtual ~cKinematics() = default;

	bool     hasMultiplePasses() const { return mNumPasses > 1; };
	uint32_t numPasses() const	       { return mNumPasses; };
	bool     atEndOfPass() const	   { return mAtEndOfPass; };

	double getSensorPitch_deg() const;
	double getSensorRoll_deg() const;
	double getSensorYaw_deg() const;

	bool   rotateToSEU() const;
	void   rotateToSEU(bool apply);

	void setSensorOrientation(double yaw_deg, double pitch_deg,
								double roll_deg);

	void setSensorOrientation(double pitch_deg, double roll_deg);

	const ouster::cRotationMatrix<double>& getRotationMatrix()const { return mSensorToSEU; }

	virtual void writeHeader(cPointCloudSerializer& serializer) = 0;

	/*
	 * Does the kinematics model require a pass through the
	 * data file to precompute telemetry data.
	 */
	virtual bool requiresTelemetryPass() = 0;
	virtual void telemetryPassComplete() = 0;

	/*
	 * Attach/Detach any parsers needed for the kinematics pass to the data file.
	 */
	virtual void attachKinematicParsers(cBlockDataFileReader& file) = 0;
	virtual void detachKinematicParsers(cBlockDataFileReader& file) = 0;

	/*
	 * Attach/Detach any parsers/serializers needed for the transform pass to the data file.
	 */
	virtual void attachTransformParsers(cBlockDataFileReader& file) = 0;
	virtual void detachTransformParsers(cBlockDataFileReader& file) = 0;

	virtual void attachTransformSerializers(cBlockDataFileWriter& file) = 0;
	virtual void detachTransformSerializers(cBlockDataFileWriter& file) = 0;

	void rotate(float& x, float& y, float& z);
	void rotate(double& x, double& y, double& z);
	void rotate(pointcloud::sCloudPoint_t& p);

	virtual bool transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud) = 0;

	void setDebugFileName(const std::string& filename);

protected:
	uint32_t mNumPasses = 0;
	bool	 mAtEndOfPass = false;

private:
	/*
	 * The rotation needed to convert sensor orientation
	 * to the spidercam South\East\Up coordination system
	 */
	ouster::cRotationMatrix<double> mSensorToSEU;

	bool   mRotateSensorData = false;

private:
	double mPitch_deg = 0.0;
	double mRoll_deg = 0.0;
	double mYaw_deg = 0.0;

protected:
	bool			mDebugActive = false;
	std::string		mDebugFilename;
	std::ofstream	mDebugFile;
};


class cKinematics_None : public cKinematics
{
public:
	cKinematics_None() = default;
	virtual ~cKinematics_None() = default;

	void writeHeader(cPointCloudSerializer& serializer) override {};
	bool requiresTelemetryPass() override { return false; };
	void telemetryPassComplete() override {};

	void attachKinematicParsers(cBlockDataFileReader& file) override {};
	void detachKinematicParsers(cBlockDataFileReader& file) override {};

	void attachTransformParsers(cBlockDataFileReader& file) override {};
	void detachTransformParsers(cBlockDataFileReader& file) override {};

	void attachTransformSerializers(cBlockDataFileWriter& file) override {};
	void detachTransformSerializers(cBlockDataFileWriter& file) override {};

	bool transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_SensorInfo_t>& cloud) override 
	{
		return true;
	};
};
