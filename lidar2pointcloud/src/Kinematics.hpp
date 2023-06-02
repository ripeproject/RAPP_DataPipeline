
#pragma once

#include "PointCloudTypes.hpp"

#include <ouster/simple_blas.h>

#include <stdexcept>


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
	cKinematics() = default;
	virtual ~cKinematics() = default;

	bool hasMultiplePasses() const { return mNumPasses > 1; };
	uint32_t numPasses() const	   { return mNumPasses; };

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

	virtual void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) = 0;

protected:
	uint32_t mNumPasses = 0;
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

	void transform(double time_us,
		ouster::matrix_col_major<pointcloud::sCloudPoint_t>& cloud) override {};
};
