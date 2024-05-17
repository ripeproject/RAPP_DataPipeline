
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

	double getPlotLength_mm() const;
	double getPlotWidth_mm() const;

	bool getUseCenterOfHeight() const;

	double getHeightThreshold_pct() const;

	double getTolerance_mm() const;
	double getBounds_pct() const;

	void setMethod(ePlotIsolationMethod method);

	void setPlotLength_mm(double length_mm);
	void setPlotWidth_mm(double width_mm);

	void setUseCenterOfHeight(bool enable);

	void setHeightThreshold_pct(double threshold_pct);

	void setTolerance_mm(double tolerance_mm);
	void setBounds_pct(double bounds_pct);

	bool operator==(const cPlotConfigIsolationMethod& rhs) const;
	bool operator!=(const cPlotConfigIsolationMethod& rhs) const;

	void setDirtyFlag(bool dirty);

protected:
	void load(const nlohmann::json& jdoc);
	nlohmann::json save();

private:
	bool mDirty = false;

	ePlotIsolationMethod mMethod = ePlotIsolationMethod::NONE;

	double mPlotLength_mm = 0.0;
	double mPlotWidth_mm = 0.0;

	double mHeightThreshold_pct = 0.0;

	bool mUseCenterOfHeight = true;

	double mTolerance_mm = 15.0;
	double mBounds_pct = 50.0;

	friend class cPlotConfigFile;
	friend class cPlotConfigPlotInfo;
};


