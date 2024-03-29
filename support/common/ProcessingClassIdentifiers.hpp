
#pragma once

#include <cbdf/extra/ClassIdentifiers.hpp>


enum class ProcessingClassIDs : ClassID_t
{
    /***********************************************************
     * 32,769 - 49,152 : Range for Processed data
     ***********************************************************/

    /*
     * Point Cloud data usually from LiDAR data
     */
    POINT_CLOUD  = static_cast<ClassID_t>(ClassIDs::PROCESSED_DATA_BASE_ID),

    /*
     * General Processing Information
     */
    PROCESSING_INFO = POINT_CLOUD + 0x0080,

    /*
     * Plot Information
     */
    PLOT_INFO = PROCESSING_INFO + 0x0008,

};

