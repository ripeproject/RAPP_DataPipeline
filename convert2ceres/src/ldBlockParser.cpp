
#include "ldBlockParser.hpp"
#include "ldBlockDataFile.hpp"
#include "ldBlockId.hpp"


lidar_data::cBlockParser::cBlockParser()
{}

void lidar_data::cBlockParser::setVersion(uint8_t major, uint8_t minor)
{
    blockID().setVersion(major, minor);
}

lidar_data::cBlockID lidar_data::cBlockParser::blockID() const
{
    return const_cast<cBlockParser*>(this)->blockID();
}



