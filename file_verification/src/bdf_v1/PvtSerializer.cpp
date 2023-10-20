
#include "PvtSerializer.hpp"
#include "PvtDataIdentifiers.hpp"
#include "BlockDataFile.hpp"


#include <cassert>

using namespace v1::pvt;

v1::cPvtSerializer::cPvtSerializer()
:
    cBlockSerializer()
{
    setVersion(1, 0);
}

v1::cPvtSerializer::cPvtSerializer(std::size_t n, cBlockDataFileWriter* pDataFile)
:
    cBlockSerializer(n, pDataFile)
{
    setVersion(1, 0);
}

v1::cBlockID& v1::cPvtSerializer::blockID()
{
    return mBlockID;
}

void v1::cPvtSerializer::write(ePOSTION_UNITS unit)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::POSITION_UNIT);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(unit);

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write(eVELOCITY_UNITS unit)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::VELOCITY_UNIT);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(unit);

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write(eTIME_UNITS unit)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::TIME_UNIT);

    mDataBuffer.clear();
    mDataBuffer << static_cast<uint8_t>(unit);

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_position(double x)
{
    assert(mpDataFile);

	mBlockID.dataID(DataID::POSITION_1D);

    mDataBuffer.clear();
    mDataBuffer << x;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_position(double x, double y)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::POSITION_2D);

    mDataBuffer.clear();
    mDataBuffer << x;
    mDataBuffer << y;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_position(double x, double y, double z)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::POSITION_3D);

    mDataBuffer.clear();
    mDataBuffer << x;
    mDataBuffer << y;
    mDataBuffer << z;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_velocity(double x)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::VELOCITY_1D);

    mDataBuffer.clear();
    mDataBuffer << x;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_velocity(double x, double y)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::VELOCITY_2D);

    mDataBuffer.clear();
    mDataBuffer << x;
    mDataBuffer << y;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_velocity(double x, double y, double z)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::VELOCITY_3D);

    mDataBuffer.clear();
    mDataBuffer << x;
    mDataBuffer << y;
    mDataBuffer << z;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

void v1::cPvtSerializer::write_timestamp(std::uint64_t t)
{
    assert(mpDataFile);

    mBlockID.dataID(DataID::TIMESTAMP);

    mDataBuffer.clear();
    mDataBuffer << t;

    mpDataFile->writeBlock(mBlockID, mDataBuffer.data(), mDataBuffer.size());
}

