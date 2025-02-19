#pragma once

#include <string>
#include <limits>


namespace nMathUtils
{
	// Wrap the angle to the range of -pi to pi
	double wrap_neg_pi_to_pi(double angle_rad);

	// Wrap the angle to the range of 0 to two pi
	double wrap_0_to_two_pi(double angle_rad);

	// Wrap the angle to the range of 0 to 360
	double wrap_0_to_360(double angle_deg);

	// Wrap the angle to the range of -180 to 180
	double wrap_neg_180_to_180(double angle_deg);

	template<typename T>
	T bound(T value, T lower_bound, T upper_bound)
	{
		if (value > upper_bound) return upper_bound;
		if (value < lower_bound) return lower_bound;

		return value;
	}

	template<typename T0, typename T1>
	T0 bound(T1 value)
	{
		if (value > std::numeric_limits<T0>::max()) return std::numeric_limits<T0>::max();
		if (value < std::numeric_limits<T0>::lowest()) return std::numeric_limits<T0>::lowest();

		return static_cast<T0>(value);
	}

	template<typename T>
	T threshold(T min, T max, double level_pct)
	{
		if (level_pct > 100.0) return max;
		if (level_pct < 0.0) return min;

		return static_cast<T>((max - min) * (level_pct / 100.0)) + min;
	}
} // End of Namespace nMathUtils