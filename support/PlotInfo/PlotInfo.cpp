
#include "PlotInfo.hpp"


cPlot::cPlot(int plotId) : cPlotMetaData(plotId), cPlotPointCloud()
{}

void cPlot::assign(const cBasePointCloud<pointcloud::sCloudPoint_t>& pc)
{
	cPlotPointCloud::assign(pc);
}

void cPlot::assign(const cBasePointCloud<pointcloud::sCloudPoint_FrameID_t>& pc)
{
	cPlotPointCloud::assign(pc);
}

void cPlot::assign(const cBasePointCloud<pointcloud::sCloudPoint_SensorInfo_t>& pc)
{
	cPlotPointCloud::assign(pc);
}

cPlotInfo::cPlotInfo()
{}

cPlotInfo::~cPlotInfo()
{}

bool cPlotInfo::empty() const
{
	return mPlots.empty();
}

std::size_t cPlotInfo::size() const
{
	return mPlots.size();
}

void cPlotInfo::addPlot(std::unique_ptr<cPlot> plot)
{
	mPlots.emplace_back(std::move(plot));
}




