
#pragma once

#include "datatypes.hpp"
#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include "RappPointCloud.hpp"

#include <string>
#include <vector>
#include <map>

// Forward Declarations

// Load the GPS ground point into the data model and create ground mesh
void clear_ground_data();
bool load_ground_data(const std::string& ground_data_filename);

double getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm);
