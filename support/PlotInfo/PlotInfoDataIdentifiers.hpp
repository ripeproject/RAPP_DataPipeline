
#pragma once

#include <cstdint>

namespace plot_info
{
	/**********************************************************
	   WARNING: DO NOT CHANGE THE ORDER OF THESE ENUMS!!!!
	   
	   Only add new one to the end of the list.
	**********************************************************/
    enum class DataID : uint16_t
    {
		PLOT_ID,
		PLOT_NAME,
		POINT_CLOUD_DATA,
		POINT_CLOUD_DIMENSIONS,
		EVENT,
		DESCRIPTION,
	};
}


