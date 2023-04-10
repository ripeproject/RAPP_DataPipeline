
#include "BlockParser.hpp"
#include "BlockDataFile.hpp"
#include "BlockId.hpp"


v1::cBlockParser::cBlockParser()
{}

void v1::cBlockParser::setVersion(uint8_t major, uint8_t minor)
{
    blockID().setVersion(major, minor);
}

v1::cBlockID v1::cBlockParser::blockID() const
{
    return const_cast<cBlockParser*>(this)->blockID();
}



