

#include "PlotConfigCorrections.hpp"

#include <stdexcept>


namespace plot_config
{
	int to_date(int month, int day)
	{
		return month * 100 + day;
	}
}

cPlotConfigCorrection::cPlotConfigCorrection(int month, int day)
	: mEffectiveMonth(month), mEffectiveDay(day)
{
}

const int cPlotConfigCorrection::date() const { return plot_config::to_date(mEffectiveMonth, mEffectiveDay); }
const int cPlotConfigCorrection::month() const { return mEffectiveMonth; }
const int cPlotConfigCorrection::day() const { return mEffectiveDay; }

void cPlotConfigCorrection::clear()
{
	mBounds.clear();
	mIsolationMethod.clear();
}

bool cPlotConfigCorrection::isDirty() const
{
	for (const auto& exclusion : mExclusions)
	{
		if (exclusion.isDirty())
			return true;
	}

	return mBounds.isDirty() || mIsolationMethod.isDirty();
}

bool cPlotConfigCorrection::empty() const
{
	return mBounds.empty();
}

bool cPlotConfigCorrection::contains_point(rfm::rappPoint2D_t point) const
{
	return mBounds.contains(point);
}

bool cPlotConfigCorrection::contains_point(std::int32_t x_mm, std::int32_t y_mm) const
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

void cPlotConfigCorrection::setBounds(const cPlotConfigBoundary& bounds)
{
	mBounds = bounds;
}

void cPlotConfigCorrection::setIsolationMethod(const cPlotConfigIsolationMethod& method)
{
	mIsolationMethod = method;
}

void cPlotConfigCorrection::clearDirtyFlag()
{
	mBounds.clearDirtyFlag();
	mIsolationMethod.clearDirtyFlag();

	for (auto& exclusion : mExclusions)
	{
		exclusion.setDirtyFlag(false);
	}
}

void cPlotConfigCorrection::setDirtyFlag(bool dirty)
{
	mBounds.setDirtyFlag(dirty);
	mIsolationMethod.setDirtyFlag(dirty);

	for (auto& exclusion : mExclusions)
	{
		exclusion.setDirtyFlag(dirty);
	}
}

void cPlotConfigCorrection::load(const nlohmann::json& jdoc)
{
	mBounds.load(jdoc);
	mIsolationMethod.load(jdoc);

	if (jdoc.contains("exclusions"))
	{
		auto& exclusions = jdoc["exclusions"];
		for (auto& exclusion : exclusions)
		{
			cPlotConfigExclusion e;
			e.load(exclusion);
			mExclusions.push_back(e);
		}
	}
}

nlohmann::json cPlotConfigCorrection::save()
{
	nlohmann::json correctionDoc;

	std::string date = std::to_string(mEffectiveMonth);
	date += "/";
	date += std::to_string(mEffectiveDay);

	correctionDoc["date"] = date;

	mBounds.save(correctionDoc);
	correctionDoc["isolation method"] = mIsolationMethod.save();

	if (!mExclusions.empty())
	{
		nlohmann::json exclusionDoc;

		for (auto& exclusion : mExclusions)
		{
			exclusionDoc.push_back(exclusion.save());
		}

		if (!exclusionDoc.is_null())
			correctionDoc["exclusions"] = exclusionDoc;
	}

	return correctionDoc;
}


/**********************************************************
 * The collection of plot corrections
 **********************************************************/

cPlotConfigCorrections::cPlotConfigCorrections()
{}

cPlotConfigCorrections::~cPlotConfigCorrections()
{}

void cPlotConfigCorrections::clear()
{
	mCorrections.clear();
}

bool cPlotConfigCorrections::empty() const
{
	return mCorrections.empty();
}

bool cPlotConfigCorrections::isDirty() const
{
	for (const auto& correction : mCorrections)
	{
		if (correction.second.isDirty())
			return true;
	}
	
	return mDirty;
}

bool cPlotConfigCorrections::contains_point(rfm::rappPoint2D_t point) const
{
	for (const auto& entry : mCorrections)
	{
		const auto& correction = entry.second;

		if (correction.contains_point(point))
			return true;
	}

	return false;
}

bool cPlotConfigCorrections::contains_point(std::int32_t x_mm, std::int32_t y_mm) const
{
	for (const auto& entry : mCorrections)
	{
		const auto& correction = entry.second;

		if (correction.contains_point(x_mm, y_mm))
			return true;
	}

	return false;
}

bool cPlotConfigCorrections::contains(const int date) const
{
	return mCorrections.contains(date);
}

bool cPlotConfigCorrections::contains(const int month, const int day) const
{
	return contains(plot_config::to_date(month, day));
}

const cPlotConfigCorrection& cPlotConfigCorrections::front() const
{
	return mCorrections.begin()->second;
}

cPlotConfigCorrection& cPlotConfigCorrections::front()
{
	return mCorrections.begin()->second;
}

const cPlotConfigBoundary& cPlotConfigCorrections::getBounds(int date) const
{
	auto it = find(date);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getBounds();
}

