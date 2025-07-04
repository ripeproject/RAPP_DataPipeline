
#pragma once

#include "PlotConfigBoundary.hpp"
#include "PlotConfigIsolationMethod.hpp"


#include "PlotConfigScan.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <limits>


// Forward Declaration
class cPlotConfigCorrection
{
public:
	cPlotConfigCorrection(int month, int day);
	~cPlotConfigCorrection() = default;

	const int date() const;

	const int month() const;
	const int day() const;

	void clear();

	bool isDirty() const;

	bool empty() const;

	bool contains(rfm::rappPoint2D_t point) const;
	bool contains(std::int32_t x_mm, std::int32_t y_mm) const;

	const cPlotConfigBoundary& getBounds() const;
	cPlotConfigBoundary& getBounds();

	const cPlotConfigIsolationMethod& getIsolationMethod() const;
	cPlotConfigIsolationMethod& getIsolationMethod();

	void setBounds(const cPlotConfigBoundary& bounds);
	void setIsolationMethod(const cPlotConfigIsolationMethod& method);

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	const int mEffectiveMonth;
	const int mEffectiveDay;

	cPlotConfigBoundary mBounds;
	cPlotConfigIsolationMethod mIsolationMethod;

	friend class cPlotConfigCorrections;
};


class cPlotConfigCorrections
{
public:
public:
	typedef std::map<int, cPlotConfigCorrection> PlotCorrections_t;

	typedef PlotCorrections_t::iterator			iterator;
	typedef PlotCorrections_t::const_iterator	const_iterator;

public:
	cPlotConfigCorrections();
	~cPlotConfigCorrections();

	bool empty() const;

	bool isDirty() const;

	bool contains(rfm::rappPoint2D_t point) const;
	bool contains(std::int32_t x_mm, std::int32_t y_mm) const;

	const cPlotConfigBoundary& getBounds(int month, int day) const;
	cPlotConfigBoundary& getBounds(int month, int day);

	const cPlotConfigIsolationMethod& getIsolationMethod(int month, int day) const;
	cPlotConfigIsolationMethod& getIsolationMethod(int month, int day);

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	PlotCorrections_t::const_iterator find(int month, int day) const;
	PlotCorrections_t::iterator find(int month, int day);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	PlotCorrections_t mCorrections;

	friend class cPlotConfigFile;
};


