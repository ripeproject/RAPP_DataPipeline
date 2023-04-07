
#include "PvtParser.hpp"
#include "PvtDataIdentifiers.hpp"
#include "BlockDataFile.hpp"


#include <cassert>

using namespace v1::pvt;

v1::cPvtParser::cPvtParser()
:
    cBlockParser()
{
    setVersion(1, 0);

    mPositionUnit = pvt::ePOSTION_UNITS::UNKNOWN;
    mVelocityUnit = pvt::eVELOCITY_UNITS::UNKNOWN;
    mTimeUnit = pvt::eTIME_UNITS::UNKNOWN;

    mX = mY = mZ = 0.0;
    mVx = mVy = mVz = 0.0;
    mTimeStamp = 0;
}

v1::cBlockID& v1::cPvtParser::blockID()
{
    return mBlockID;
}

void v1::cPvtParser::processData(BLOCK_MAJOR_VERSION_t major_version,
                             BLOCK_MINOR_VERSION_t minor_version,
                             BLOCK_DATA_ID_t data_id, 
                             cDataBuffer& buffer)
{
    switch (static_cast<pvt::DataID>(data_id))
    {
    case DataID::POSITION_UNIT:
        processPositionUnits(buffer);
        break;
    case DataID::VELOCITY_UNIT:
        processVelocityUnits(buffer);
        break;
    case DataID::TIME_UNIT:
        processTimeUnits(buffer);
        break;
    case DataID::POSITION_1D:
        processPosition1D(buffer);
        break;
    case DataID::POSITION_2D:
        processPosition2D(buffer);
        break;
    case DataID::POSITION_3D:
        processPosition3D(buffer);
        break;
    case DataID::VELOCITY_1D:
        processVelocity1D(buffer);
        break;
    case DataID::VELOCITY_2D:
        processVelocity2D(buffer);
        break;
    case DataID::VELOCITY_3D:
        processVelocity3D(buffer);
        break;
    case DataID::TIMESTAMP:
        processTimestamp(buffer);
        break;
    }
}

void v1::cPvtParser::processPositionUnits(cDataBuffer& buffer)
{
    uint8_t u;
    buffer >> u;
    mPositionUnit = static_cast<pvt::ePOSTION_UNITS>(u);
}

void v1::cPvtParser::processVelocityUnits(cDataBuffer& buffer)
{
    uint8_t u;
    buffer >> u;
    mVelocityUnit = static_cast<pvt::eVELOCITY_UNITS>(u);
}

void v1::cPvtParser::processTimeUnits(cDataBuffer& buffer)
{
    uint8_t u = 0;
    buffer >> u;
    mTimeUnit = static_cast<pvt::eTIME_UNITS>(u);
}

void v1::cPvtParser::processPosition1D(cDataBuffer& buffer)
{
    buffer >> mX;
}

void v1::cPvtParser::processPosition2D(cDataBuffer& buffer)
{
    buffer >> mX;
    buffer >> mY;
}

void v1::cPvtParser::processPosition3D(cDataBuffer& buffer)
{
    buffer >> mX;
    buffer >> mY;
    buffer >> mZ;
}

void v1::cPvtParser::processVelocity1D(cDataBuffer& buffer)
{
    buffer >> mVx;
}

void v1::cPvtParser::processVelocity2D(cDataBuffer& buffer)
{
    buffer >> mVx;
    buffer >> mVy;
}

void v1::cPvtParser::processVelocity3D(cDataBuffer& buffer)
{
    buffer >> mVx;
    buffer >> mVy;
    buffer >> mVz;
}


void v1::cPvtParser::processTimestamp(cDataBuffer& buffer)
{
    buffer >> mTimeStamp;
}

