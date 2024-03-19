
#include "GroundModelUtils.hpp"
#include "GroundDataModel.hpp"
#include "FieldUtils.hpp"
#include "GpsFileReader.hpp"

#include "Constants.hpp"


namespace
{
	cGroundDataModel gData;
}

void clear_ground_data()
{
	gData.clearGroundPoints();
	gData.clearGroundMesh();
}

bool load_ground_data(const std::string& ground_data_filename)
{
	cGpsFileReader gps;
	gps.loadFromFile(ground_data_filename);

	auto rapp_points = gps.GetRappPoints();

	if (rapp_points.empty())
		return false;

	gData.addGroundPoints(rapp_points);

	auto data = gData.getGroundPoints();

	if (data.empty())
		return false;

	auto mesh = computeGroundMesh(data);

	if (mesh.empty())
		return false;

	gData.clearGroundMesh();
	gData.addMeshData(mesh);

	return true;
}

double getMeshHeight_mm(std::int32_t x_mm, std::int32_t y_mm)
{
	return gData.getMeshHeight_mm(x_mm, y_mm);
}
