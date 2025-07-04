

#include "PlotConfigCorrections.hpp"

#include <stdexcept>


cPlotConfigCorrection::cPlotConfigCorrection(int month, int day)
	: mEffectiveMonth(month), mEffectiveDay(day)
{
}

const int cPlotConfigCorrection::date() const { return mEffectiveMonth * 100 + mEffectiveDay; }
const int cPlotConfigCorrection::month() const { return mEffectiveMonth; }
const int cPlotConfigCorrection::day() const { return mEffectiveDay; }

void cPlotConfigCorrection::clear()
{
	mBounds.clear();
	mIsolationMethod.clear();
}

bool cPlotConfigCorrection::isDirty() const
{
	return mBounds.isDirty() || mIsolationMethod.isDirty();
}

bool cPlotConfigCorrection::empty() const
{
	return mBounds.empty();
}

bool cPlotConfigCorrection::contains(rfm::rappPoint2D_t point) const
{
	return mBounds.contains(point);
}

bool cPlotConfigCorrection::contains(std::int32_t x_mm, std::int32_t y_mm) const
{
	return mBounds.contains(x_mm, y_mm);
}

const cPlotConfigBoundary& cPlotConfigCorrection::getBounds() const
{
	return mBounds;
}

cPlotConfigBoundary& cPlotConfigCorrection::getBounds()
{
	return mBounds;
}

const cPlotConfigIsolationMethod& cPlotConfigCorrection::getIsolationMethod() const
{
	return mIsolationMethod;
}

cPlotConfigIsolationMethod& cPlotConfigCorrection::getIsolationMethod()
{
	return mIsolationMethod;
}


/**********************************************************
 * The collection of plot corrections
 **********************************************************/

cPlotConfigCorrections::cPlotConfigCorrections()
{}

cPlotConfigCorrections::~cPlotConfigCorrections()
{}

bool cPlotConfigCorrections::empty() const
{
	return mCorrections.empty();
}

bool cPlotConfigCorrections::isDirty() const
{
	return mDirty;
}

bool cPlotConfigCorrections::contains(rfm::rappPoint2D_t point) const
{
	for (const auto& entry : mCorrections)
	{
		const auto& correction = entry.second;

		if (correction.contains(point))
			return true;
	}

	return false;
}

bool cPlotConfigCorrections::contains(std::int32_t x_mm, std::int32_t y_mm) const
{
	for (const auto& entry : mCorrections)
	{
		const auto& correction = entry.second;

		if (correction.contains(x_mm, y_mm))
			return true;
	}

	return false;
}

const cPlotConfigBoundary& cPlotConfigCorrections::getBounds(int month, int day) const
{
	auto it = find(month, day);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getBounds();
}

cPlotConfigBoundary& cPlotConfigCorrections::getBounds(int month, int day)
{
	auto it = find(month, day);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getBounds();
}

const cPlotConfigIsolationMethod& cPlotConfigCorrections::getIsolationMethod(int month, int day) const
{
	auto it = find(month, day);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getIsolationMethod();
}

cPlotConfigIsolationMethod& cPlotConfigCorrections::getIsolationMethod(int month, int day)
{
	auto it = find(month, day);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getIsolationMethod();
}

void cPlotConfigCorrections::clearDirtyFlag()
{
	mDirty = false;
}

void cPlotConfigCorrections::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
}

cPlotConfigCorrections::PlotCorrections_t::const_iterator cPlotConfigCorrections::find(int month, int day) const
{
	int date = month * 100 + day;

	PlotCorrections_t::const_iterator result = mCorrections.end();

	for (auto it = mCorrections.begin(); it != mCorrections.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cPlotConfigCorrections::PlotCorrections_t::iterator cPlotConfigCorrections::find(int month, int day)
{
	int date = month * 100 + day;

	PlotCorrections_t::iterator result = mCorrections.end();

	for (auto it = mCorrections.begin(); it != mCorrections.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

void cPlotConfigCorrections::load(const nlohmann::json& jdoc)
{
}

nlohmann::json cPlotConfigCorrections::save()
{
	nlohmann::json catalogDoc;
	return catalogDoc;
}


