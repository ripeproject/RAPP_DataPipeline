
#include "BlockDataFile.hpp"
#include "BlockId.hpp"
#include "BlockDataFileExceptions.hpp"
#include "Crc.hpp"

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cerrno>
#include <cstring>


namespace
{
    static const uint8_t  BLOCK_FILE_HEADER_SIZE = 8;
    static const uint8_t  BLOCK_FILE_HEADER[BLOCK_FILE_HEADER_SIZE] = { 137, 83, 80, 68, 82, 67, 77, 3 };
    static const uint16_t BLOCK_FILE_BOM = 0x55AA;
}

/*******************************************************************
 * 
 * B L O C K   D A T A   F I L E   W R I T E R
 * 
 ******************************************************************/

v1::cBlockDataFileWriter::cBlockDataFileWriter()
{
    v1::make_crc_table();
}

v1::cBlockDataFileWriter::cBlockDataFileWriter(const std::string& filename)
    : cBlockDataFileWriter()
{
    if (!open(filename))
    {
        std::string msg = "Failed to open file: ";
        msg += filename;
        throw bdf::io_error(msg);
    }
}

v1::cBlockDataFileWriter::~cBlockDataFileWriter()
{
    mFile.close();
}


bool v1::cBlockDataFileWriter::open(const std::string& filename)
{
    mFile.open(filename, std::ios_base::binary | std::ios_base::trunc);
    if (mFile.good() && mFile.is_open())
    {
        mFile.write(reinterpret_cast<const char*>(BLOCK_FILE_HEADER), sizeof(BLOCK_FILE_HEADER));
        if (!mFile.good())
        {
            mFile.close();
            return false;
        }

        mFile.write(reinterpret_cast<const char*>(&BLOCK_FILE_BOM), sizeof(BLOCK_FILE_BOM));
        if (!mFile.good())
        {
            mFile.close();
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool v1::cBlockDataFileWriter::isOpen() const
{
    return mFile.is_open();
}

void v1::cBlockDataFileWriter::close()
{
    mFile.close();
}

bool v1::cBlockDataFileWriter::fail() const
{
    return mFile.fail();
}

bool v1::cBlockDataFileWriter::bad() const
{
    return mFile.bad();
}

bool v1::cBlockDataFileWriter::good() const
{
    return mFile.good();
}

bool v1::cBlockDataFileWriter::writeBlock(const cBlockID& id)
{
    std::lock_guard<std::mutex> write_guard(mWriteMutex);

    std::uint32_t n = 0;
    mFile.write(reinterpret_cast<const char*>(&n), sizeof(n));

    BLOCK_CLASS_ID_t classID = id.classID();
    BLOCK_MAJOR_VERSION_t majorVersion = id.majorVersion();
    BLOCK_MINOR_VERSION_t minorVersion = id.minorVersion();
    BLOCK_DATA_ID_t dataID = id.dataID();

    mFile.write(reinterpret_cast<const char*>(&classID), sizeof(classID));
    mFile.write(reinterpret_cast<const char*>(&majorVersion), sizeof(majorVersion));
    mFile.write(reinterpret_cast<const char*>(&minorVersion), sizeof(minorVersion));
    mFile.write(reinterpret_cast<const char*>(&dataID), sizeof(dataID));

    uint32_t c = crc(id);
    mFile.write(reinterpret_cast<const char*>(&c), sizeof(c));

    return mFile.good();
}

bool v1::cBlockDataFileWriter::writeBlock(const cBlockID& id, const std::byte* buf, std::size_t len)
{
    if (len == 0)
    {
        return writeBlock(id);
    }

    std::lock_guard<std::mutex> write_guard(mWriteMutex);

    std::uint32_t n = len;
    mFile.write(reinterpret_cast<const char*>(&n), sizeof(n));

    BLOCK_CLASS_ID_t classID = id.classID();
    BLOCK_MAJOR_VERSION_t majorVersion = id.majorVersion();
    BLOCK_MINOR_VERSION_t minorVersion = id.minorVersion();
    BLOCK_DATA_ID_t dataID = id.dataID();

    mFile.write(reinterpret_cast<const char*>(&classID), sizeof(classID));
    mFile.write(reinterpret_cast<const char*>(&majorVersion), sizeof(majorVersion));
    mFile.write(reinterpret_cast<const char*>(&minorVersion), sizeof(minorVersion));
    mFile.write(reinterpret_cast<const char*>(&dataID), sizeof(dataID));

    mFile.write(reinterpret_cast<const char*>(buf), len);

    uint32_t c = crc(id, buf, len);
    mFile.write(reinterpret_cast<const char*>(&c), sizeof(c));

    return mFile.good();
}


/*******************************************************************
 *
 * B L O C K   D A T A   F I L E   R E A D E R
 *
 ******************************************************************/

v1::cBlockDataFileReader::cBlockDataFileReader() : mByteSwapNeeded(false)
{
    v1::make_crc_table();
}

v1::cBlockDataFileReader::cBlockDataFileReader(const std::string& filename)
    : cBlockDataFileReader()
{
    open(filename);
    mBuffer.capacity(4096);
}

v1::cBlockDataFileReader::~cBlockDataFileReader()
{
    mFile.close();
}


bool v1::cBlockDataFileReader::open(const std::string& filename)
{
    mFile.open(filename, std::ios_base::binary);
    if (mFile.is_open())
    {
        unsigned char header[BLOCK_FILE_HEADER_SIZE] = { 0 };
        mFile.read(reinterpret_cast<char*>(header), BLOCK_FILE_HEADER_SIZE);
        if (mFile.bad() || mFile.fail())
        {
            mFile.close();
            return false;
        }

        bool result = std::equal(std::begin(header), std::end(header),
            std::begin(BLOCK_FILE_HEADER), std::end(BLOCK_FILE_HEADER));

        if (!result)
        {
            mFile.close();
            return false;
        }

        uint16_t bom = 0;
        mFile.read(reinterpret_cast<char*>(&bom), sizeof(bom));
        if (mFile.bad() || mFile.fail())
        {
            mFile.close();
            return false;
        }

        const uint16_t BLOCK_FILE_BOM_SWAP = 0xAA55;
        mByteSwapNeeded = bom == BLOCK_FILE_BOM_SWAP;
    }
    else
    {
        return false;
    }

    mFileName = filename;
    return true;
}

bool v1::cBlockDataFileReader::isOpen() const
{
    return mFile.is_open();
}

void v1::cBlockDataFileReader::close()
{
    mFile.close();
}

bool v1::cBlockDataFileReader::fail() const
{
    return mFile.fail();
}

bool v1::cBlockDataFileReader::bad() const
{
    return mFile.bad();
}

bool v1::cBlockDataFileReader::good() const
{
    return mFile.good();
}

bool v1::cBlockDataFileReader::eof() const
{
    return mFile.eof();
}

std::streampos v1::cBlockDataFileReader::filePosition()
{
    return mFile.tellg();
}

void v1::cBlockDataFileReader::gotoPosition(std::streampos pos)
{
    if (mFile.eof() || mFile.fail())
    {
        mFile.close();
        open(mFileName);
    }

    mFile.seekg(pos);
}


void v1::cBlockDataFileReader::attach(cBlockParser* pParser)
{
    if (!pParser) return;

    // sBlockHeader id = pParser->blockID().blockHeader();
    BLOCK_CLASS_ID_t id = pParser->blockID().classID();

    mParsers.insert(std::make_pair(id, pParser));
}

v1::cBlockParser* v1::cBlockDataFileReader::detach(cBlockID id)
{
    auto it = mParsers.find(id.classID());
    if (mParsers.end() != it)
    {
        mParsers.erase(it);
        return it->second;
    }

    return nullptr;
}

bool v1::cBlockDataFileReader::processBlock()
{
    if (mFile.eof())
        return false;

    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }

    std::uint32_t len = 0;
    mFile.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (mFile.eof())
        return false;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading block length: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }

    if (mByteSwapNeeded)
    {
    }

    BLOCK_CLASS_ID_t classID = 0;;
    BLOCK_MAJOR_VERSION_t majorVersion = 0;
    BLOCK_MINOR_VERSION_t minorVersion = 0;
    BLOCK_DATA_ID_t data_id = 0;

    mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading class id: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }

    mFile.read(reinterpret_cast<char*>(&majorVersion), sizeof(majorVersion));;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading major version: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }

    mFile.read(reinterpret_cast<char*>(&minorVersion), sizeof(minorVersion));;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading minor version: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }

    mFile.read(reinterpret_cast<char*>(&data_id), sizeof(data_id));
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading data id: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }


    if (mByteSwapNeeded)
    {
    }

    cBlockID blockId(static_cast<ClassIDs>(classID), majorVersion, minorVersion);
    blockId.dataID(data_id);

    if (len == 0)
    {
        uint32_t file_crc = 0;
        mFile.read(reinterpret_cast<char*>(&file_crc), sizeof(file_crc));
        if (mFile.bad())
        {
            std::string msg = "I/O error while reading file CRC: ";
            msg += std::strerror(errno);
            throw bdf::stream_error(errno, msg);
        }

        if (file_crc != crc(blockId))
        {
            std::string msg = "CRC failure: Class ID=";
            msg += std::to_string(classID);
            msg += ", Major Version=";
            msg += std::to_string(majorVersion);
            msg += ", Minor Version=";
            msg += std::to_string(minorVersion);
            msg += ", Data ID=";
            msg += std::to_string(data_id);
            msg += ", Data Lenth=0";
            throw bdf::crc_error(classID, majorVersion, minorVersion, data_id, msg);
        }

        mBuffer.reset();
    }
    else
    {
        if (mBuffer.capacity() < len)
        {
            mBuffer.capacity(len);
        }

        mBuffer.reset();
        mFile.read(reinterpret_cast<char*>(mBuffer.data(len)), len);
        if (mFile.bad())
        {
            std::string msg = "I/O error while reading block data: ";
            msg += std::strerror(errno);
            throw bdf::stream_error(errno, msg);
        }

        uint32_t file_crc = 0;
        mFile.read(reinterpret_cast<char*>(&file_crc), sizeof(file_crc));
        if (mFile.bad())
        {
            std::string msg = "I/O error while reading file CRC: ";
            msg += std::strerror(errno);
            throw bdf::stream_error(errno, msg);
        }

        uint32_t c = crc(blockId, mBuffer.data(), len);
        if (file_crc != c)
        {
            std::string msg = "CRC failure: Class ID=";
            msg += std::to_string(classID);
            msg += ", Major Version=";
            msg += std::to_string(majorVersion);
            msg += ", Minor Version=";
            msg += std::to_string(minorVersion);
            msg += ", Data ID=";
            msg += std::to_string(data_id);
            msg += ", Data Lenth=";
            msg += std::to_string(len);
            throw bdf::crc_error(classID, majorVersion, minorVersion, data_id, msg);
        }
    }

    auto parser = mParsers.find(classID);
    if (parser != mParsers.end())
    {
        parser->second->processData(majorVersion, minorVersion, data_id, mBuffer);
    }
    else
    {
        unknownClassID(classID);
    }

    return !mFile.eof();
}

