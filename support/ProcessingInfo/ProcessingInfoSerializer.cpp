
#include "ProcessingInfoSerializer.hpp"
#include "ProcessingInfoBlockId.hpp"
#include "ProcessingInfoDataIdentifiers.hpp"

#include <cbdf/BlockDataFile.hpp>

#include <cassert>
#include <stdexcept>

using namespace processing_info;

cProcessingInfoSerializer::cProcessingInfoSerializer()
:
    cBlockSerializer(), mBlockID(new cProcessingInfoID())
{}

cProcessingInfoSerializer::cProcessingInfoSerializer(std::size_t n, cBlockDataFileWriter* pDataFile)
:
    cBlockSerializer(n, pDataFile), mBlockID(new cProcessingInfoID())
{
}

cProcessingInfoSerializer::~cProcessingInfoSerializer() {};

cBlockID& cProcessingInfoSerializer::blockID()
{
    return *mBlockID;
}

void cProcessingInfoSerializer::write(const sProcessInfo_1_t& info)
{
    assert(mpDataFile);

    mBlockID->setVersion(1,0);
	mBlockID->dataID(DataID::PROCESSING_INFO);

    mDataBuffer.clear();
    mDataBuffer << info.name;
    mDataBuffer.put<uint16_t>(static_cast<uint16_t>(info.type));
    mDataBuffer << info.year;
    mDataBuffer << info.month;
    mDataBuffer << info.day;
    mDataBuffer << info.hour;
    mDataBuffer << info.minute;
    mDataBuffer << info.seconds;

    assert(!mDataBuffer.overrun());

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sProcessInfo_1_t data.");

    mpDataFile->writeBlock(*mBlockID, mDataBuffer.data(), mDataBuffer.size());
}


