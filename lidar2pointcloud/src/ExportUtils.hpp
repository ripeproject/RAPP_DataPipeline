
#pragma once

#include "RappPointCloud.hpp"

#include <string>


void exportPointcloud2Ply(int id, const std::string& filename, const cRappPointCloud& pc, bool useBinaryFormat = true);


