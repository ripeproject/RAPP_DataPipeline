
#pragma once

#include "RappPointCloud.hpp"

#include <string>


void exportPointcloud2Ply(int id, const std::string& filename, const cRappPointCloud& pc, bool useBinaryFormat = true,
	double max_angle = 5.0, double threshold_pct = 0.1);


