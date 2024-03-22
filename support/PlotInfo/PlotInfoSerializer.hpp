/**
 * @file
 * @brief Serializer for storing "procesing info" data into a block based data file
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

	void writeID(int id);
	void writeName(const std::string& name);
	void writeDescription(const std::string& description);

	void writeEvent(const std::string& event);

	void writeDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m, double z_min_m, double z_max_m);

	void write(const cPointCloud& in);
	void write(const cPointCloud_FrameId& in);
	void write(const cPointCloud_SensorInfo& in);

protected:
	cBlockID& blockID() override;

private:
	std::unique_ptr<cPlotInfoID> mBlockID;
};

