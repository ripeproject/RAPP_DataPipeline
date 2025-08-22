
#pragma once


enum class eKinematicModel { CONSTANT_SPEED, SPIDERCAM, GPS_SPEEDS };

enum class eOrientationModel { CONSTANT, LINEAR, INTREP_CURVE, IMU };

enum class eTranslateToGroundModel { NONE, CONSTANT, FIT, INTREP_CURVE };

enum class eRotateToGroundModel { NONE, CONSTANT, FIT, INTREP_CURVE };

namespace kdt
{
	struct sDollyAtitude_t
	{
		double pitch_deg = 0.0;
		double roll_deg = 0.0;
		double yaw_deg = 0.0;
	};

	struct sDollyOrientation_t
	{
		double timestamp_us = 0.0;

		double pitch_deg = 0.0;
		double roll_deg = 0.0;
		double yaw_deg = 0.0;

		double pitchRate_dps = 0.0;
		double rollRate_dps = 0.0;
		double yawRate_dps = 0.0;
	};

	struct sDollyOrientationInterpPoint_t
	{
		double distance_pct = 0.0;
		double pitch_deg = 0.0;
		double roll_deg = 0.0;
		double yaw_deg = 0.0;

		bool operator==(const sDollyOrientationInterpPoint_t& rhs) const
		{
			return (distance_pct == rhs.distance_pct) && (pitch_deg == rhs.pitch_deg)
				&& (roll_deg == rhs.roll_deg) && (yaw_deg == rhs.yaw_deg);
		}

		bool operator!=(const sDollyOrientationInterpPoint_t& rhs) const
		{
			return !operator==(rhs);
		}
	};

	/*
	 * The RAPP uses a south/east/up coordinate system
	 * x axis is positive heading south
	 * y axis is positive heading east
	 * z axis is positive going up
	 */
	struct sDollyInfo_t
	{
		double timestamp_us = 0.0;

		double x_mm = 0.0;
		double y_mm = 0.0;
		double z_mm = 0.0;

		double vx_mmps = 0.0;
		double vy_mmps = 0.0;
		double vz_mmps = 0.0;

		double pitch_deg = 0.0;
		double roll_deg = 0.0;
		double yaw_deg = 0.0;

		double pitchRate_dps = 0.0;
		double rollRate_dps = 0.0;
		double yawRate_dps = 0.0;
	};

	struct sSensorKinematics_t
	{
		double startPitch_deg = 0.0;
		double startRoll_deg = 0.0;
		double startYaw_deg = 0.0;

		double endPitch_deg = 0.0;
		double endRoll_deg = 0.0;
		double endYaw_deg = 0.0;
	};
}