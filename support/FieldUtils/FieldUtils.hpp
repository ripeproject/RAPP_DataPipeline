
#pragma once

#include "datatypes.hpp"
#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include <string>
#include <vector>


std::vector<cRappTriangle> computeGroundMesh(const std::vector<rfm::rappPoint_t>& ground_points);

struct sLineParameters_t
{
    double m = 0.0;
    double b = 0.0;
};

sLineParameters_t fitHorizontalLine(const std::vector<rfm::rappPoint_t>& points);

sLineParameters_t fitVerticalLine(const std::vector<rfm::rappPoint_t>& points);

sLineParameters_t fitLine(const std::vector<rfm::rappPoint_t>& points, const std::vector<rfm::rappPoint_t>& line);


