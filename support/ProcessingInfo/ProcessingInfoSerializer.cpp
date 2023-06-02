
#include "ProcessingInfoSerializer.hpp"
#include "ProcessingInfoBlockId.hpp"
#include "ProcessingInfoDataIdentifiers.hpp"

#include <cbdf/BlockDataFile.hpp>

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

void cProcessingInfoSerializer::write(const std::string& name, processing_info::ePROCESSING_TYPE type)
{
    processing_info::sProcessInfo_1_t info;

    info.name = name;
    info.type = type;

    std::time_t t = std::time(nullptr);
    auto time = std::localtime(&t);
    info.year = time->tm_year + 1900;
    info.month = time->tm_mon + 1;
    info.day = time->tm_mday;
    info.hour = time->tm_hour;
    info.minute = time->tm_min;
    info.seconds = time->tm_sec;

    write(info);
}

void cProcessingInfoSerializer::write(const sProcessInfo_1_t& info)
{
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

    if (mDataBuffer.overrun())
        throw std::runtime_error("ERROR, Buffer Overrun in writing sProcessInfo_1_t data.");

    writeBlock(*mBlockID, mDataBuffer);
}


