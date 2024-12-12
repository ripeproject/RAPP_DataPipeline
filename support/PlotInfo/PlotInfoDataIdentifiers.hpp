
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
		POINT_CLOUD_DATA_FRAME_ID,
		POINT_CLOUD_DATA_SENSOR_INFO,
        BEGIN_PLOT_INFO_BLOCK,
        END_PLOT_INFO_BLOCK,
        BEGIN_PLOT_INFO_LIST,
        END_PLOT_INFO_LIST,
		SPECIES,
		CULTIVAR,
        BEGIN_TREATMENT_LIST,
        END_OF_TREATMENT_LIST,
        TREATMENT,
        CONSTRUCT_NAME,
		POT_LABEL,
		SEED_GENERATION,
		COPY_NUMBER,
		SUB_PLOT_ID,
		VEGETATION_ONLY,
	};
}


