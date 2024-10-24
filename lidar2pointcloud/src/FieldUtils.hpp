
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


struct sOffsetInterpTable_t
{
	std::vector<rfm::sPointCloudTranslationInterpPoint_t> offset_m;
	double R = 0;
	bool valid = false;
};

sOffsetInterpTable_t fitPointCloudHeightTable(std::vector<rfm::sPointCloudTranslationInterpPoint_t> heights);
sOffsetInterpTable_t computePointCloudHeightTable(std::vector<rfm::sPointCloudTranslationInterpPoint_t> heights, uint16_t steps);


struct sAnglesInterpTable_t
{
	std::vector<rfm::sPointCloudRotationInterpPoint_t> angles_deg;
	double R = 0;
	bool valid = false;
};

sAnglesInterpTable_t fitPointCloudAngleTable(std::vector<rfm::sPointCloudRotationInterpPoint_t> angles, uint16_t steps);
sAnglesInterpTable_t computePointCloudAngleTable(std::vector<rfm::sPointCloudRotationInterpPoint_t> angles, uint16_t steps);


struct sPitchAndRoll_t
{
	double pitch_deg = 0;
	double roll_deg = 0;
	double R = 0;
	bool valid = false;
};

sPitchAndRoll_t fitPointCloudPitchRollToGroundMesh(const cRappPointCloud& pc);


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


struct sOffset_t
{
	double offset_mm = 0;
	bool valid = false;
};

sOffset_t computePcToGroundMeshDistance_mm(const cRappPointCloud& pc);
sOffset_t computePcToGroundMeshDistance_mm(const cRappPointCloud& pc, double threshold_pct);

sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc);
sPitchAndRoll_t computePcToGroundMeshRotation_deg(const cRappPointCloud& pc, double threshold_pct);

sOffset_t computePcToGroundMeshDistanceUsingGrid_mm(const cRappPointCloud& pc, double threshold_pct);
sPitchAndRoll_t computePcToGroundMeshRotationUsingGrid_deg(const cRappPointCloud& pc, double threshold_pct);
sPitchAndRoll_t computePcToGroundMeshRotationUsingGrid_deg(const cRappPointCloud::vCloud_t& pc, 
	const double minX_mm, const double maxX_mm, const double minY_mm, const double maxY_mm, double threshold_pct);

void shiftPointCloudToAGL(int id, cRappPointCloud& pc);
void shiftPointCloudToAGL(int id, cRappPointCloud::vCloud_t& pc);

void rotate(cRappPointCloud::vCloud_t& pc, double yaw_deg, double pitch_deg, double roll_deg);
void translate(cRappPointCloud::vCloud_t& pc, int dx_mm, int dy_mm, int dz_mm);

