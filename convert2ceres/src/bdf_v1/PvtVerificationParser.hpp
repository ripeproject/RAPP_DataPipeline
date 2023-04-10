/**
 * @file
 * @brief Parser for reader Position/Velocity/Time data from a block based data file
 */
#pragma once

#include "PvtParser.hpp"
#include "DataBuffer.hpp"

namespace v1
{
	class cPvtVerificationParser : public cPvtParser
	{
	public:
		virtual void onPositionUnits(pvt::ePOSTION_UNITS  positionUnit) = 0;
		virtual void onVelocityUnits(pvt::eVELOCITY_UNITS velocityUnit) = 0;
		virtual void onTimeUnits(pvt::eTIME_UNITS timeUnit) = 0;
		virtual void onPosition1D(double x) = 0;
		virtual void onPosition2D(double x, double y) = 0;
		virtual void onPosition3D(double x, double y, double z) = 0;
		virtual void onVelocity1D(double Vx) = 0;
		virtual void onVelocity2D(double Vx, double Vy) = 0;
		virtual void onVelocity3D(double Vx, double Vy, double Vz) = 0;
		virtual void onTimestamp(std::uint64_t timeStamp) = 0;

	protected:
		void processPositionUnits(v1::cDataBuffer& buffer) override;
		void processVelocityUnits(v1::cDataBuffer& buffer) override;
		void processTimeUnits(v1::cDataBuffer& buffer) override;

		void processPosition1D(v1::cDataBuffer& buffer) override;
		void processPosition2D(v1::cDataBuffer& buffer) override;
		void processPosition3D(v1::cDataBuffer& buffer) override;

		void processVelocity1D(v1::cDataBuffer& buffer) override;
		void processVelocity2D(v1::cDataBuffer& buffer) override;
		void processVelocity3D(v1::cDataBuffer& buffer) override;

		void processTimestamp(v1::cDataBuffer& buffer) override;
	};
}
