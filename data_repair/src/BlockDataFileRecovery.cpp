
#include "BlockDataFileRecovery.hpp"

#include <cbdf/BlockId.hpp>
#include <cbdf/BlockDataFileExceptions.hpp>
#include <cbdf/ClassIdentifiers.hpp>

#include <cbdf/extra/Crc.hpp>

#include <cbdf/extra/ExpInfoClassIdentifiers.hpp>
#include <cbdf/extra/ExperimentDataIdentifiers.hpp>

#include <cbdf/extra/ExpCtrlClassIdentifiers.hpp>
#include <cbdf/extra/PvtDataIdentifiers.hpp>
#include <cbdf/extra/SpidercamDataIdentifiers.hpp>

#include <cbdf/extra/SensorClassIdentifiers.hpp>
#include <cbdf/extra/AxisCommunicationsDataIdentifiers.hpp>
#include <cbdf/extra/SsnxDataIdentifiers.hpp>
#include <cbdf/extra/OusterDataIdentifiers.hpp>

#include <cbdf/extra/WeatherClassIdentifiers.hpp>
#include <cbdf/extra/WeatherDataIdentifiers.hpp>

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cerrno>
#include <cstring>


/*******************************************************************
 *
 * B L O C K   D A T A   F I L E   R E C O V E R Y
 *
 ******************************************************************/

bool cBlockDataFileRecovery::processBlock()
{
    if (mFile.eof())
        return false;

    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }

    mStartOfBlock = mFile.tellg();

    std::uint32_t len = 0;
    mFile.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading block length: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        if (mFile.eof())
            return false;

        throw bdf::formatting_error("I/O error while processing block.");
    }

    if (mByteSwapNeeded)
    {
    }

    BLOCK_CLASS_ID_t classID = 0;;
    BLOCK_MAJOR_VERSION_t majorVersion = 0;
    BLOCK_MINOR_VERSION_t minorVersion = 0;
    BLOCK_DATA_ID_t data_id = 0;

    mStartOfClassID = mFile.tellg();

    mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading class id: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }

    mStartOfMajorVersion = mFile.tellg();

    mFile.read(reinterpret_cast<char*>(&majorVersion), sizeof(majorVersion));;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading major version: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }

    mStartOfMinorVersion = mFile.tellg();

    mFile.read(reinterpret_cast<char*>(&minorVersion), sizeof(minorVersion));;
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading minor version: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }

    mStartOfDataID = mFile.tellg();
    mFile.read(reinterpret_cast<char*>(&data_id), sizeof(data_id));
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading data id: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }


    if (mByteSwapNeeded)
    {
    }

    cBlockID blockId(static_cast<BLOCK_CLASS_ID_t>(classID), majorVersion, minorVersion);
    blockId.dataID(data_id);

    if (len == 0)
    {
        mStartOfCRC = mFile.tellg();
        uint32_t file_crc = readCRC();
        uint32_t crc = bdf::crc(blockId);
        if (file_crc != crc)
        {
            if (tryToFixBlockId(blockId))
            {
                return !mFile.eof();
            }

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
        mStartOfPayload = mFile.tellg();
        readPayload(len, mBuffer);

        mStartOfCRC = mFile.tellg();
        uint32_t file_crc = readCRC();

/*
        if (file_crc == 0)
        {
            std::string msg = "File CRC Zero: Class ID=";
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
*/

        uint32_t crc = bdf::crc(blockId, mBuffer.data(), len);
        if (file_crc != crc)
        {
            if (tryToFixBlock(blockId, mBuffer, len))
            {
                return !mFile.eof();
            }

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

    if (len == 0)
    {
        processBlock(blockId);
    }
    else
    {
        processBlock(blockId, mBuffer.data(), len);
    }

    return !mFile.eof();
}

void cBlockDataFileRecovery::readPayload(uint32_t len, cDataBuffer& buffer)
{
    if (buffer.capacity() < len)
    {
        buffer.capacity(len);
    }

    buffer.reset();
    mFile.read(reinterpret_cast<char*>(buffer.data(len)), len);
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading block data: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        if (mFile.eof())
        {
            std::string msg = "Could not read payload of size=";
            msg += std::to_string(len);
            throw bdf::unexpected_eof(msg);
        }

        throw bdf::formatting_error("I/O error while processing block.");
    }
}

uint32_t cBlockDataFileRecovery::readCRC()
{
    uint32_t file_crc = 0;
    mFile.read(reinterpret_cast<char*>(&file_crc), sizeof(file_crc));
    if (mFile.bad())
    {
        std::string msg = "I/O error while reading file CRC: ";
        msg += std::strerror(errno);
        throw bdf::stream_error(errno, msg);
    }
    if (mFile.fail())
    {
        throw bdf::formatting_error("I/O error while processing block.");
    }

    return file_crc;
}

bool cBlockDataFileRecovery::tryToFixBlockId(const cBlockID blockID)
{
    auto result = checkBlockId(blockID, 0);
    if (result != eBlockStatus::OK)
    {
        return fixAtBlockId(blockID, 0, result);
    }
    
    uint32_t testLen = 0;
    BLOCK_CLASS_ID_t classID = 0;
    BLOCK_MAJOR_VERSION_t majorVer = 0;
    BLOCK_MINOR_VERSION_t minorVer = 0;
    BLOCK_DATA_ID_t dataID = 0;

    mFile.seekg(mStartOfCRC);
    mFile.read(reinterpret_cast<char*>(&testLen), sizeof(testLen));
    mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));
    mFile.read(reinterpret_cast<char*>(&majorVer), sizeof(majorVer));
    mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
    mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

    cBlockID testID(static_cast<BLOCK_CLASS_ID_t>(classID), majorVer, minorVer);
    testID.dataID(dataID);
    if (eBlockStatus::OK == checkBlockId(testID, testLen))
    {
        return fixAtCRC(blockID, testID, testLen);
    }

    return false;
}

