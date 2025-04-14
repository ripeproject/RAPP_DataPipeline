
#include "OusterInfo.hpp"

#include "PointCloudGenerator.hpp"

#include <stdexcept>
#include <algorithm>
#include <numbers>
#include <cmath>


using namespace ouster;


cOusterInfo::cOusterInfo()
{
	mPointCloudGenerator = std::make_shared<cPointCloudGenerator>();
}

void cOusterInfo::clear()
{
	mImuIntrinsics.reset();

	mImuData.clear();

	mPointCloudGenerator->clear();
}

/**********************************************************
* Getters
***********************************************************/

std::weak_ptr<cPointCloudGenerator> cOusterInfo::getPointCloudGenerator() const
{
	return mPointCloudGenerator;
}

double cOusterInfo::getUpdateRate_Hz() const
{
	return mUpdateRate_Hz;
}

const std::optional<nOusterTypes::imu_intrinsics_2_t>& cOusterInfo::getImuIntrinsics() const
{
	return mImuIntrinsics;
}

const std::deque<nOusterTypes::imu_data_t>& cOusterInfo::getImuData() const
{
	return mImuData;
}


/**********************************************************
* Setters
***********************************************************/
void cOusterInfo::setUpdateRate_Hz(uint8_t device_id, double rate_hz)
{
	if (rate_hz > 0.0)
		mUpdateRate_Hz = rate_hz;
}

void cOusterInfo::setBeamIntrinsics(uint8_t device_id, nOusterTypes::beam_intrinsics_2_t intrinsics)
{
	mPointCloudGenerator->setBeamOffset(intrinsics.lidar_to_beam_origins_mm);
	mPointCloudGenerator->setBeamAzimuthAngles_deg(intrinsics.azimuth_angles_deg);
	mPointCloudGenerator->setBeamAltitudeAngles_deg(intrinsics.altitude_angles_deg);
}

void cOusterInfo::setImuIntrinsics(uint8_t device_id, nOusterTypes::imu_intrinsics_2_t intrinsics)
{
	mImuIntrinsics = intrinsics;
}

void cOusterInfo::setLidarIntrinsics(uint8_t device_id, nOusterTypes::lidar_intrinsics_2_t intrinsics)
{
	mPointCloudGenerator->setLidarToSensorTransform(intrinsics.lidar_to_sensor_transform);
}

void cOusterInfo::setLidarDataFormat(uint8_t device_id, nOusterTypes::lidar_data_format_2_t format)
{
	mPointCloudGenerator->setDimensions(format.columns_per_frame, format.pixels_per_column);

	mCorrectedPixelShiftByRow = format.pixel_shift_by_row;
	for (int i = 0; i < mCorrectedPixelShiftByRow.size(); ++i)
		mCorrectedPixelShiftByRow[i] -= 12;
}

void cOusterInfo::clearImuData()
{
	mImuData.clear();
}

void cOusterInfo::addImuData(uint8_t device_id, nOusterTypes::imu_data_t data)
{
	mImuData.push_back(data);
}

void cOusterInfo::clearLidarData()
{
}

void cOusterInfo::addLidarData(uint8_t device_id, const cOusterLidarData& data)
{
	mPointCloudGenerator->addLidarData(data);
}

