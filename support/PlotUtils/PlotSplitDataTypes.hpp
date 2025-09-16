
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
	};
}

