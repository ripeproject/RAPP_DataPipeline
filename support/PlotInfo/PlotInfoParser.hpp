/**
 * @file
 * @brief Parser for reading the "plot info" data from a block based data file.
 * 
 * Plot Information is general information on what processing steps have been 
 * applied to the block based data file.
 */
#pragma once

#include "PointCloud.hpp"

#include <cbdf/BlockParser.hpp>

#include <memory>

 // Forward Declarations
class cPlotInfoID;


class cPlotInfoParser : public cBlockParser
{
public:
	cPlotInfoParser();
	virtual ~cPlotInfoParser();

	cBlockID& blockID() override;

	virtual void onPlotID(int id) = 0;
	virtual void onPlotName(const std::string& name) = 0;

	virtual void onDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m,
		double z_min_m, double z_max_m) = 0;

	virtual void onPointCloudData(cPointCloud pointCloud) = 0;

	virtual void onEvent(const std::string& event) = 0;
	virtual void onEventDescription(const std::string& description) = 0;

protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	virtual void processPlotID(cDataBuffer& buffer);
	virtual void processPlotName(cDataBuffer& buffer);

	void processDimensions(cDataBuffer& buffer);

	void processPointCloudData_1_0(cDataBuffer& buffer);
	void processPointCloudData_1_1(cDataBuffer& buffer);

	virtual void processEvent(cDataBuffer& buffer);
	virtual void processEventDescription(cDataBuffer& buffer);

private:
	std::unique_ptr<cPlotInfoID> mBlockID;
};

