

#include "SsnxVerificationParser.hpp"
#include "ParserExceptions.hpp"

#include <stdexcept>


void cSsnxVerificationParser::onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_1_t pos)
{
    sensorPresent = true;
    ++mNumPosition;
}

void cSsnxVerificationParser::onPVT_Cartesian(uint8_t device_id, ssnx::gps::PVT_Cartesian_2_t pos)
{
    sensorPresent = true;
    ++mNumPosition;
}

void cSsnxVerificationParser::onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_1_t pos)
{
    sensorPresent = true;
    ++mNumPosition;
}

void cSsnxVerificationParser::onPVT_Geodetic(uint8_t device_id, ssnx::gps::PVT_Geodetic_2_t pos)
{
    sensorPresent = true;
    ++mNumPosition;
}

void cSsnxVerificationParser::onPosCovGeodetic(uint8_t device_id, ssnx::gps::PosCovGeodetic_1_t cov)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onVelCovGeodetic(uint8_t device_id, ssnx::gps::VelCovGeodetic_1_t cov)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onDOP(uint8_t device_id, ssnx::gps::DOP_1_t dop)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onPVT_Residuals(uint8_t device_id, ssnx::gps::PVT_Residuals_1_t residuals)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onRAIMStatistics(uint8_t device_id, ssnx::gps::RAIMStatistics_1_t raim)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onPOS_Local(uint8_t device_id, ssnx::gps::POS_Local_1_t pos)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onPOS_Projected(uint8_t device_id, ssnx::gps::POS_Projected_1_t pos)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onReceiverTime(uint8_t device_id, ssnx::gps::ReceiverTime_1_t time)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onRtcmDatum(uint8_t device_id, ssnx::gps::RtcmDatum_1_t datum)
{
    sensorPresent = true;
}

void cSsnxVerificationParser::onReferencePoint(uint8_t device_id, double avgLat_rad, double avgLng_rad, double avgHeight_m,
    double stdLat_rad, double stdLng_rad, double stdHeight_m, bool heightComputed)
{
    sensorPresent = true;
}

