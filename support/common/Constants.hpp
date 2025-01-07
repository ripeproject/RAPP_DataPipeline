
#pragma once

#include <numbers>
#include <cmath>

namespace nConstants
{
//	const static double PI = 2.0 * std::asin(+1.0);
	constexpr static double PI = std::numbers::pi;
	constexpr static double TWO_PI = 2.0 * PI;

	/*** Angle Conversions ***/
	constexpr static double RAD_TO_DEG = 180.0 / PI;
	constexpr static double DEG_TO_RAD = 1.0 / RAD_TO_DEG;

	/*** Length Conversions ***/
	constexpr static double M_TO_MM = 1000.0;
	constexpr static double MM_TO_M = 1.0 / M_TO_MM;
	constexpr static double M_TO_CM = 100.0;
	constexpr static double CM_TO_M = 1.0 / M_TO_CM;
	constexpr static double CM_TO_MM = 10.0;
	constexpr static double MM_TO_CM = 1.0 / CM_TO_MM;

	constexpr static double FT_TO_IN = 12.0;
	constexpr static double IN_TO_FT = 1.0 / FT_TO_IN;

	constexpr static double FT_TO_M = 0.3048;
	constexpr static double M_TO_FT = 1.0 / FT_TO_M;

	constexpr static double FT_TO_MM = FT_TO_M * M_TO_MM;
	constexpr static double MM_TO_FT = 1.0 / FT_TO_MM;

	constexpr static double IN_TO_MM = 25.4;
	constexpr static double MM_TO_IN = 1.0 / IN_TO_MM;

	/*** Time Conversions ***/
	constexpr static double SEC_TO_MS = 1000.0;
	constexpr static double MS_TO_SEC = 1.0 / SEC_TO_MS;
	constexpr static double MS_TO_US = 1000.0;
	constexpr static double MS_TO_NS  = 1'000'000.0;

	constexpr static double SEC_TO_US = SEC_TO_MS * MS_TO_US;
	constexpr static double US_TO_SEC = 1.0 / SEC_TO_US;
	constexpr static double US_TO_MS  = 1.0 / MS_TO_US;
	constexpr static double US_TO_NS  = 1000.0;

	constexpr static double SEC_TO_NS = 1'000'000'000.0;
	constexpr static double NS_TO_SEC = 1.0 / SEC_TO_NS;
	constexpr static double NS_TO_US  = 1.0 / US_TO_NS;
	constexpr static double NS_TO_MS  = 1.0 / MS_TO_NS;

	/*** Speed Conversions ***/
	constexpr static double MPS_TO_MPH = 2.23693629;
	constexpr static double MPH_TO_MPS = 1.0 / MPS_TO_MPH;
}