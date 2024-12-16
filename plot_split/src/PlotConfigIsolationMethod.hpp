
#pragma once

#include "datatypes.hpp"

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <string>
#include <memory>


class cPlotConfigIsolationMethod
{
public:
	cPlotConfigIsolationMethod();
	~cPlotConfigIsolationMethod() = default;

	void clear();

	bool isDirty() const;

	ePlotIsolationMethod getMethod() const;

	// Use when method is "Center of Plot", "Center of Height", or "Iterative"
	double getPlotLength_mm() const;
	double getPlotWidth_mm() const;

	// Use when method is "Center of Height" or "Iterative"
	double getHeightThreshold_pct() const;

	// Use when method is "Iterative"
	double getTolerance_mm() const;
	double getBounds_pct() const;

	void setMethod(ePlotIsolationMethod method);

	void setPlotLength_mm(double length_mm);
	void setPlotWidth_mm(double width_mm);

	void setHeightThreshold_pct(double threshold_pct);

	void setTolerance_mm(double tolerance_mm);
	void setBounds_pct(double bounds_pct);

	bool operator==(const cPlotConfigIsolationMethod& rhs) const;
	bool operator!=(const cPlotConfigIsolationMethod& rhs) const;

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

	double mHeightThreshold_pct = 0.0;

	double mTolerance_mm = 15.0;
	double mBounds_pct = 50.0;

	friend class cPlotConfigFile;
	friend class cPlotConfigPlotInfo;
};


