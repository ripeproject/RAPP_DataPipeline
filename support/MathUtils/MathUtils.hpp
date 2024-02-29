#pragma once

#include <string>
#include <vector>


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

} // End of Namespace nMathUtils