bool cBlockDataFileRecovery::tryToFixBlock(const cBlockID blockID, const cDataBuffer buffer, const uint32_t len)
{
    auto result = checkBlockId(blockID, len);
    if (result != eBlockStatus::OK)
    {
        return fixAtBlockId(blockID, len, result);
    }

    uint32_t testLen = 0;
    BLOCK_CLASS_ID_t classID = 0;
    BLOCK_MAJOR_VERSION_t majorVer = 0;
    BLOCK_MINOR_VERSION_t minorVer = 0;
    BLOCK_DATA_ID_t dataID = 0;

    auto n = buffer.read_size() - 6;
    std::size_t i = 0;
    for (; i < n; ++i)
    {
        testLen = *(reinterpret_cast<const uint32_t*>(buffer.data(i)));
        classID = *(reinterpret_cast<const BLOCK_CLASS_ID_t*>(buffer.data(i+4)));
        majorVer = *(reinterpret_cast<const BLOCK_MAJOR_VERSION_t*>(buffer.data(i+6)));
        minorVer = *(reinterpret_cast<const BLOCK_MINOR_VERSION_t*>(buffer.data(i+7)));
        dataID = *(reinterpret_cast<const BLOCK_DATA_ID_t*>(buffer.data(i+8)));
        cBlockID testID(static_cast<BLOCK_CLASS_ID_t>(classID), majorVer, minorVer);
        testID.dataID(dataID);
        if (eBlockStatus::OK == checkBlockId(testID, testLen))
        {
            return fixAtDataBuffer(i, blockID, len, testID, testLen);
        }
    }

    mFile.seekg(mStartOfCRC);
    mFile.read(reinterpret_cast<char*>(&testLen), sizeof(testLen));
    mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));
    mFile.read(reinterpret_cast<char*>(&majorVer), sizeof(majorVer));
    mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
    mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

    cBlockID testID(static_cast<BLOCK_CLASS_ID_t>(classID), majorVer, minorVer);
    testID.dataID(dataID);
    if (eBlockStatus::OK == checkBlockId(testID, testLen))
    {
        return fixAtCRC(blockID, buffer, len, testID, testLen);
    }

    return false;
}

cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkBlockId(const cBlockID blockID, uint32_t len)
{
    switch (static_cast<ClassIDs>(blockID.classID()))
    {
    case static_cast<ClassIDs>(ExperimentInfoClassIDs::EXPERIMENT_INFO):
    {
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;

        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;

        return checkExperimentBlock(blockID, len);
    }
    case static_cast<ClassIDs>(ExperimentCtrlClassIDs::PVT):
    {
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;

        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;

        return checkPvtBlock(blockID, len);
    }
    case static_cast<ClassIDs>(ExperimentCtrlClassIDs::SPIDERCAM):
    {
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;

        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;

        return checkSpidercamBlock(blockID, len);
    }
    case static_cast<ClassIDs>(SensorClassIDs::OUSTER):
    {
        return eBlockStatus::BAD_CLASS_ID;
    }
    case static_cast<ClassIDs>(SensorClassIDs::OUSTER_LIDAR):
    {
        if (blockID.majorVersion() != 2)
            return eBlockStatus::BAD_MAJOR_VERSION;

        if (blockID.minorVersion() != 3)
            return eBlockStatus::BAD_MINOR_VERSION;

        return checkOusterLidarBlock(blockID, len);
    }
    case static_cast<ClassIDs>(SensorClassIDs::SEPTENTRIO):
    {
        return eBlockStatus::BAD_CLASS_ID;
    }
    case static_cast<ClassIDs>(SensorClassIDs::SSNX):
    {
        return checkSsnxBlock(blockID, len);
    }
    case static_cast<ClassIDs>(SensorClassIDs::HYSPEX):
    {
        return eBlockStatus::BAD_CLASS_ID;
    }
    case static_cast<ClassIDs>(SensorClassIDs::HYSPEX_SWIR_384):
    {
        return eBlockStatus::BAD_CLASS_ID;
    }
    case static_cast<ClassIDs>(SensorClassIDs::HYSPEX_VNIR_3000N):
    {
        return eBlockStatus::BAD_CLASS_ID;
    }
    case static_cast<ClassIDs>(SensorClassIDs::AXIS_COMMUNICATIONS):
    {
        return eBlockStatus::BAD_CLASS_ID;
    }
    case static_cast<ClassIDs>(SensorClassIDs::AXIS_COMMUNICATIONS_CAMERA):
    {
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;

        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;

        return checkAxisCommunicationBlock(blockID, len);
    }
    case static_cast<ClassIDs>(WeatherClassIDs::WEATHER):
    {
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;

        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;

        return checkWeatherBlock(blockID, len);
    }
    default:
        return eBlockStatus::BAD_CLASS_ID;
    };

    return eBlockStatus::OK;
}

