#pragma once

#include <cbdf/OusterInfoTypes.hpp>

#include <string>
#include <optional>
#include <deque>
#include <vector>
#include <memory>

// Forward Declarations
class cPointCloudGenerator;
class cOusterLidarData;


class cOusterInfo
{
public:
	cOusterInfo();
	virtual ~cOusterInfo() = default;

public:
	void clear();

	std::weak_ptr<cPointCloudGenerator> getPointCloudGenerator() const;

public:
	double getUpdateRate_Hz() const;

	const std::optional<nOusterTypes::imu_intrinsics_2_t>&	getImuIntrinsics() const;

	const std::deque<nOusterTypes::imu_data_t>&	getImuData() const;

protected:
	void setUpdateRate_Hz(double rate_hz);

	void setBeamIntrinsics(nOusterTypes::beam_intrinsics_2_t intrinsics);
	void setImuIntrinsics(nOusterTypes::imu_intrinsics_2_t intrinsics);
	void setLidarIntrinsics(nOusterTypes::lidar_intrinsics_2_t intrinsics);
	void setLidarDataFormat(nOusterTypes::lidar_data_format_2_t format);

	void clearImuData();
	void addImuData(nOusterTypes::imu_data_t data);

	void addLidarData(const cOusterLidarData& data);

private:
	double mUpdateRate_Hz = 10.0;

	std::optional<nOusterTypes::imu_intrinsics_2_t>		mImuIntrinsics;

	std::vector<int> mCorrectedPixelShiftByRow;

	std::deque<nOusterTypes::imu_data_t>	mImuData;

	std::shared_ptr<cPointCloudGenerator> mPointCloudGenerator;

	friend class cOusterInfoLoader;
};

