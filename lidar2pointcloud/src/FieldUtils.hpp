
#pragma once

#include "datatypes.hpp"
#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include "RappPointCloud.hpp"

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

struct sOffset_t
{
	double offset_mm = 0;
	bool valid = false;
};

struct sPitchAndRoll_t
{
	double pitch_deg = 0;
	double roll_deg = 0;
	double R = 0;
	bool valid = false;
};

sPitchAndRoll_t fitPointCloudToGroundMesh(const cRappPointCloud& pc);

struct sPitch_t
{
	double pitch_deg = 0;
	double R = 0;
	bool valid = false;
};

sPitch_t fitPointCloudPitchToGroundMesh_deg(const cRappPointCloud& pc);

struct sRoll_t
{
	double roll_deg = 0;
	double R = 0;
	bool valid = false;
};

sRoll_t fitPointCloudRollToGroundMesh_deg(const cRappPointCloud& pc);


sOffset_t computePcToGroundMeshDistance_mm(const cRappPointCloud& pc);
sOffset_t computePcToGroundMeshDistance_mm(const cRappPointCloud& pc, double threshold_pct);

sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc);
sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc, double threshold_pct);

sOffset_t computePcToGroundMeshDistanceUsingGrid_mm(const cRappPointCloud& pc, double threshold_pct);
sPitchAndRoll_t computePcToGroundMeshRotationUsingGrid_deg(const cRappPointCloud& pc, double threshold_pct);

void shiftPointCloudToAGL(cRappPointCloud& pc);
void shiftPointCloudToAGL(cRappPointCloud::vCloud_t& pc);

