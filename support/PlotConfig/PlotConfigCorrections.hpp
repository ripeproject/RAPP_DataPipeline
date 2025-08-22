
#pragma once

#include "PlotConfigBoundary.hpp"
#include "PlotConfigIsolationMethod.hpp"
#include "PlotConfigExclusion.hpp"
#include "PlotConfigInclusion.hpp"


//#include "PlotConfigScan.hpp"

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

	bool contains_point(rfm::rappPoint2D_t point) const;
	bool contains_point(std::int32_t x_mm, std::int32_t y_mm) const;

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
	std::vector<cPlotConfigExclusion> mExclusions;
	std::vector<cPlotConfigInclusion> mInclusions;

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

	void clear();

	bool empty() const;

	bool isDirty() const;

	bool contains_point(rfm::rappPoint2D_t point) const;
	bool contains_point(std::int32_t x_mm, std::int32_t y_mm) const;


	bool contains(const int date) const;
	bool contains(const int month, const int day) const;

	const cPlotConfigCorrection& front() const;
	cPlotConfigCorrection& front();

	const cPlotConfigBoundary& getBounds(int date) const;
	cPlotConfigBoundary& getBounds(int date);

	const cPlotConfigBoundary& getBounds(int month, int day) const;
	cPlotConfigBoundary& getBounds(int month, int day);

	const cPlotConfigIsolationMethod& getIsolationMethod(int date) const;
	cPlotConfigIsolationMethod& getIsolationMethod(int date);

	const cPlotConfigIsolationMethod& getIsolationMethod(int month, int day) const;
	cPlotConfigIsolationMethod& getIsolationMethod(int month, int day);

	iterator		begin();
	iterator		end();

	const_iterator	begin() const;
	const_iterator	end() const;

	const_iterator	find(const int date) const;
	iterator		find(const int date);

	const_iterator	find(const int month, const int day) const;
	iterator		find(const int month, const int day);

	const_iterator	find_exact(const int month, const int day) const;
	iterator		find_exact(const int month, const int day);

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

	cPlotConfigCorrection& add(const int month, const int day);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	PlotCorrections_t mCorrections;

	friend class cPlotConfigPlotInfo;
};


