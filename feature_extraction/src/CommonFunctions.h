#ifndef _COMMON_FUNCTIONS_
#define _COMMON_FUNCTIONS_
#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

#include "nanoflann.hpp"
#include "utils.h"

//using namespace cv;
//using namespace std;
//using namespace nanoflann;

namespace lineFunctions
{
	void lineFitting(int rows, int cols, std::vector<cv::Point>& contour,
					double thMinimalLineLength, std::vector<std::vector<cv::Point2d>>& lines);

	void subDivision(std::vector<std::vector<cv::Point>>& straightString,
					std::vector<cv::Point>& contour, int first_index, int last_index,
					double min_deviation, int min_size);

	void lineFittingSVD(cv::Point *points, int length, std::vector<double>& parameters, double& maxDev);
};

struct PCAInfo
{
	double lambda0 = 0.0;
	double scale = 0.0;
	cv::Matx31d normal, planePt;
	std::vector<int> idxAll, idxIn;

	PCAInfo &operator =(const PCAInfo &info)
	{
		this->lambda0 = info.lambda0;
		this->normal = info.normal;
		this->idxIn = info.idxIn;
		this->idxAll = info.idxAll;
		this->scale = scale;
		return *this;
	}
};

class PCAFunctions 
{
public:
	PCAFunctions(void) = default;
	~PCAFunctions(void) = default;

	void Ori_PCA( PointCloud<double> &cloud, int k, std::vector<PCAInfo> &pcaInfos, double &scale, double &magnitd );

	void PCASingle( std::vector<std::vector<double> > &pointData, PCAInfo &pcaInfo );

	void MCMD_OutlierRemoval( std::vector<std::vector<double> > &pointData, PCAInfo &pcaInfo );

	double median(std::vector<double> dataset);
};

#endif //_COMMON_FUNCTIONS_
