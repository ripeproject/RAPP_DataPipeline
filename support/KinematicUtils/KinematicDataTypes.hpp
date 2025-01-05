
#pragma once


enum class eKinematicModel { CONSTANT_SPEED, SPIDERCAM, GPS_SPEEDS };

enum class eOrientationModel { CONSTANT, LINEAR, INTREP_CURVE, IMU };

enum class eTranslateToGroundModel { NONE, CONSTANT, FIT, INTREP_CURVE };

enum class eRotateToGroundModel { NONE, CONSTANT, FIT, INTREP_CURVE };

