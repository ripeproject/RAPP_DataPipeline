
#pragma once

#include "datatypes.hpp"
#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include <string>
#include <vector>


std::vector<cRappTriangle> computeDelaunay2dMesh(const std::vector<rfm::rappPoint_t>& points, double max_length_mm = 1'000'000.0);

struct sLineParameters_t
{
    double m = 0.0;
    double b = 0.0;
};

sLineParameters_t fitHorizontalLine(const std::vector<rfm::rappPoint_t>& points);

sLineParameters_t fitVerticalLine(const std::vector<rfm::rappPoint_t>& points);

sLineParameters_t fitLine(const std::vector<rfm::rappPoint_t>& points, const std::vector<rfm::rappPoint_t>& line);


