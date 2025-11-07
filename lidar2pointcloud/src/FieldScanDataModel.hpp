#pragma once

#include "datatypes.hpp"

#include "OusterInfo.hpp"

#include "RappFieldBoundary.hpp"
#include "RappTriangle.hpp"

#include "RappPointCloud.hpp"

#include <cbdf/ProcessingInfo.hpp>

#include <cbdf/ExperimentInfo.hpp>
#include <cbdf/SsnxInfo.hpp>
#include <cbdf/SpiderCamInfo.hpp>
#include <cbdf/WeatherInfo.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward Declarations
class cFieldScanLoader;


class cFieldScanDataModel
{
public:
	explicit cFieldScanDataModel(int id);
	~cFieldScanDataModel();

	const std::string& getFileName() const;

	const std::string& getExperimentTitle() const;
	const std::string& getMeasurementTitle() const;

	double getScanTime_sec() const;
	double getGroundTrack_deg() const;

	void loadFieldScanData(const std::string& filename);

	void clear();

	bool hasLidarData() const;
	bool hasSpiderData() const;
	bool hasGpsData() const;

	std::shared_ptr<cProcessingInfo> getProcessingInfo();
	std::shared_ptr<cProcessingInfo> getProcessingInfo() const;

	std::shared_ptr<cExperimentInfo> getExperimentInfo();
	std::shared_ptr<cExperimentInfo> getExperimentInfo() const;

	std::shared_ptr<cSpiderCamInfo>  getSpiderCamInfo();
	std::shared_ptr<cSpiderCamInfo>  getSpiderCamInfo() const;

	std::shared_ptr<cSsnxInfo>       getSsnxInfo();
	std::shared_ptr<cSsnxInfo>       getSsnxInfo() const;

	std::shared_ptr<cOusterInfo>	 getOusterInfo();
	std::shared_ptr<cOusterInfo>	 getOusterInfo() const;

	std::shared_ptr<cWeatherInfo>	 getWeatherInfo();
	std::shared_ptr<cWeatherInfo>	 getWeatherInfo() const;

	void setScanTime_sec(double time_sec);
	void setGroundTrack_deg(double track_deg);

	void validateStartPosition(int32_t x_mm, int32_t y_mm, int32_t z_mm, uint32_t tolerence_mm = 500);
	void validateEndPosition(int32_t x_mm, int32_t y_mm, int32_t z_mm, uint32_t tolerence_mm = 500);

private:
	const int mID;

	std::string mFilename;

private:
	std::string mExperimentTitle;
	std::string mMeasurementTitle;

	double mScanTime_sec = 0.0;
	double mGroundTrack_deg = 0.0;

	std::shared_ptr<cProcessingInfo> mProcessingInfo;
	std::shared_ptr<cExperimentInfo> mExperimentInfo;
	std::shared_ptr<cSpiderCamInfo>  mSpiderCamInfo;
	std::shared_ptr<cSsnxInfo>		 mSsnxInfo;
	std::shared_ptr<cOusterInfo>	 mOusterInfo;
	std::shared_ptr<cWeatherInfo>	 mWeatherInfo;
};

