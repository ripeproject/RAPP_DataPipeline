/**
 * @file
 * @brief Parser for reading data from a block based data file
 */
#pragma once

#include "BlockId.hpp"
#include "DataBuffer.hpp"

namespace v1
{
	// Forward Declarations
	class cBlockDataFileReader;
	class cBlockID;


	class cBlockParser
	{
	public:
		cBlockParser();
		~cBlockParser() = default;

		void setVersion(uint8_t major, uint8_t minor);

		cBlockID blockID() const;
		virtual cBlockID& blockID() = 0;

		virtual void processData(BLOCK_MAJOR_VERSION_t major_version,
			BLOCK_MINOR_VERSION_t minor_version,
			BLOCK_DATA_ID_t data_id,
			cDataBuffer& buffer) = 0;
	};

}

