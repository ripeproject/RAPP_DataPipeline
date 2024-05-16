/**
* @file
* @brief Serializer for storing "plot info" data into a block based data file
*
* @usage
*
* Saving a single plot info record should contain the following calls :
*
*	writeBeginPlotInfoBlock
*
* write pointcloud::eCOORDINATE_SYSTEM
*
* writeDistanceWindow;
*writeAzimuthWindow;
*writeAltitudeWindow;
*
*write pointcloud::eKINEMATIC_MODEL
*
* writeSensorKinematics
* or
*writeBeginSensorKinematics
* writeSensorKinematicInfo
* writeSensorKinematicInfo
*				...
*			writeEndSensorKinematics
*
*writeDimensions
*
*write cPointCloud or write cPointCloud_FrameId or write cPointCloud_SensorInfo
*
*	writeEndPlotInfoBlock
*
*
*Saving a list of plot info records should contain the following calls :
*
*	writeBeginPlotInfoList
*		Plot Info Block(as above)
*		Plot Info Block
*				...
*	writeEndPlotInfoList
*/

#pragma once

#include "PointCloud.hpp"

#include <cbdf/BlockSerializer.hpp>

#include <memory>


// Forward Declarations
class cPlotInfoID;


class cPlotInfoSerializer : public cBlockSerializer
{
public:
	cPlotInfoSerializer();
	explicit cPlotInfoSerializer(std::size_t n, cBlockDataFileWriter* pDataFile = nullptr);
	virtual ~cPlotInfoSerializer();

	void writeBeginPlotInfoList();
	void writeEndPlotInfoList();

	void writeBeginPlotInfoBlock();
	void writeEndPlotInfoBlock();

	void writeID(int id);
	void writeID(int id, int sub_id);
	void writeName(const std::string& name);
	void writeDescription(const std::string& description);

	void writeSpecies(const std::string& species);
	void writeCultivar(const std::string& cultivar);

	void writeTreatment(const std::string& treatment);
	void writeTreatments(const std::vector<std::string>& treatments);

	void writeConstructName(const std::string& name);
	void writeEvent(const std::string& event);

	void writeDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m, double z_min_m, double z_max_m);

	void write(const cPointCloud& in);
	void write(const cPointCloud_FrameId& in);
	void write(const cPointCloud_SensorInfo& in);

	//
	// These helper functions are not normally needed as they are call by
	// writeTreatments.
	//
	void writeBeginTreatments();
	void writeEndOfTreatments();

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cPlotInfoID> mBlockID;
};

