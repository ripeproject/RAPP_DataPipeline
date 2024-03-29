
#pragma once

#include <numbers>
#include <cmath>

namespace nConstants
{
//	const static double PI = 2.0 * std::asin(+1.0);
	const static double PI = std::numbers::pi;
	const static double TWO_PI = 2.0 * PI;

	const static double RAD_TO_DEG = 180.0 / PI;
	const static double DEG_TO_RAD = 1.0 / RAD_TO_DEG;

	const static double M_TO_MM = 1000.0;
	const static double MM_TO_M = 1.0 / M_TO_MM;
	const static double M_TO_CM = 100.0;
	const static double CM_TO_M = 1.0 / M_TO_CM;
	const static double CM_TO_MM = 10.0;
	const static double MM_TO_CM = 1.0 / CM_TO_MM;

	const static double FT_TO_M = 0.3048;
	const static double M_TO_FT = 1.0 / FT_TO_M;

	const static double FT_TO_MM = FT_TO_M * M_TO_MM;
	const static double MM_TO_FT = 1.0 / FT_TO_MM;

	const static double MPS_TO_MPH = 2.23693629;
	const static double MPH_TO_MPS = 1.0 / MPS_TO_MPH;
}