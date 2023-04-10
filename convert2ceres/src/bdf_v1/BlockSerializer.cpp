
#include "BlockSerializer.hpp"
#include "BlockDataFile.hpp"
#include "BlockId.hpp"


v1::cBlockSerializer::cBlockSerializer()
:
    cBlockSerializer(1024)
{}

v1::cBlockSerializer::cBlockSerializer(std::size_t n, v1::cBlockDataFileWriter* pDataFile)
:
    mpDataFile(pDataFile)
{
    mDataBuffer.capacity(n);
}

uint16_t v1::cBlockSerializer::classID() const
{
    return const_cast<v1::cBlockSerializer*>(this)->blockID().classID();
}

void v1::cBlockSerializer::attach(v1::cBlockDataFileWriter* pDataFile)
{
    mpDataFile = pDataFile;
}

v1::cBlockDataFileWriter* v1::cBlockSerializer::detach()
{
    auto pFile = mpDataFile;
    mpDataFile = nullptr;
    return pFile;
}

std::size_t v1::cBlockSerializer::bufferCapacity() const
{
    return mDataBuffer.capacity();
}

void v1::cBlockSerializer::setBufferCapacity(std::size_t n)
{
    mDataBuffer.capacity(n);
}

void v1::cBlockSerializer::setVersion(uint8_t major, uint8_t minor)
{
    blockID().setVersion(major, minor);
}




