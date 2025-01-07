
#pragma once


#include "RappPointCloud.hpp"
#include <cbdf/PlotPointCloud.hpp>

#include <string>


void exportPointcloud2Ply(const std::string& filename, const cPlotPointCloud& pc, bool useBinaryFormat = true);