bool cBlockDataFileRecovery::fixAtBlockId(const cBlockID originalBlockID, uint32_t originalLen, eBlockStatus blockStatus)
{
    std::ifstream::pos_type mStartPos = 0;
    std::ifstream::pos_type mEndPos = 0;
    switch (blockStatus)
    {
    case eBlockStatus::BAD_CLASS_ID:
        mEndPos = mStartPos = mStartOfClassID;
        mEndPos += 6;
        break;
    case eBlockStatus::BAD_MAJOR_VERSION:
        mEndPos = mStartPos = mStartOfMajorVersion;
        mEndPos += 4;
        break;
    case eBlockStatus::BAD_MINOR_VERSION:
        mEndPos = mStartPos = mStartOfMinorVersion;
        mEndPos += 3;
        break;
    case eBlockStatus::BAD_DATA_ID:
        mEndPos = mStartPos = mStartOfDataID;
        mEndPos += 2;
        break;
    case eBlockStatus::BAD_PAYLOAD:
        mEndPos = mStartPos = mStartOfPayload;
        break;
    default:
        return false;
    }

    uint32_t insertedLen = 0;
    BLOCK_CLASS_ID_t classID = 0;
    BLOCK_MAJOR_VERSION_t majorVer = 0;
    BLOCK_MINOR_VERSION_t minorVer = 0;
    BLOCK_DATA_ID_t dataID = 0;
    cBlockID insertedBlockID;
    uint32_t insertedCRC = 0;

    do
    {
        mFile.seekg(mStartPos);
        mFile.read(reinterpret_cast<char*>(&insertedLen), sizeof(insertedLen));
        mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));
        mFile.read(reinterpret_cast<char*>(&majorVer), sizeof(majorVer));
        mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
        mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

        insertedBlockID.classID(static_cast<BLOCK_CLASS_ID_t>(classID));
        insertedBlockID.setVersion(majorVer, minorVer);
        insertedBlockID.dataID(dataID);

        mStartPos += 1;

        if (mStartPos == mEndPos)
            return false;

    } while (eBlockStatus::OK != checkBlockId(insertedBlockID, insertedLen));

    cDataBuffer insertedBuffer;

    mStartPos = mFile.tellg();

    if (insertedLen == 0)
    {
        insertedCRC = bdf::crc(insertedBlockID);
        std::ifstream::pos_type crc_pos = mFile.tellg();
        uint32_t test_crc = readCRC();
        if (insertedCRC == test_crc)
        {
            cBlockID testID = originalBlockID;
            if (!recoverBlockID(testID, originalLen, blockStatus))
            {
                return false;
            }

            if (originalLen == 0)
            {
                uint32_t test_crc = readCRC();
                if (test_crc != bdf::crc(testID))
                    return false;

                processBlock(testID);
                processBlock(insertedBlockID);
                return true;
            }

            readPayload(originalLen, mBuffer);
            uint32_t test_crc = readCRC();
            auto crc = bdf::crc(testID, mBuffer.data(), originalLen);
            if (crc == test_crc)
            {
                if (originalLen != mBuffer.read_size())
                {
                    int x = 5;
                    ++x;
                }

                processBlock(testID, mBuffer.data(), originalLen);
                processBlock(insertedBlockID);
                return true;
            }

            return false;
        }

        mFile.seekg(crc_pos);
        cBlockID testID = originalBlockID;
        if (!recoverBlockID(testID, originalLen, blockStatus))
        {
            return false;
        }

        crc_pos = mFile.tellg();
        test_crc = readCRC();
        if (insertedCRC == test_crc)
        {
            if (originalLen == 0)
            {
                uint32_t test_crc = readCRC();
                if (test_crc != bdf::crc(testID))
                    return false;

                processBlock(testID);
                processBlock(insertedBlockID);
                return true;
            }

            readPayload(originalLen, mBuffer);
            uint32_t test_crc = readCRC();
            auto crc = bdf::crc(testID, mBuffer.data(), originalLen);
            if (crc == test_crc)
            {
                if (originalLen != mBuffer.read_size())
                {
                    int x = 5;
                    ++x;
                }

                processBlock(testID, mBuffer.data(), originalLen);
                processBlock(insertedBlockID);
                return true;
            }

            return false;
        }
        else if (originalLen == 0)
        {
            uint32_t crc1 = readCRC();
            if ((test_crc != bdf::crc(testID)) || (insertedCRC != crc1))
                return false;

            processBlock(testID);
            processBlock(insertedBlockID);
            return true;
        }

        mFile.seekg(crc_pos);
        readPayload(originalLen, mBuffer);
        auto crc = bdf::crc(testID, mBuffer.data(), originalLen);
        uint32_t crc1 = readCRC();
        uint32_t crc2 = readCRC();

        if (((crc == crc1) || (crc == crc2)) && ((insertedCRC == crc1) || (insertedCRC == crc2)))
        {
            if (originalLen != mBuffer.read_size())
            {
                int x = 5;
                ++x;
            }

            processBlock(testID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID);
            return true;
        }

        return false;
    }

    // There are multiple patterns that could happen here:
    // Case 0: inserted, inserted crc, block id, original, original crc
    // Case 1: inserted, block id, original, original crc, inserted crc
    // Case 2: inserted, block id, original, inserted crc, original crc
    // Case 3: inserted, block id, inserted crc, original, original crc
    // Case 4: block id, original, original crc, inserted, inserted crc
    // Case 5: block id, original, inserted, original crc, inserted crc
    // Case 6: block id, original, inserted, inserted crc, original crc
    // Case 7: block id, inserted, inserted crc, original, original crc
    // Case 8: block id, inserted, original, original crc, inserted crc
    // Case 9: block id, inserted, original, inserted crc, original crc

    // We will start by testing if the inserted block is complete
    // inserted, inserted crc, original, original crc
    std::ifstream::pos_type start_pos = mFile.tellg();
    readPayload(insertedLen, insertedBuffer);
    std::ifstream::pos_type crc_pos = mFile.tellg();
    uint32_t test_crc = readCRC();
    insertedCRC = bdf::crc(insertedBlockID, insertedBuffer.data(), insertedLen);
    if (insertedCRC == test_crc)
    {
        // We are in case 0!
        cBlockID testID = originalBlockID;
        if (!recoverBlockID(testID, originalLen, blockStatus))
        {
            return false;
        }

        readPayload(originalLen, mBuffer);
        auto crc = bdf::crc(testID, mBuffer.data(), originalLen);
        uint32_t test_crc = readCRC();
        if (crc == test_crc)
        {
            if (insertedLen != insertedBuffer.read_size())
            {
                int x = 5;
                ++x;
            }

            processBlock(testID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
            return true;

        }

        return false;
    }

    // Test for cases 1, 2, and 3...
    mFile.seekg(crc_pos);
    cBlockID testID = originalBlockID;
    if (recoverBlockID(testID, originalLen, blockStatus))
    {
        start_pos = mFile.tellg();

        // Try cases one and two...
        readPayload(originalLen, mBuffer);
        auto crc = bdf::crc(testID, mBuffer.data(), originalLen);

        uint32_t crc1 = readCRC();
        uint32_t crc2 = readCRC();

        if (((crc == crc1) || (crc == crc2)) && ((insertedCRC == crc1) || (insertedCRC == crc2)))
        {
            // Case 1 or 2!
            processBlock(testID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
            return true;
        }

        // Try cases three...
        mFile.seekg(start_pos);
        test_crc = readCRC();
        if (insertedCRC == test_crc)
        {
            // Case 3!
            readPayload(originalLen, mBuffer);
            auto crc = bdf::crc(testID, mBuffer.data(), originalLen);
            test_crc = readCRC();
            if (crc == test_crc)
            {
                processBlock(testID, mBuffer.data(), originalLen);
                processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
                return true;
            }
        }

        return false;
    }

    // Test for cases 4, 5, 6, 7, 8, and 9...
    mFile.seekg(start_pos);
    testID = originalBlockID;
    if (recoverBlockID(testID, originalLen, blockStatus))
    {
        start_pos = mFile.tellg();

        // Try cases 4, 5, and 6...
        readPayload(originalLen, mBuffer);
        auto crc = bdf::crc(testID, mBuffer.data(), originalLen);

        crc_pos = mFile.tellg();
        test_crc = readCRC();
        if (crc == test_crc)
        {
            // Case 4!
            readPayload(insertedLen, insertedBuffer);
            insertedCRC = bdf::crc(insertedBlockID, insertedBuffer.data(), insertedLen);

            uint32_t test_crc = readCRC();

            if (test_crc == insertedCRC)
            {
                processBlock(testID, mBuffer.data(), originalLen);
                processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
                return true;
            }

            return false;
        }

        // Try cases five and six...
        mFile.seekg(crc_pos);
        readPayload(insertedLen, insertedBuffer);
        insertedCRC = bdf::crc(insertedBlockID, insertedBuffer.data(), insertedLen);

        uint32_t crc1 = readCRC();
        uint32_t crc2 = readCRC();

        if (((crc == crc1) || (crc == crc2)) && ((insertedCRC == crc1) || (insertedCRC == crc2)))
        {
            // Case 5 or 6!
            processBlock(testID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
            return true;
        }

        // Try cases 7, 8, and 9...
        mFile.seekg(start_pos);
        readPayload(insertedLen, insertedBuffer);
        insertedCRC = bdf::crc(insertedBlockID, insertedBuffer.data(), insertedLen);

        crc_pos = mFile.tellg();
        test_crc = readCRC();
        if (insertedCRC == test_crc)
        {
            // Case 7!
            readPayload(originalLen, mBuffer);
            auto crc = bdf::crc(testID, mBuffer.data(), originalLen);
            
            uint32_t test_crc = readCRC();

            if (test_crc == crc)
            {
                processBlock(testID, mBuffer.data(), originalLen);
                processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
                return true;
            }

            return false;
        }

        // Try cases eight and nine...
        mFile.seekg(crc_pos);
        readPayload(originalLen, mBuffer);
        crc = bdf::crc(testID, mBuffer.data(), originalLen);

        crc1 = readCRC();
        crc2 = readCRC();

        if (((crc == crc1) || (crc == crc2)) && ((insertedCRC == crc1) || (insertedCRC == crc2)))
        {
            // Case 8 or 9!
            processBlock(testID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID, insertedBuffer.data(), insertedLen);
            return true;
        }
    }

    return false;
}

bool cBlockDataFileRecovery::recoverBlockID(cBlockID& blockID, uint32_t len, eBlockStatus blockStatus)
{
    BLOCK_CLASS_ID_t classID = 0;
    BLOCK_MAJOR_VERSION_t majorVer = 0;
    BLOCK_MINOR_VERSION_t minorVer = 0;
    BLOCK_DATA_ID_t dataID = 0;

    switch (blockStatus)
    {
    case eBlockStatus::BAD_CLASS_ID:
        mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));
        mFile.read(reinterpret_cast<char*>(&majorVer), sizeof(majorVer));
        mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
        mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

        blockID.classID(static_cast<BLOCK_CLASS_ID_t>(classID));
        blockID.setVersion(majorVer, minorVer);
        blockID.dataID(dataID);
        break;
    case eBlockStatus::BAD_MAJOR_VERSION:
        mFile.read(reinterpret_cast<char*>(&majorVer), sizeof(majorVer));
        mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
        mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

        blockID.setVersion(majorVer, minorVer);
        blockID.dataID(dataID);
        break;
    case eBlockStatus::BAD_MINOR_VERSION:
        mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
        mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

        blockID.minorVersion(minorVer);
        blockID.dataID(dataID);
        break;
    case eBlockStatus::BAD_DATA_ID:
        mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));
        blockID.dataID(dataID);
        break;
    case eBlockStatus::BAD_PAYLOAD:
    default:
        return false;
    }

    return (eBlockStatus::OK == checkBlockId(blockID, len));
}

