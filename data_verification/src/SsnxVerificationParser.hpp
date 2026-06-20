/**
 * @file
 * @brief Parser for reading SSNX gps data from a block based data file
 */
#pragma once

#include <cbdf/SsnxParser.hpp>

class cSsnxVerificationParser : public cSsnxParser
{
public:
	bool sensorPresent = false;

	int mNumPosition = 0;

	void onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_1_t pos) override;
	void onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_2_t pos) override;
	void onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_1_t pos) override;
	void onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_2_t pos) override;
	void onPosCovGeodetic(uint8_t device_id, ssnx::gps::PosCovGeodetic_1_t cov) override;
	void onVelCovGeodetic(uint8_t device_id, ssnx::gps::VelCovGeodetic_1_t cov) override;
	void onDOP(uint8_t device_id, ssnx::gps::DOP_1_t dop) override;
	void onPVT_Residuals(uint8_t device_id, ssnx::gps::PVT_Residuals_1_t residuals) override;
	void onRAIMStatistics(uint8_t device_id, ssnx::gps::RAIMStatistics_1_t raim) override;
	void onPOS_Local(uint8_t device_id, ssnx::gps::POS_Local_1_t pos) override;
	void onPOS_Projected(uint8_t device_id, ssnx::gps::POS_Projected_1_t pos) override;
	void onReceiverTime(uint8_t device_id, ssnx::gps::ReceiverTime_1_t time) override;
	void onRtcmDatum(uint8_t device_id, ssnx::gps::RtcmDatum_1_t datum) override;

	void onReferencePoint(uint8_t device_id, double avgLat_rad, double avgLng_rad, double avgHeight_m,
		double stdLat_rad, double stdLng_rad, double stdHeight_m, bool heightComputed) override;


protected:
};


