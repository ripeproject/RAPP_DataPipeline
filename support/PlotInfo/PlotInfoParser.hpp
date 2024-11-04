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


	virtual void onBeginPlotInfoList() = 0;
	virtual void onEndPlotInfoList() = 0;

	virtual void onBeginPlotInfoBlock() = 0;
	virtual void onEndPlotInfoBlock() = 0;

	virtual void onPlotID(int id) = 0;
	virtual void onSubPlotID(int id) = 0;
	virtual void onName(const std::string& name) = 0;
	virtual void onDescription(const std::string& description) = 0;

	virtual void onSpecies(const std::string& species) = 0;
	virtual void onCultivar(const std::string& cultivar) = 0;

	virtual void onTreatment(const std::string& treatment) = 0;

	virtual void onConstructName(const std::string& name) = 0;
	virtual void onEvent(const std::string& event) = 0;

	virtual void onPotLabel(const std::string& pot_label) = 0;
	virtual void onSeedGeneration(const std::string& seed_generation) = 0;
	virtual void onCopyNumber(const std::string& copy_number) = 0;

	virtual void onPlotDimensions(double x_min_m, double x_max_m,
		double y_min_m, double y_max_m,
		double z_min_m, double z_max_m) = 0;

	virtual void onPlotPointCloudData(cPointCloud pointCloud) = 0;
	virtual void onPlotPointCloudData(cPointCloud_FrameId pointCloud) = 0;
	virtual void onPlotPointCloudData(cPointCloud_SensorInfo pointCloud) = 0;

protected:
	void processData(BLOCK_MAJOR_VERSION_t major_version,
		             BLOCK_MINOR_VERSION_t minor_version, 
		             BLOCK_DATA_ID_t data_id, 
		             cDataBuffer& buffer) override;

protected:
	virtual void processBeginPlotInfoList(cDataBuffer& buffer);
	virtual void processEndPlotInfoList(cDataBuffer& buffer);

	virtual void processBeginPlotInfoBlock(cDataBuffer& buffer);
	virtual void processEndPlotInfoBlock(cDataBuffer& buffer);

	virtual void processPlotID(cDataBuffer& buffer);
	virtual void processSubPlotID(cDataBuffer& buffer);
	virtual void processName(cDataBuffer& buffer);
	virtual void processDescription(cDataBuffer& buffer);

	virtual void processSpecies(cDataBuffer& buffer);
	virtual void processCultivar(cDataBuffer& buffer);

	virtual void processTreatment(cDataBuffer& buffer);

	virtual void processConstructName(cDataBuffer& buffer);
	virtual void processEvent(cDataBuffer& buffer);

	virtual void processPotLabel(cDataBuffer& buffer);
	virtual void processSeedGeneration(cDataBuffer& buffer);
	virtual void processCopyNumber(cDataBuffer& buffer);

	void processDimensions(cDataBuffer& buffer);

	void processPointCloudData(cDataBuffer& buffer);
	void processPointCloudData_FrameId(cDataBuffer& buffer);
	void processPointCloudData_SensorInfo(cDataBuffer& buffer);

private:
	std::unique_ptr<cPlotInfoID> mBlockID;
};

