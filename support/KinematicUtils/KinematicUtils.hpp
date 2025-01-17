
#pragma once

#include "datatypes.hpp"
#include "KinematicDataTypes.hpp"

#include <cbdf/OusterInfoTypes.hpp>
#include <cbdf/SpiderCamInfoTypes.hpp>
#include <cbdf/SsnxInfoTypes.hpp>

#include <ouster/simple_blas.h>
#include <ouster/ouster_utils.h>

#include <vector>
#include <deque>

// Forward Declarations


std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
	const rfm::rappSpeeds_t& speed, double* scan_time_sec = nullptr);

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const std::deque<nSpiderCamTypes::sPosition_t>& data, uint32_t startIndex, uint32_t endIndex);

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const std::deque<nSsnxTypes::sPvtGeodetic_t>& data);

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
	const std::deque<nSsnxTypes::sPvtGeodetic_t>& vel, bool ignoreInvalidGroundTrackData, double* scan_time_sec = nullptr);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	double pitch_offset_deg, double roll_offset_deg, double yaw_offset_deg, double scan_time_sec);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	kdt::sDollyAtitude_t start, kdt::sDollyAtitude_t end, double scan_time_sec);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	std::vector<kdt::sDollyOrientationInterpPoint_t> table, double scan_time_sec);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	const std::deque<nOusterTypes::imu_data_t>& imu, nOusterTypes::imu_intrinsics_2_t transform);

void mergeDollyOrientation(std::vector<kdt::sDollyInfo_t>& dolly, const std::vector<kdt::sDollyOrientation_t>& orientation);


std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
	const rfm::rappSpeeds_t& speed, double* scan_time_sec = nullptr);

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const std::deque<nSpiderCamTypes::sPosition_t>& data, uint32_t startIndex, uint32_t endIndex);

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const std::deque<nSsnxTypes::sPvtGeodetic_t>& data);

std::vector<kdt::sDollyInfo_t> computeDollyKinematics(int id, const rfm::rappPoint_t& start, const rfm::rappPoint_t& end,
	const std::deque<nSsnxTypes::sPvtGeodetic_t>& vel, bool ignoreInvalidGroundTrackData, double* scan_time_sec = nullptr);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	double pitch_offset_deg, double roll_offset_deg, double yaw_offset_deg, double scan_time_sec);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	kdt::sDollyAtitude_t start, kdt::sDollyAtitude_t end, double scan_time_sec);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	std::vector<kdt::sDollyOrientationInterpPoint_t> table, double scan_time_sec);

std::vector<kdt::sDollyOrientation_t> computeDollyOrientationKinematics(int id, double mount_pitch_deg, double mount_roll_deg, double mount_yaw_deg,
	const std::deque<nOusterTypes::imu_data_t>& imu, nOusterTypes::imu_intrinsics_2_t transform);

void mergeDollyOrientation(int id, std::vector<kdt::sDollyInfo_t>& dolly, const std::vector<kdt::sDollyOrientation_t>& orientation);

bool transform(double time_us, const std::vector<kdt::sDollyInfo_t>& path,
	ouster::matrix_col_major<rfm::sPoint3D_t>& cloud, std::vector<kdt::sDollyInfo_t>* pComputedPath, double* displacement_mm = nullptr);
