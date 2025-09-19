
#pragma once

#include "datatypes.hpp"


enum class ePlotOrientation { eWEST_TO_EAST, eEAST_TO_WEST, eNORTH_TO_SOUTH, eSOUTH_TO_NORTH };
enum class ePlotIsolationMethod { NONE, CENTER_OF_PLOT, CENTER_OF_HEIGHT, POUR, ITERATIVE, FIND_CENTER, INCLUSIONS };
enum class ePlotExclusionType { CIRCLE, OVAL, SQUARE, RECTANGLE };
enum class ePlotInclusionType { CIRCLE, OVAL, SQUARE, RECTANGLE };


namespace rfm
{
	struct sPlotBoundingBox_t
	{
		int32_t	plotNumber = -1;
		rappPoint2D_t northEastCorner;
		rappPoint2D_t northWestCorner;
		rappPoint2D_t southEastCorner;
		rappPoint2D_t southWestCorner;

		std::int32_t length() const
		{
			return ((northEastCorner.y_mm - northWestCorner.y_mm) + (southEastCorner.y_mm - southWestCorner.y_mm)) / 2;
		}

		std::int32_t width() const
		{
			return ((southEastCorner.x_mm - northEastCorner.x_mm) + (southWestCorner.x_mm - northWestCorner.x_mm)) / 2;
		}
	};

	struct sPlotBoundingCircle_t
	{
		int32_t	plotNumber = -1;

		std::int32_t radius_mm = 0;
		rappPoint2D_t center;
	};
}

