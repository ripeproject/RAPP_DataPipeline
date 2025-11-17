
#pragma once

//#include <cbdf/SpidercamParser.hpp>
//#include <spidercam/spidercam_types.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>



class cBIL_2_Png
{
public:
	cBIL_2_Png();
	~cBIL_2_Png();

    void setOutputPath(std::filesystem::path out);

	void setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm);

	// Spidercam Parser Data
	void onPosition(double x_mm, double y_mm, double z_mm, double speed_mmps);

	// Experiment Info Parser Data
	void onStartRecordingTimestamp(uint64_t timestamp_ns);
	void onEndRecordingTimestamp(uint64_t timestamp_ns);

private:
    void writeRgbImage(std::filesystem::path filename);

private:
    std::filesystem::path mOutputPath;
	std::fstream mMetaData;

	std::string mId;
	std::string mSerialNumber;
	uint16_t mMinWavelength_nm = 0;
	uint16_t mMaxWavelength_nm = 0;

	std::string mLensName;
	double mWorkingDistance_cm = 0;
	double mFieldOfView_deg = 0;

	uint16_t mAverageFrames = 0;
	uint32_t mFramePeriod_us = 0;
	uint32_t mIntegrationTime_us = 0;

	float mAmbientTemp_C = 0;
	float mSensorTemp_C = 0;


	std::size_t mSpatialSize = 0;
	std::size_t mSpectralSize = 0;
	std::size_t mMaxRows = 0;

	float mColorScale = 1.0;

	float mRed_nm   = 641.07f;
	float mGreen_nm = 547.95f;
	float mBlue_nm  = 463.30f;

	std::size_t mRedIndex   = 279;
	std::size_t mGreenIndex = 169;
	std::size_t mBlueIndex  = 69;

	char mPlotID = 'A';

	std::size_t mActiveRow = 0;

	cv::Mat mImage;

    uint32_t    mFrameCount = 0;

    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, s; };
    struct uint3 { uint16_t s, r, a; };

    std::vector<float4>   mPositions;

    bool mResyncTimestamp = false;
	uint64_t mStartTimestamp_ns = 0;
};
