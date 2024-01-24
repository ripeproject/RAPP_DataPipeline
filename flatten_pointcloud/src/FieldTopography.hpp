
#pragma once

#include "datatypes.hpp"
#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include "RappPointCloud.hpp"

#include <string>
#include <vector>

// Forward Declarations


void test(const std::vector<rfm::rappPoint_t>& ground_points);

std::vector<cRappTriangle> computeGroundMesh(const std::vector<rfm::rappPoint_t>& ground_points);

struct sPitchAndRoll_t
{
	double pitch_deg = 0;
	double roll_deg = 0;
	double R = 0;
};

sPitchAndRoll_t fitPointCloudToGroundMesh(const cRappPointCloud& pc);

struct sPitch_t
{
	double pitch_deg = 0;
	double R = 0;
};

sPitch_t fitPointCloudPitchToGroundMesh_deg(const cRappPointCloud& pc);

struct sRoll_t
{
	double roll_deg = 0;
	double R = 0;
};

sRoll_t fitPointCloudRollToGroundMesh_deg(const cRappPointCloud& pc);

double computePcToGroundMeshDistance_mm(const cRappPointCloud& pc);
double computePcToGroundMeshDistance_mm(const cRappPointCloud& pc, double threashold_pct);

sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc);
sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc, double threashold_pct);
