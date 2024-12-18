#pragma once

#include "PointCloudTypes.hpp"
#include "PointCloudInfo.hpp"
#include "PlotInfo.hpp"
#include "PlotInfoParser.hpp"

#include <memory>


class cPlotInfoLoader : public cPlotInfoParser
{
public:
	explicit cPlotInfoLoader(std::weak_ptr<cPlotInfo> info);
	virtual ~cPlotInfoLoader();

protected:

	void onBeginPlotInfoList() override;
	void onEndPlotInfoList() override;

	void onBeginPlotInfoBlock() override;
	void onEndPlotInfoBlock() override;

	void onPlotID(int id) override;
	void onSubPlotID(int id) override;
	void onName(const std::string& name) override;
	void onDescription(const std::string& description) override;

	void onSpecies(const std::string& species) override;
	void onCultivar(const std::string& cultivar) override;

	void onTreatment(const std::string& treatment) override;

	void onConstructName(const std::string& name) override;
	void onEvent(const std::string& event) override;

	void onPotLabel(const std::string& pot_label) override;
	void onSeedGeneration(const std::string& seed_generation) override;
	void onCopyNumber(const std::string& copy_number) override;
	void onLeafType(const std::string& leaf_type) override;

	void onVegetationOnly(const bool vegetation_only)  override;

	void onPlotDimensions(double x_min_m, double x_max_m,
						double y_min_m, double y_max_m,
						double z_min_m, double z_max_m) override;

	void onPlotPointCloudData(cPointCloud pointCloud) override;
	void onPlotPointCloudData(cPointCloud_FrameId pointCloud) override;
	void onPlotPointCloudData(cPointCloud_SensorInfo pointCloud) override;


private:
	std::shared_ptr<cPlotInfo> mInfo;

	bool mHasListTag = false;
	bool mHasBlockTag = false;

	std::unique_ptr<cPlot> mPlot;
};