cPlotConfigBoundary& cPlotConfigCorrections::getBounds(int date)
{
	auto it = find(date);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getBounds();
}

const cPlotConfigBoundary& cPlotConfigCorrections::getBounds(int month, int day) const
{
	return getBounds(plot_config::to_date(month, day));
}

cPlotConfigBoundary& cPlotConfigCorrections::getBounds(int month, int day)
{
	return getBounds(plot_config::to_date(month, day));
}

const cPlotConfigIsolationMethod& cPlotConfigCorrections::getIsolationMethod(int date) const
{
	auto it = find(date);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getIsolationMethod();
}

cPlotConfigIsolationMethod& cPlotConfigCorrections::getIsolationMethod(int date)
{
	auto it = find(date);
	if (it == mCorrections.end())
		throw std::logic_error("oops");

	return it->second.getIsolationMethod();
}

const cPlotConfigIsolationMethod& cPlotConfigCorrections::getIsolationMethod(int month, int day) const
{
	return getIsolationMethod(plot_config::to_date(month, day));
}

cPlotConfigIsolationMethod& cPlotConfigCorrections::getIsolationMethod(int month, int day)
{
	return getIsolationMethod(plot_config::to_date(month, day));
}

void cPlotConfigCorrections::clearDirtyFlag()
{
	for (auto& correction : mCorrections)
	{
		correction.second.clearDirtyFlag();
	}

	mDirty = false;
}

void cPlotConfigCorrections::setDirtyFlag(bool dirty)
{
	for (auto& correction : mCorrections)
	{
		correction.second.setDirtyFlag(dirty);
	}

	mDirty = dirty;
}

cPlotConfigCorrections::iterator cPlotConfigCorrections::begin()
{
	return mCorrections.begin();
}

cPlotConfigCorrections::iterator cPlotConfigCorrections::end()
{
	return mCorrections.end();
}

cPlotConfigCorrections::const_iterator	cPlotConfigCorrections::begin() const
{
	return mCorrections.begin();
}

cPlotConfigCorrections::const_iterator	cPlotConfigCorrections::end() const
{
	return mCorrections.end();
}

cPlotConfigCorrections::const_iterator	cPlotConfigCorrections::find(const int date) const
{
	if (mCorrections.empty())
		return mCorrections.end();

	if (date < mCorrections.begin()->first)
		return mCorrections.begin();

	PlotCorrections_t::const_iterator result = mCorrections.end();

	for (auto it = mCorrections.begin(); it != mCorrections.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cPlotConfigCorrections::iterator cPlotConfigCorrections::find(const int date)
{
	if (mCorrections.empty())
		return mCorrections.end();

	if (date < mCorrections.begin()->first)
		return mCorrections.begin();

	PlotCorrections_t::iterator result = mCorrections.end();

	for (auto it = mCorrections.begin(); it != mCorrections.end(); ++it)
	{
		if (it->first > date)
			return result;

		result = it;
	}

	return result;
}

cPlotConfigCorrections::PlotCorrections_t::const_iterator cPlotConfigCorrections::find(int month, int day) const
{
	return find(plot_config::to_date(month, day));
}

cPlotConfigCorrections::PlotCorrections_t::iterator cPlotConfigCorrections::find(int month, int day)
{
	return find(plot_config::to_date(month, day));
}

cPlotConfigCorrections::const_iterator	cPlotConfigCorrections::find_exact(const int month, const int day) const
{
	int date = plot_config::to_date(month, day);

	return mCorrections.find(date);
}

cPlotConfigCorrections::iterator cPlotConfigCorrections::find_exact(const int month, const int day)
{
	int date = plot_config::to_date(month, day);

	return mCorrections.find(date);
}


cPlotConfigCorrection& cPlotConfigCorrections::add(const int month, const int day)
{
	int date = plot_config::to_date(month, day);

	if (mCorrections.contains(date))
	{
		return mCorrections.find(date)->second;
	}

	cPlotConfigCorrection correction = { month, day };

	mCorrections.insert(std::make_pair(date, std::move(correction)));

	cPlotConfigCorrection& result = mCorrections.find(date)->second;
	result.setDirtyFlag(true);

	mDirty = true;

	return result;

}

void cPlotConfigCorrections::load(const nlohmann::json& jdoc)
{
	for (const auto& entry : jdoc)
	{
		if (entry.contains("date"))
		{
			std::string date = entry["date"];
			auto pos = date.find('/');
			int month = std::stoi(date.substr(0, pos));
			int day = std::stoi(date.substr(pos + 1));
			cPlotConfigCorrection correction(month, day);
			correction.load(entry);

			mCorrections.insert(std::make_pair(correction.date(), std::move(correction)));
		}
	}
}

nlohmann::json cPlotConfigCorrections::save()
{
	nlohmann::json correctionDoc;

	for (auto& correction : mCorrections)
	{
		correctionDoc.push_back(correction.second.save());
	}

	return correctionDoc;
}


