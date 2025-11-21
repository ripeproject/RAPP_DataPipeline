
#pragma once

#include <opencv2/core.hpp>

#include <filesystem>
#include <string>
#include <fstream>
#include <utility>
#include <map>


class cBIL_2_Png
{
public:
	cBIL_2_Png();
	~cBIL_2_Png();

	bool loadHeader(const std::string& in);

	std::pair<float, float> getWavelengthRange() const;

	void setRgbWavelengths_nm(float red_nm, float green_nm, float blue_nm);

    void writeRgbImage(std::filesystem::path in, std::filesystem::path out);

private:
    std::filesystem::path mOutputPath;

	float mColorScale = 1.0;

	float mRed_nm   = 641.07f;
	float mGreen_nm = 547.95f;
	float mBlue_nm  = 463.30f;

	std::size_t mRedIndex   = 279;
	std::size_t mGreenIndex = 169;
	std::size_t mBlueIndex  = 69;

	cv::Mat mImage;

	std::map<std::string, std::string> mHeader;
};
