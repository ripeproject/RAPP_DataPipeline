/**
 * @file
 * @brief Serializer for storing data into a block based data file
 */
#pragma once

#include "DataBuffer.hpp"


namespace v1
{
	// Forward Declarations
	class cBlockDataFileWriter;
	class cBlockID;

	class cBlockSerializer
	{
	public:
		cBlockSerializer();
		explicit cBlockSerializer(std::size_t n, cBlockDataFileWriter* pDataFile = nullptr);
		~cBlockSerializer() = default;

		uint16_t classID() const;

		void attach(cBlockDataFileWriter* pDataFile);
		cBlockDataFileWriter* detach();

		std::size_t bufferCapacity() const;
		void setBufferCapacity(std::size_t n);

		void setVersion(uint8_t major, uint8_t minor);

		explicit operator bool() const noexcept
		{
			return mpDataFile != nullptr;
		}

	protected:
		virtual cBlockID& blockID() = 0;

	protected:
		cDataBuffer     mDataBuffer;
		cBlockDataFileWriter* mpDataFile;
	};
}
