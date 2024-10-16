
#include "PlotConfigIsolationMethod.hpp"

#include "Constants.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <stdexcept>


cPlotConfigIsolationMethod::cPlotConfigIsolationMethod()
{}

void cPlotConfigIsolationMethod::clear()
{
	mDirty = false;
}

bool cPlotConfigIsolationMethod::isDirty() const
{
	return mDirty;
}

bool cPlotConfigIsolationMethod::operator==(const cPlotConfigIsolationMethod& rhs) const
{
	if (mMethod == rhs.mMethod)
	{
		switch (mMethod)
		{
		case ePlotIsolationMethod::NONE:
			return true;

		case ePlotIsolationMethod::CENTER_OF_HEIGHT:
			return mPlotLength_mm == rhs.mPlotLength_mm
				&& mPlotWidth_mm == rhs.mPlotWidth_mm;

		case ePlotIsolationMethod::POUR:
			return mUseCenterOfHeight == rhs.mUseCenterOfHeight;

		case ePlotIsolationMethod::ITERATIVE:
			return mPlotLength_mm == rhs.mPlotLength_mm
				&& mPlotWidth_mm == rhs.mPlotWidth_mm;

		case ePlotIsolationMethod::FIND_CENTER:
			return mPlotLength_mm == rhs.mPlotLength_mm
				&& mPlotWidth_mm == rhs.mPlotWidth_mm;
		}
	}

	return false;
}

bool cPlotConfigIsolationMethod::operator!=(const cPlotConfigIsolationMethod& rhs) const
{
	return !operator==(rhs);
}


ePlotIsolationMethod cPlotConfigIsolationMethod::getMethod() const
{
	return mMethod;
}

double cPlotConfigIsolationMethod::getPlotLength_mm() const
{
	return mPlotLength_mm;
}

double cPlotConfigIsolationMethod::getPlotWidth_mm() const
{
	return mPlotWidth_mm;
}

bool cPlotConfigIsolationMethod::getUseCenterOfHeight() const
{
	return mUseCenterOfHeight;
}

double cPlotConfigIsolationMethod::getHeightThreshold_pct() const
{
	return mHeightThreshold_pct;
}

double cPlotConfigIsolationMethod::getTolerance_mm() const
{
	return mTolerance_mm;
}

double cPlotConfigIsolationMethod::getBounds_pct() const
{
	return mBounds_pct;
}

void cPlotConfigIsolationMethod::setMethod(ePlotIsolationMethod method)
{
	mDirty = (mMethod != method);
	mMethod = method;
}

void cPlotConfigIsolationMethod::setPlotLength_mm(double length_mm)
{
	mDirty = (mPlotLength_mm != length_mm);
	mPlotLength_mm = length_mm;
}

void cPlotConfigIsolationMethod::setPlotWidth_mm(double width_mm)
{
	mDirty = (mPlotWidth_mm != width_mm);
	mPlotWidth_mm = width_mm;
}

void cPlotConfigIsolationMethod::setUseCenterOfHeight(bool enable)
{
	mDirty = (mUseCenterOfHeight != enable);
	mUseCenterOfHeight = enable;
}

void cPlotConfigIsolationMethod::setHeightThreshold_pct(double threshold_pct)
{
	if (threshold_pct < 0.0)  threshold_pct = 0.0;
	if (threshold_pct > 100.0)  threshold_pct = 100.0;

	mDirty = (mHeightThreshold_pct != threshold_pct);
	mHeightThreshold_pct = threshold_pct;
}


void cPlotConfigIsolationMethod::setTolerance_mm(double tolerance_mm)
{
	mDirty = (mTolerance_mm != tolerance_mm);
	mTolerance_mm = tolerance_mm;
}

void cPlotConfigIsolationMethod::setBounds_pct(double bounds_pct)
{
	mDirty = (mBounds_pct != bounds_pct);
	mBounds_pct = bounds_pct;
}

void cPlotConfigIsolationMethod::setDirtyFlag(bool dirty)
{
	mDirty = dirty;
}

void cPlotConfigIsolationMethod::load(const nlohmann::json& jdoc)
{

	if (!jdoc.contains("isolation method"))
		return;
		
	auto method = jdoc["isolation method"];

	if (method.is_null())
	{
		mMethod = ePlotIsolationMethod::NONE;
		return;
	}

	std::string type = method["type"];
	
	if (type == "none")
	{
		mMethod = ePlotIsolationMethod::NONE;
	}
	else if (type == "center of height")
	{
		mMethod = ePlotIsolationMethod::CENTER_OF_HEIGHT;
		mPlotLength_mm = method["plot length (mm)"];
		mPlotWidth_mm = method["plot width (mm)"];

		if (method.contains("height threshold (%)"))
			mHeightThreshold_pct = method["height threshold (%)"];
	}
	else if (type == "pour")
	{
		mMethod = ePlotIsolationMethod::POUR;
		mUseCenterOfHeight = method["use center of height"];
		if (method.contains("height threshold (%)"))
			mHeightThreshold_pct = method["height threshold (%)"];
	}
	else if (type == "iterative")
	{
		mMethod = ePlotIsolationMethod::ITERATIVE;
		mPlotLength_mm = method["plot length (mm)"];
		mPlotWidth_mm = method["plot width (mm)"];
		mTolerance_mm = method["tolerance (mm)"];
		mBounds_pct = method["bounds (pct)"];

		if (method.contains("height threshold (%)"))
			mHeightThreshold_pct = method["height threshold (%)"];
	}
	else if (type == "find center")
	{
		mMethod = ePlotIsolationMethod::FIND_CENTER;
		mPlotLength_mm = method["plot length (mm)"];
		mPlotWidth_mm = method["plot width (mm)"];

		if (method.contains("height threshold (%)"))
			mHeightThreshold_pct = method["height threshold (%)"];
	}
}

nlohmann::json cPlotConfigIsolationMethod::save()
{
	nlohmann::json method;

	switch (mMethod)
	{
	case ePlotIsolationMethod::NONE:
		method["type"] = "none";
		break;
	case ePlotIsolationMethod::CENTER_OF_HEIGHT:
		method["type"] = "center of height";

		method["plot length (mm)"] = mPlotLength_mm;
		method["plot width (mm)"] = mPlotWidth_mm;

		if (mHeightThreshold_pct > 0.0)
			 method["height threshold (%)"] = mHeightThreshold_pct;
		break;

	case ePlotIsolationMethod::POUR:
		method["type"] = "pour";
		method["use center of height"] = mUseCenterOfHeight;
		break;

	case ePlotIsolationMethod::ITERATIVE:
		method["type"] = "iterative";

		method["plot length (mm)"] = mPlotLength_mm;
		method["plot width (mm)"] = mPlotWidth_mm;
		method["tolerance (mm)"] = mTolerance_mm;
		method["bounds (pct)"] = mBounds_pct;

		if (mHeightThreshold_pct > 0.0)
			method["height threshold (%)"] = mHeightThreshold_pct;

		break;

	case ePlotIsolationMethod::FIND_CENTER:
		method["type"] = "find center";

		method["plot length (mm)"] = mPlotLength_mm;
		method["plot width (mm)"] = mPlotWidth_mm;

		if (mHeightThreshold_pct > 0.0)
			method["height threshold (%)"] = mHeightThreshold_pct;
		break;
	}

	mDirty = false;

	return method;
}


