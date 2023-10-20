
#pragma once

#include "BlockId.hpp"

namespace v1
{
	/* Make the table for a fast CRC. */
	void make_crc_table();

	/* Return the CRC of the block id and block bytes buf[0..len-1]. */
	uint32_t crc(const cBlockID& blockID, const std::byte* buf, std::size_t len);

	/* Return the CRC of the block id.  Used in zero length data blocks */
	uint32_t crc(const cBlockID& blockID);
}
