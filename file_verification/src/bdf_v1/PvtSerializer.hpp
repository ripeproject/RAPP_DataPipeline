/**
 * @file
 * @brief Serializer for storing Position/Velocity/Time data into a block based data file
 */
#pragma once

#include "BlockSerializer.hpp"
#include "PvtBlockId.hpp"

#include <chrono>

namespace v1
{
	class cPvtSerializer : public cBlockSerializer
	{
	public:
		cPvtSerializer();
		explicit cPvtSerializer(std::size_t n, cBlockDataFileWriter* pDataFile);
		~cPvtSerializer() = default;

		void write(pvt::ePOSTION_UNITS unit);
		void write(pvt::eVELOCITY_UNITS unit);
		void write(pvt::eTIME_UNITS unit);

		void write_position(double x);
		void write_position(double x, double y);
		void write_position(double x, double y, double z);

		void write_velocity(double x);
		void write_velocity(double x, double y);
		void write_velocity(double x, double y, double z);

		void write_timestamp(std::uint64_t t);

	protected:
		cBlockID& blockID() override;

	private:
		cPvtID    mBlockID;
	};
}
