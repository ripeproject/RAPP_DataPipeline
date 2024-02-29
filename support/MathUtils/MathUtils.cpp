
#include "MathUtils.hpp"
#include "Constants.hpp"

#include <algorithm>
#include <cctype>
#include <numbers>


double nMathUtils::wrap_neg_pi_to_pi(double angle_rad)
{
    while (angle_rad > std::numbers::pi)
    {
        angle_rad -= nConstants::TWO_PI;
    }

    while (angle_rad < -std::numbers::pi)
    {
        angle_rad += nConstants::TWO_PI;
    }

    return angle_rad;
}

double nMathUtils::wrap_0_to_two_pi(double angle_rad)
{
    while (angle_rad > nConstants::TWO_PI)
    {
        angle_rad -= nConstants::TWO_PI;
    }

    while (angle_rad < 0)
    {
        angle_rad += nConstants::TWO_PI;
    }

    return angle_rad;
}

double nMathUtils::wrap_0_to_360(double angle_deg)
{
    while (angle_deg > 360.0)
    {
        angle_deg -= 360.0;
    }

    while (angle_deg < 0.0)
    {
        angle_deg += 360.0;
    }

    return angle_deg;
}

double nMathUtils::wrap_neg_180_to_180(double angle_deg)
{
    while (angle_deg > 180.0)
    {
        angle_deg -= 360.0;
    }

    while (angle_deg < -180.0)
    {
        angle_deg += 360.0;
    }

    return angle_deg;
}


