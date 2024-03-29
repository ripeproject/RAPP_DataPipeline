/**
 * @file
 * @brief Parser for reader Position/Velocity/Time data from a block based data file
 */
#pragma once

#include "BlockParser.hpp"
#include "PvtBlockId.hpp"

#include <chrono>

namespace v1
{
	class cPvtParser : public cBlockParser
	{
	public:
		cPvtParser();
		~cPvtParser() = default;

		cBlockID& blockID() override;

		pvt::ePOSTION_UNITS  positionUnit() const { return mPositionUnit; };
		pvt::eVELOCITY_UNITS velocityUnit() const { return mVelocityUnit; };
		pvt::eTIME_UNITS	 timeUnit() const { return mTimeUnit; };

		double X() const { return mX; };
		double Y() const { return mY; };
		double Z() const { return mZ; };

		double Vx() const { return mVx; };
		double Vy() const { return mVy; };
		double Vz() const { return mVz; };

		std::uint64_t timeStamp() const { return mTimeStamp; };


		void processData(BLOCK_MAJOR_VERSION_t major_version,
			BLOCK_MINOR_VERSION_t minor_version,
			BLOCK_DATA_ID_t data_id,
			cDataBuffer& buffer) override;

		virtual void processPositionUnits(v1::cDataBuffer& buffer);
		virtual void processVelocityUnits(v1::cDataBuffer& buffer);
		virtual void processTimeUnits(v1::cDataBuffer& buffer);

		virtual void processPosition1D(v1::cDataBuffer& buffer);
		virtual void processPosition2D(v1::cDataBuffer& buffer);
		virtual void processPosition3D(v1::cDataBuffer& buffer);

		virtual void processVelocity1D(v1::cDataBuffer& buffer);
		virtual void processVelocity2D(v1::cDataBuffer& buffer);
		virtual void processVelocity3D(v1::cDataBuffer& buffer);

		virtual void processTimestamp(v1::cDataBuffer& buffer);

	protected:
		pvt::ePOSTION_UNITS  mPositionUnit;
		pvt::eVELOCITY_UNITS mVelocityUnit;
		pvt::eTIME_UNITS	 mTimeUnit;

		double mX, mY, mZ;
		double mVx, mVy, mVz;
		std::uint64_t mTimeStamp;

	private:
		cPvtID    mBlockID;
	};
}