bool cBlockDataFileRecovery::fixAtDataBuffer(const std::size_t pos,
    const cBlockID originalBlockID, uint32_t originalLen,
    const cBlockID insertedBlockID, uint32_t insertedLen)
{
    if (pos == 0)
    {
        return fixAtStartPayload(originalBlockID, originalLen,
            insertedBlockID, insertedLen);
    }

    return false;
}

bool cBlockDataFileRecovery::fixAtStartPayload(const cBlockID originalBlockID, uint32_t originalLen,
            const cBlockID insertedBlockID, uint32_t insertedLen)
{
    // This is a check to make sure we are setting the file position
    // correctly
    mFile.seekg(mStartOfPayload);

    uint32_t testLen = 0;
    BLOCK_CLASS_ID_t testClassID = 0;
    BLOCK_MAJOR_VERSION_t testMajorVer = 0;
    BLOCK_MINOR_VERSION_t testMinorVer = 0;
    BLOCK_DATA_ID_t testDataID = 0;

    mFile.read(reinterpret_cast<char*>(&testLen), sizeof(testLen));
    if (testLen != insertedLen)
        return false;

    mFile.read(reinterpret_cast<char*>(&testClassID), sizeof(testClassID));
    if (testClassID != insertedBlockID.classID())
        return false;

    mFile.read(reinterpret_cast<char*>(&testMajorVer), sizeof(testMajorVer));
    if (testMajorVer != insertedBlockID.majorVersion())
        return false;

    mFile.read(reinterpret_cast<char*>(&testMinorVer), sizeof(testMinorVer));
    if (testMinorVer != insertedBlockID.minorVersion())
        return false;

    mFile.read(reinterpret_cast<char*>(&testDataID), sizeof(testDataID));
    if (testDataID != insertedBlockID.dataID())
        return false;

    if (insertedLen == 0)
    {
        auto pos_crc = mFile.tellg();
        auto test_crc = readCRC();
        auto inserted_crc = bdf::crc(insertedBlockID);
        if (test_crc == inserted_crc)
        {
            // Ok the original block must follow!
            if (originalLen == 0)
            {
                auto test_crc = readCRC();
                auto crc = bdf::crc(originalBlockID);
                if (test_crc == crc)
                {
                    processBlock(originalBlockID);
                    processBlock(insertedBlockID);
                    return true;
                }

                return false;
            }

            readPayload(originalLen, mBuffer);
            auto test_crc = readCRC();
            auto crc = bdf::crc(originalBlockID, mBuffer.data(), originalLen);
            if (test_crc == crc)
            {
                processBlock(originalBlockID, mBuffer.data(), originalLen);
                processBlock(insertedBlockID);
                return true;
            }

            return false;
        }

        mFile.seekg(pos_crc);

        // Ok the original block payload must follow, and then both
        // CRC checked...
        readPayload(originalLen, mBuffer);
        auto crc = bdf::crc(originalBlockID, mBuffer.data(), originalLen);

        auto crc1 = readCRC();
        auto crc2 = readCRC();
        if (((crc == crc1) || (crc == crc2)) && ((inserted_crc == crc1) || (inserted_crc == crc2)))
        {
            processBlock(originalBlockID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID);
            return true;
        }

        return false;
    }

    // Here we have multiple cases...
    // Case 0: original payload, original crc, inserted payload, inserted crc
    // Case 1: original payload, inserted payload, original crc, inserted crc
    // Case 2: original payload, inserted payload, inserted crc, original crc
    // Case 3: inserted payload, inserted crc, original payload, original crc
    // Case 4: inserted payload, original payload, inserted crc, original crc
    // Case 5: inserted payload, original payload, original crc, inserted crc

    cDataBuffer insertedPayload;

    auto start_pos = mFile.tellg();

    // Test case 0...
    readPayload(originalLen, mBuffer);
    auto crc = bdf::crc(originalBlockID, mBuffer.data(), originalLen);
    auto crc_pos = mFile.tellg();
    auto test_crc = readCRC();
    if (test_crc == crc)
    {
        // In case 0!
        readPayload(insertedLen, insertedPayload);
        auto crc = bdf::crc(insertedBlockID, insertedPayload.data(), insertedLen);
        auto test_crc = readCRC();
        if (test_crc == crc)
        {
            processBlock(originalBlockID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID, insertedPayload.data(), insertedLen);
            return true;
        }
        return false;
    }

    mFile.seekg(crc_pos);

    // Test case 1 and 2...
    readPayload(insertedLen, insertedPayload);
    auto inserted_crc = bdf::crc(insertedBlockID, insertedPayload.data(), insertedLen);
    auto crc1 = readCRC();
    auto crc2 = readCRC();
    if (((crc == crc1) || (crc == crc2)) && ((inserted_crc == crc1) || (inserted_crc == crc2)))
    {
        // In case 1 or 2!
        processBlock(originalBlockID, mBuffer.data(), originalLen);
        processBlock(insertedBlockID, insertedPayload.data(), insertedLen);
        return true;
    }

    mFile.seekg(start_pos);

    // Test case 3...
    readPayload(insertedLen, insertedPayload);
    inserted_crc = bdf::crc(insertedBlockID, insertedPayload.data(), insertedLen);
    crc_pos = mFile.tellg();
    test_crc = readCRC();
    if (test_crc == crc)
    {
        // In case 3!
        readPayload(originalLen, mBuffer);
        auto crc = bdf::crc(originalBlockID, mBuffer.data(), originalLen);
        auto test_crc = readCRC();
        if (test_crc == crc)
        {
            processBlock(originalBlockID, mBuffer.data(), originalLen);
            processBlock(insertedBlockID, insertedPayload.data(), insertedLen);
            return true;
        }
        return false;
    }

    mFile.seekg(crc_pos);

    // Test case 4 and 5...
    readPayload(originalLen, mBuffer);
    crc = bdf::crc(originalBlockID, mBuffer.data(), originalLen);
    crc1 = readCRC();
    crc2 = readCRC();
    if (((crc == crc1) || (crc == crc2)) && ((inserted_crc == crc1) || (inserted_crc == crc2)))
    {
        // In case 4 or 5!
        processBlock(originalBlockID, mBuffer.data(), originalLen);
        processBlock(insertedBlockID, insertedPayload.data(), insertedLen);
        return true;
    }

    return false;
}

