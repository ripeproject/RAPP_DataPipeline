
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

	cPlotConfigCorrection& operator=(const cPlotConfigCorrection& other);

	bool operator==(const cPlotConfigCorrection& other) const;
	bool operator!=(const cPlotConfigCorrection& other) const;

	// Similar to the assignment opereator, but does not change the date information!
	cPlotConfigCorrection& assign(const cPlotConfigCorrection& other);

	// Similar to equal, but does not compare the date information!
	bool same(const cPlotConfigCorrection& other) const;

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

	bool hasExclusions() const;

	const std::vector<cPlotConfigExclusion>& getExclusions() const;
	std::vector<cPlotConfigExclusion>& getExclusions();

	bool hasInclusions() const;

	const std::vector<cPlotConfigInclusion>& getInclusions() const;
	std::vector<cPlotConfigInclusion>& getInclusions();

	void setBounds(const cPlotConfigBoundary& bounds);
	void setIsolationMethod(const cPlotConfigIsolationMethod& method);

	void setExclusions(const std::vector<cPlotConfigExclusion>& exclusions);
	void setInclusions(const std::vector<cPlotConfigInclusion>& exclusions);

	cPlotConfigExclusion& add(const ePlotExclusionType type);
	void clearExclusions();

	cPlotConfigInclusion& add(const ePlotInclusionType type);
	void clearInclusions();

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	int mEffectiveMonth;
	int mEffectiveDay;

	cPlotConfigBoundary mBounds;
	cPlotConfigIsolationMethod mIsolationMethod;
	std::vector<cPlotConfigExclusion> mExclusions;
	std::vector<cPlotConfigInclusion> mInclusions;

	friend class cPlotConfigCorrections;
};


class cPlotConfigCorrections
{
public:
	typedef std::map<int, cPlotConfigCorrection> PlotCorrections_t;

	typedef PlotCorrections_t::iterator			iterator;
	typedef PlotCorrections_t::const_iterator	const_iterator;

public:
	cPlotConfigCorrections();
	~cPlotConfigCorrections();

	bool operator==(const cPlotConfigCorrections& other) const;
	bool operator!=(const cPlotConfigCorrections& other) const;

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

	const std::vector<cPlotConfigExclusion>& getExclusions(int date) const;
	std::vector<cPlotConfigExclusion>& getExclusions(int date);

	const std::vector<cPlotConfigExclusion>& getExclusions(int month, int day) const;
	std::vector<cPlotConfigExclusion>& getExclusions(int month, int day);

	const std::vector<cPlotConfigInclusion>& getInclusions(int date) const;
	std::vector<cPlotConfigInclusion>& getInclusions(int date);

	const std::vector<cPlotConfigInclusion>& getInclusions(int month, int day) const;
	std::vector<cPlotConfigInclusion>& getInclusions(int month, int day);

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


