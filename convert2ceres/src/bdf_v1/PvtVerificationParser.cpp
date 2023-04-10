
#include "PvtVerificationParser.hpp"

#include <cassert>

void v1::cPvtVerificationParser::processPositionUnits(v1::cDataBuffer& buffer)
{
    cPvtParser::processPositionUnits(buffer);
    onPositionUnits(mPositionUnit);
}

void v1::cPvtVerificationParser::processVelocityUnits(v1::cDataBuffer& buffer)
{
    cPvtParser::processVelocityUnits(buffer);
    onVelocityUnits(mVelocityUnit);
}

void v1::cPvtVerificationParser::processTimeUnits(v1::cDataBuffer& buffer)
{
    cPvtParser::processTimeUnits(buffer);
    onTimeUnits(mTimeUnit);
}

void v1::cPvtVerificationParser::processPosition1D(v1::cDataBuffer& buffer)
{
    cPvtParser::processPosition1D(buffer);
    onPosition1D(mX);
}

void v1::cPvtVerificationParser::processPosition2D(v1::cDataBuffer& buffer)
{
    cPvtParser::processPosition2D(buffer);
    onPosition2D(mX, mY);
}

void v1::cPvtVerificationParser::processPosition3D(v1::cDataBuffer& buffer)
{
    cPvtParser::processPosition3D(buffer);
    onPosition3D(mX, mY, mZ);
}

void v1::cPvtVerificationParser::processVelocity1D(v1::cDataBuffer& buffer)
{
    cPvtParser::processVelocity1D(buffer);
    onVelocity1D(mVx);
}

void v1::cPvtVerificationParser::processVelocity2D(v1::cDataBuffer& buffer)
{
    cPvtParser::processVelocity2D(buffer);
    onVelocity2D(mVx, mVy);
}

void v1::cPvtVerificationParser::processVelocity3D(v1::cDataBuffer& buffer)
{
    cPvtParser::processVelocity3D(buffer);
    onVelocity3D(mVx, mVy, mVz);
}

void v1::cPvtVerificationParser::processTimestamp(v1::cDataBuffer& buffer)
{
    cPvtParser::processTimestamp(buffer);
    onTimestamp(mTimeStamp);
}


