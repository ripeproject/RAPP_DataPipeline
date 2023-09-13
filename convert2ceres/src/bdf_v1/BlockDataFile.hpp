
#pragma once

#include "BlockId.hpp"
#include "BlockParser.hpp"
#include "DataBuffer.hpp"

#include <cstddef>
#include <string>
#include <cstdio>
#include <fstream>
#include <map>
#include <mutex>
#include <functional>

// Forward Declares

/**
	Storage of the data from the Spidercam system is based on a block file format.

	Data File Signature
	The first eight bytes of the datastream always contain the following(decimal) values:

	137 83 80 68 82 67 77 03

	The next four bytes of the datastream are used as a byte order marker (BOM).  The
	BOM is the hexidecimal number 55AA.  This can be written into file as:

	Big Endian: 55AA	or	Little Endian: AA55


	This signature indicates that the remainder of the datastream consisting of a series
	of data blocks.

	Block Layout
	Each block consists of three or four fields:

	+----------+  +--------------+  +--------------+  +-------+
	|  Length  |  |  Block Type  |  |  Block Data  |  |  CRC  |
	+----------+  +--------------+  +--------------+  +-------+

	or

	+----------+  +--------------+  +-------+
	|  Length  |  |  Block Type  |  |  CRC  |
	+----------+  +--------------+  +-------+


	The block data field may be empty.

	Length		: A four-byte unsigned integer giving the number of bytes in the block's data field.
				  The length counts only the data field, not itself, the block type, or the CRC.
				  Zero is a valid length. Although encoders and decoders should treat the length
				  as unsigned, its value shall not exceed 2^31-1 bytes.

	Block Type	: A six-byte unsigned integer defining the block type.  See BlockId.hpp.

	Block Data	: The data bytes appropriate to the block type, if any. This field can be of zero length.

	CRC			: A four-byte CRC (Cyclic Redundancy Code) calculated on the preceding bytes in the block,
				  including the block type field and block data fields, but not including the length field.
				  The CRC can be used to check for corruption of the data.  The CRC is always present, even
				  for blocks containing no data.
*/

namespace v1
{

	class cBlockDataFileWriter
	{
	public:
		cBlockDataFileWriter();
		explicit cBlockDataFileWriter(const std::string& filename);

		cBlockDataFileWriter(const cBlockDataFileWriter&) = delete;
		cBlockDataFileWriter(cBlockDataFileWriter&&) = delete;

		cBlockDataFileWriter& operator=(const cBlockDataFileWriter&) = delete;
		cBlockDataFileWriter& operator=(cBlockDataFileWriter&&) = delete;

		~cBlockDataFileWriter();

		bool open(const std::string& filename);
		bool isOpen() const;
		void close();

		bool fail() const;
		bool bad() const;
		bool good() const;

		bool writeBlock(const cBlockID& id);
		bool writeBlock(const cBlockID& id, const std::byte* buf, std::size_t len);

	private:
		std::ofstream mFile;
		std::mutex mWriteMutex;
	};



	class cBlockDataFileReader
	{
	public:
		typedef std::function<void(const cBlockID& id)> emptyBlock_t;
		typedef std::function<void(const cBlockID& id, const std::byte* buf, std::size_t len)> dataBlock_t;

	public:
		cBlockDataFileReader();
		explicit cBlockDataFileReader(const std::string& filename);

		cBlockDataFileReader(const cBlockDataFileReader&) = delete;
		cBlockDataFileReader(cBlockDataFileReader&&) = delete;

		cBlockDataFileReader& operator=(const cBlockDataFileReader&) = delete;
		cBlockDataFileReader& operator=(cBlockDataFileReader&&) = delete;

		~cBlockDataFileReader();

		bool open(const std::string& filename);
		bool isOpen() const;
		void close();

		bool fail() const;
		bool bad() const;
		bool good() const;
		bool eof() const;

		std::streampos filePosition();
		void gotoPosition(std::streampos pos);

		/**
		 * @brief Returns the file size
		 */
		std::uintmax_t size() const;

		void attach(cBlockParser* pParser);
		cBlockParser* detach(cBlockID id);

		void registerCallback(emptyBlock_t callback);
		void registerCallback(dataBlock_t callback);

		bool processBlock();

	private:
		//    FILE* mpFile;
		std::ifstream mFile;
		std::uintmax_t mFileSize = 0;

		std::string mFileName;
		bool mByteSwapNeeded;

		cDataBuffer mBuffer;
		std::map<BLOCK_CLASS_ID_t, cBlockParser*> mParsers;

		emptyBlock_t mEmptyBlockCallback;
		dataBlock_t  mDataBlockCallback;
	};

}