bool cBlockDataFileRecovery::fixAtCRC(const cBlockID originalBlockID,
    const cBlockID insertedBlockID, uint32_t insertedLen)
{
    uint32_t originalCRC = bdf::crc(originalBlockID);
    return false;
}

bool cBlockDataFileRecovery::fixAtCRC(const cBlockID originalBlockID, const cDataBuffer originalBuffer,
    uint32_t originalLen, const cBlockID insertedBlockID, uint32_t insertedLen)
{
    uint32_t originalCRC = bdf::crc(originalBlockID, originalBuffer.data(), originalLen);

    if (insertedLen == 0)
    {
        uint32_t file_crc = readCRC();

        auto crc = bdf::crc(insertedBlockID);
        if (file_crc != crc)
        {
            if (file_crc != originalCRC)
                return false;

            // Ok, we just read the CRC of the original block!
            processBlock(originalBlockID, originalBuffer.data(), originalLen);

            // Read the crc for the inserted block
            uint32_t file_crc = readCRC();
            if (file_crc != crc)
                return false;

            processBlock(insertedBlockID);
            return true;
        }

        // Read the crc for the original block
        file_crc = readCRC();
        if (file_crc != originalCRC)
            return false;

        processBlock(originalBlockID, originalBuffer.data(), originalLen);
        processBlock(insertedBlockID);
        return true;
    }

    // We are assuming this payload was inserted intacted!
    readPayload(insertedLen, mBuffer);
    auto crc = bdf::crc(insertedBlockID, mBuffer.data(), insertedLen);
    uint32_t crc1 = readCRC();
    uint32_t crc2 = readCRC();

    if (crc == crc1)
    {
        if (crc2 != originalCRC)
        {
            uint32_t testLen = crc2;
            BLOCK_CLASS_ID_t classID = 0;
            BLOCK_MAJOR_VERSION_t majorVer = 0;
            BLOCK_MINOR_VERSION_t minorVer = 0;
            BLOCK_DATA_ID_t dataID = 0;

            mFile.read(reinterpret_cast<char*>(&classID), sizeof(classID));
            mFile.read(reinterpret_cast<char*>(&majorVer), sizeof(majorVer));
            mFile.read(reinterpret_cast<char*>(&minorVer), sizeof(minorVer));
            mFile.read(reinterpret_cast<char*>(&dataID), sizeof(dataID));

            cBlockID testID(static_cast<BLOCK_CLASS_ID_t>(classID), majorVer, minorVer);
            testID.dataID(dataID);
            if (eBlockStatus::OK == checkBlockId(testID, testLen))
            {
                return fixAtCRC(originalBlockID, originalBuffer, originalLen, testID, testLen);
            }

            return false;
        }

        processBlock(originalBlockID, originalBuffer.data(), originalLen);
        processBlock(insertedBlockID, mBuffer.data(), insertedLen);
        return true;
    }
    else if (crc1 == originalCRC)
    {
        if (crc2 != crc)
            return false;

        processBlock(originalBlockID, originalBuffer.data(), originalLen);
        processBlock(insertedBlockID, mBuffer.data(), insertedLen);
        return true;
    }

    return false;
}


