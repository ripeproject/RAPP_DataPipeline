
#include "PlotInfoLoader.hpp"
#include "PlotInfo.hpp"
#include "PointCloud.hpp"


cPlotInfoLoader::cPlotInfoLoader(std::weak_ptr<cPlotInfo> info)
{
    mInfo = info.lock();
}

cPlotInfoLoader::~cPlotInfoLoader()
{}

void cPlotInfoLoader::onBeginPlotInfoList()
{
	mHasListTag = true;
}

void cPlotInfoLoader::onEndPlotInfoList()
{
	mHasListTag = false;
}

void cPlotInfoLoader::onBeginPlotInfoBlock()
{
	mHasBlockTag = true;
}

void cPlotInfoLoader::onEndPlotInfoBlock()
{
	mHasBlockTag = false;
}

void cPlotInfoLoader::onPlotID(int id)
{
	mPlot = std::make_unique<cPlot>(id);
}

void cPlotInfoLoader::onSubPlotID(int id)
{
	id;
}

void cPlotInfoLoader::onName(const std::string& name)
{
	if (mPlot)
		mPlot->setPlotName(name);
}

void cPlotInfoLoader::onDescription(const std::string& description)
{
	if (mPlot)
		mPlot->setDescription(description);
}

void cPlotInfoLoader::onSpecies(const std::string& species)
{
	if (mPlot)
		mPlot->setSpecies(species);
}

void cPlotInfoLoader::onCultivar(const std::string& cultivar)
{
	if (mPlot)
		mPlot->setCultivar(cultivar);
}

void cPlotInfoLoader::onTreatment(const std::string& treatment)
{
	if (mPlot)
		mPlot->addTreatment(treatment);
}

void cPlotInfoLoader::onConstructName(const std::string& name)
{
	if (mPlot)
		mPlot->setConstructName(name);
}

void cPlotInfoLoader::onEvent(const std::string& event)
{
	if (mPlot)
		mPlot->setEvent(event);
}

void cPlotInfoLoader::onPotLabel(const std::string& pot_label)
{
	if (mPlot)
		mPlot->setPotLabel(pot_label);
}

void cPlotInfoLoader::onSeedGeneration(const std::string& seed_generation)
{
	if (mPlot)
		mPlot->setSeedGeneration(seed_generation);
}

void cPlotInfoLoader::onCopyNumber(const std::string& copy_number)
{
	if (mPlot)
		mPlot->setCopyNumber(copy_number);
}

void cPlotInfoLoader::onPlotDimensions(double x_min_m, double x_max_m,
	double y_min_m, double y_max_m,
	double z_min_m, double z_max_m)
{
	x_min_m;
}

void cPlotInfoLoader::onPlotPointCloudData(cPointCloud pointCloud)
{
	if (mPlot)
	{
		mPlot->assign(pointCloud);

		mInfo->addPlot(std::move(mPlot));
	}
}

void cPlotInfoLoader::onPlotPointCloudData(cPointCloud_FrameId pointCloud)
{
	if (mPlot)
	{
		mPlot->assign(pointCloud);

		mInfo->addPlot(std::move(mPlot));
	}
}

void cPlotInfoLoader::onPlotPointCloudData(cPointCloud_SensorInfo pointCloud)
{
	if (mPlot)
	{
		mPlot->assign(pointCloud);

		mInfo->addPlot(std::move(mPlot));
	}
}


