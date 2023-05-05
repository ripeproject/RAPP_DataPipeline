
#include "ProcessingInfoParser.hpp"
#include "ProcessingInfoBlockId.hpp"
#include "ProcessingInfoDataIdentifiers.hpp"
#include <cbdf/BlockDataFile.hpp>

#include <stdexcept>

using namespace processing_info;


cProcessingInfoParser::cProcessingInfoParser()
:
    cBlockParser(), mBlockID(new cProcessingInfoID())
{}

cProcessingInfoParser::~cProcessingInfoParser() {}

cBlockID& cProcessingInfoParser::blockID()
{
    return *mBlockID;
}

void cProcessingInfoParser::processData(BLOCK_MAJOR_VERSION_t major_version,
                                BLOCK_MINOR_VERSION_t minor_version,
                                BLOCK_DATA_ID_t data_id,
                                cDataBuffer& buffer)
{
    mBlockID->setVersion(major_version, minor_version);
    mBlockID->dataID(static_cast<processing_info::DataID>(data_id));

    switch (static_cast<processing_info::DataID>(data_id))
    {
    case DataID::PROCESSING_INFO:
        processProcessingInfo(buffer);
        break;
    }
}

void cProcessingInfoParser::processProcessingInfo(cDataBuffer& buffer)
{
    sProcessInfo_1_t info;

    buffer >> info.name;
    info.type = static_cast<ePROCESSING_TYPE>(buffer.get<uint16_t>());

    buffer >> info.year;
    buffer >> info.month;
    buffer >> info.day;
    buffer >> info.hour;
    buffer >> info.minute;
    buffer >> info.seconds;

    if (buffer.underrun())
        throw std::runtime_error("ERROR, Buffer under run in processProcessingInfo.");

    onProcessingInfo(info);
}

