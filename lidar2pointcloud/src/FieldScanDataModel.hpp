#pragma once

#include "datatypes.hpp"

#include "OusterInfo.hpp"

#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include "RappPointCloud.hpp"

#include <cbdf/ExperimentInfo.hpp>
#include <cbdf/SsnxInfo.hpp>
#include <cbdf/SpiderCamInfo.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward Declarations
class cFieldScanLoader;


class cFieldScanDataModel
{
public:
	cFieldScanDataModel();
	~cFieldScanDataModel();

	double getScanTime_sec() const;
	double getGroundTrack_deg() const;

	const std::string& getFileName();

	void loadFieldScanData(const std::string& filename);

	void clear();

	bool hasLidarData() const;
	bool hasSpiderData() const;
	bool hasGpsData() const;

	std::shared_ptr<cExperimentInfo> getExperimentInfo();
	std::shared_ptr<cSpiderCamInfo>  getSpiderCamInfo();
	std::shared_ptr<cSsnxInfo>       getSsnxInfo();
	std::shared_ptr<cOusterInfo>	 getOusterInfo();

	void setStartIndex(std::size_t i);
	void setEndIndex(std::size_t i);

/*
	void onStartPosition(double x_mm, double y_mm, double h_mm, uint32_t index);
	void onEndPosition(double x_mm, double y_mm, double h_mm, uint32_t index);
	void onGroundTrackUpdate(double groundTrack_deg);
*/

private:
	double mScanTime_sec = 0.0;
	double mGroundTrack_deg = 0.0;

	std::string mFilename;

	std::shared_ptr<cExperimentInfo> mExperimentInfo;
	std::shared_ptr<cSpiderCamInfo>  mSpiderCamInfo;
	std::shared_ptr<cSsnxInfo>		 mSsnxInfo;
	std::shared_ptr<cOusterInfo>	 mOusterInfo;

	std::unique_ptr<cFieldScanLoader> mFieldScanLoader;
};

