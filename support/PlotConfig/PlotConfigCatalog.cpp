
#include "PlotConfigCatalog.hpp"

#include <stdexcept>


namespace
{
	const uint32_t HAS_SUB_PLOT_MASK = 0x8000'0000;
	const uint32_t SUB_PLOT_FLAG = 0x8000'0000;

	const uint32_t PLOT_MASK = 0x7FFF'FF00;

	const uint32_t SUB_PLOT_MASK = 0x0000'00FF;
}

cPlotConfigCatalog::cPlotConfigCatalog()
{}

cPlotConfigCatalog::~cPlotConfigCatalog()
{
}

bool cPlotConfigCatalog::isDirty() const
{
	return mDirty;
}

void cPlotConfigCatalog::clearDirtyFlag()
{
	mDirty = false;
}

void cPlotConfigCatalog::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
}

void cPlotConfigCatalog::load(const nlohmann::json& jdoc)
{
}

nlohmann::json cPlotConfigCatalog::save()
{
	nlohmann::json catalogDoc;
	return catalogDoc;
}


