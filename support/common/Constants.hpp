
#pragma once

#include <numbers>
#include <cmath>

namespace nConstants
{
//	const static double PI = 2.0 * std::asin(+1.0);
	const static double PI = std::numbers::pi;
	const static double TWO_PI = 2.0 * PI;

	/*** Angle Conversions ***/
	const static double RAD_TO_DEG = 180.0 / PI;
	const static double DEG_TO_RAD = 1.0 / RAD_TO_DEG;

	/*** Length Conversions ***/
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

	/*** Time Conversions ***/
	const static double SEC_TO_MS = 1000.0;
	const static double MS_TO_SEC = 1.0 / SEC_TO_MS;
	const static double MS_TO_US = 1000.0;
	const static double MS_TO_NS  = 1'000'000.0;

	const static double SEC_TO_US = SEC_TO_MS * MS_TO_US;
	const static double US_TO_SEC = 1.0 / SEC_TO_US;
	const static double US_TO_MS  = 1.0 / MS_TO_US;
	const static double US_TO_NS  = 1000.0;

	const static double SEC_TO_NS = 1'000'000'000.0;
	const static double NS_TO_SEC = 1.0 / SEC_TO_NS;
	const static double NS_TO_US  = 1.0 / US_TO_NS;
	const static double NS_TO_MS  = 1.0 / MS_TO_NS;

	/*** Speed Conversions ***/
	const static double MPS_TO_MPH = 2.23693629;
	const static double MPH_TO_MPS = 1.0 / MPS_TO_MPH;
}