/**********************************************************
 * Data Block Verification
 **********************************************************/
cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkExperimentBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<experiment::DataID>(blockID.dataID());

    switch (dataId)
    {
    case experiment::DataID::EXPERIMENT:
        break;
    case experiment::DataID::START_TIME:
        if (len != 24) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::END_TIME:
        if (len != 24) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::START_RECORDING_TIMESTAMP:
        if (len != 8) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::END_RECORDING_TIMESTAMP:
        if (len != 8) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::RESEARCHER:
    case experiment::DataID::CULTIVAR:
        break;
    case experiment::DataID::EXPERIMENT_TITLE:
        if (len == 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::BEGIN_HEADER:
        if (len != 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::END_OF_HEADER:
        if (len != 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::BEGIN_FOOTER:
        if (len != 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::END_OF_FOOTER:
        if (len != 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::BEGIN_SENSOR_LIST:
        if (len != 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::END_OF_SENSOR_LIST:
        if (len != 0) return eBlockStatus::BAD_PAYLOAD;
        break;
    case experiment::DataID::SENSOR_DATA_BLOCK_INFO:
        break;
    case experiment::DataID::RECORDING_HEARTBEAT_TIMESTAMP:
        if (len != 8) return eBlockStatus::BAD_PAYLOAD;
        break;
    default:
        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}

cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkPvtBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<pvt::DataID>(blockID.dataID());

    switch (dataId)
    {
    case pvt::DataID::POSITION_UNIT:
    case pvt::DataID::VELOCITY_UNIT:
    case pvt::DataID::TIME_UNIT:
    case pvt::DataID::POSITION_1D:
    case pvt::DataID::POSITION_2D:
    case pvt::DataID::POSITION_3D:
    case pvt::DataID::VELOCITY_1D:
    case pvt::DataID::VELOCITY_2D:
    case pvt::DataID::VELOCITY_3D:
    case pvt::DataID::TIMESTAMP:
        break;
    default:
        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}

cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkAxisCommunicationBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<axis::DataID>(blockID.dataID());

    switch (dataId)
    {
    case axis::DataID::TIMESTAMP:
        return eBlockStatus::BAD_PAYLOAD;
    case axis::DataID::CAMERA_ID:
        if (len != 4) return eBlockStatus::BAD_PAYLOAD;
        break;
    case axis::DataID::RESOLUTION:
        if (len != 4) return eBlockStatus::BAD_PAYLOAD;
        break;
    case axis::DataID::FRAMES_PER_SECOND:
        if (len != 4) return eBlockStatus::BAD_PAYLOAD;
        break;
    case axis::DataID::BITMAP:
    case axis::DataID::JPEG:
    case axis::DataID::MPEG_FRAME:
        if (len < 4) return eBlockStatus::BAD_PAYLOAD;
        break;
    default:
        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}

cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkOusterLidarBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<ouster::DataID>(blockID.dataID());

    switch (dataId)
    {
    case ouster::DataID::CONFIGURATION_INFO:
    case ouster::DataID::BEAM_INTRINSICS:
    case ouster::DataID::IMU_INTRINSICS:
    case ouster::DataID::IMU_DATA:
    case ouster::DataID::LIDAR_DATA:
    case ouster::DataID::LIDAR_DATA_FORMAT:
    case ouster::DataID::LIDAR_DATA_FRAME_TIMESTAMP:
    case ouster::DataID::LIDAR_INTRINSICS:
    case ouster::DataID::MULTIPURPOSE_IO:
    case ouster::DataID::NMEA:
    case ouster::DataID::SENSOR_INFO:
    case ouster::DataID::SYNC_PULSE_IN:
    case ouster::DataID::SYNC_PULSE_OUT:
    case ouster::DataID::TIMESTAMP:
    case ouster::DataID::TIME_INFO:
        break;
    default:
        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}

cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkSpidercamBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<spidercam::DataID>(blockID.dataID());

    switch (dataId)
    {
    case spidercam::DataID::DOLLY_POSITION:
        if (len != 72) return eBlockStatus::BAD_PAYLOAD;
        break;

    default:
        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}

cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkSsnxBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<ssnx::DataID>(blockID.dataID());

    switch (dataId)
    {
    case ssnx::DataID::PVT_CARTESIAN:
        if ((blockID.majorVersion() < 1) || (blockID.majorVersion() > 2))
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        //if (len != 72) return eBlockStatus::BAD_PAYLOAD;
        return eBlockStatus::OK;
    case ssnx::DataID::PVT_GEODETIC:
        if ((blockID.majorVersion() < 1) || (blockID.majorVersion() > 2))
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() > 2)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::POS_COV_GEODETIC:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::VEL_COV_GEODETIC:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::DOP:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::PVT_RESIDUALS:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::RAIM_STATISTICS:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::PVT_GEODETIC_AUTH:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::POS_PROJECTED:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::RECEIVER_TIME:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;
    case ssnx::DataID::RTCM_DATUM:
        if (blockID.majorVersion() != 1)
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() != 0)
            return eBlockStatus::BAD_MINOR_VERSION;
        return eBlockStatus::OK;

    default:
        if ((blockID.majorVersion() < 1) || (blockID.majorVersion() > 2))
            return eBlockStatus::BAD_MAJOR_VERSION;
        if (blockID.minorVersion() > 2)
            return eBlockStatus::BAD_MINOR_VERSION;

        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}


cBlockDataFileRecovery::eBlockStatus cBlockDataFileRecovery::checkWeatherBlock(const cBlockID blockID, uint32_t len)
{
    auto dataId = static_cast<weather::DataID>(blockID.dataID());

    switch (dataId)
    {
    case weather::DataID::CONFIGURATION_INFO:
        break;
    case weather::DataID::WIND_DATA_VALID:
        if (len != 1) return eBlockStatus::BAD_PAYLOAD;
        break;
    case weather::DataID::WIND_SPEED_MPS:
        if (len != 8) return eBlockStatus::BAD_PAYLOAD;
        break;
    case weather::DataID::WIND_SPEED_KNOTS:
        if (len != 8) return eBlockStatus::BAD_PAYLOAD;
        break;
    case weather::DataID::WIND_DIRECTION_DEG:
        if (len != 8) return eBlockStatus::BAD_PAYLOAD;
        break;

    default:
        return eBlockStatus::BAD_DATA_ID;
    }

    return eBlockStatus::OK;
}

