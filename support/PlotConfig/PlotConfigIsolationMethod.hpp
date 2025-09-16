
#pragma once

#include "datatypes.hpp"

#include "PlotSplitDataTypes.hpp"
#include "PlotConfigInclusion.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <optional>
#include <string>
#include <memory>


class cPlotConfigIsolationMethod
{
public:
	cPlotConfigIsolationMethod();
	~cPlotConfigIsolationMethod() = default;

	cPlotConfigIsolationMethod& operator=(const cPlotConfigIsolationMethod& rhs);

	void clear();

	bool isDirty() const;

	ePlotIsolationMethod getMethod() const;

	// Use when method is "Center of Plot", "Center of Height", or "Iterative"
	double getPlotLength_mm() const;
	double getPlotWidth_mm() const;

	// Use when method is "Center of Height" or "Iterative"
	std::optional<double> getHeightThreshold_pct() const;
	std::optional<double> getMaxDisplacement_pct() const;

	// Use when method is "Iterative"
	double getTolerance_mm() const;
	double getBounds_pct() const;

	void setMethod(ePlotIsolationMethod method);

	void setPlotLength_mm(double length_mm);
	void setPlotWidth_mm(double width_mm);

	void clearHeightThreshold_pct();
	void setHeightThreshold_pct(double threshold_pct);
	void clearMaxDisplacement_pct();
	void setMaxDisplacement_pct(double displacement_pct);

	void setTolerance_mm(double tolerance_mm);
	void setBounds_pct(double bounds_pct);

	bool operator==(const cPlotConfigIsolationMethod& rhs) const;
	bool operator!=(const cPlotConfigIsolationMethod& rhs) const;

	void clearDirtyFlag();
	void setDirtyFlag(bool dirty);

protected:
	void setDirty(bool dirty);

	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	ePlotIsolationMethod mMethod = ePlotIsolationMethod::NONE;

	double mPlotLength_mm = 0.0;
	double mPlotWidth_mm = 0.0;

	std::optional<double> mHeightThreshold_pct;
	std::optional<double> mMaxDisplacement_pct;

	double mTolerance_mm = 15.0;
	double mBounds_pct = 50.0;

	std::vector<cPlotConfigInclusion> mInclusions;

	friend class cPlotConfigFile;
	friend class cPlotConfigPlotInfo;
	friend class cPlotConfigCorrection;
};


