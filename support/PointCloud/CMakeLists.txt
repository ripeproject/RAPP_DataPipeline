
# The Pointcloud Serializer/Parser project will build a library for serialization  
# or parsering of pointcloud data

# Usage:
# cmake -G <generator> -D CMAKE_INSTALL_PREFIX=<path to support libraries>

CMAKE_MINIMUM_REQUIRED(VERSION 3.18)
MESSAGE(STATUS "Found CMake ${CMAKE_VERSION}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# print_all_variables is a debug macro to list all of the CMAKE variables
#
macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

# Fix behavior of CMAKE_CXX_STANDARD and CMAKE_C_STANDARD when targeting macOS.
IF(POLICY CMP0025)
     CMAKE_POLICY(SET CMP0025 NEW)
ENDIF()

# Potential dangerous comparison of variables. Details: https://cmake.org/cmake/help/v3.1/policy/CMP0054.html
IF(POLICY CMP0054)
     CMAKE_POLICY(SET CMP0054 NEW)
ENDIF()

IF(POLICY CMP0071)
     CMAKE_POLICY(SET CMP0071 NEW)
ENDIF()

#
# Setting this policy to NEW allows us to use the MSVC_RUNTIME_LIBRARY 
# target property.  The generator expression would look like:
#
# To use MultiThreaded (-MT) and MultiThreadedDebug (-MTd)
# set_property(TARGET target PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#
# To use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
# set_property(TARGET target PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
#

cmake_policy(SET CMP0091 NEW)

# warn about deprecated stuff so that we may try fixing it
SET(CMAKE_WARN_DEPRECATED 1)


# Find and include my libraries for data collection
find_package(CBDF REQUIRED cbdf) # info) # ctrl sensors)


#******************************************************************************
# PROJECT OPTIONS
#******************************************************************************

if (MSVC)
    option(MSVC_STATIC_RUNTIME "Link static runtime libraries" OFF)
endif()


#******************************************************************************
# PROJECT LANGUAGE SUPPORT
#******************************************************************************

# We need a C and C++ compiler, so make sure project() test for it.
#project(pointcloud LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn off any compiler extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)



add_library(pointcloud_io)

set_target_properties(pointcloud_io PROPERTIES LANGUAGE CXX)

target_compile_features(pointcloud_io PRIVATE cxx_std_20)

target_sources(pointcloud_io
PUBLIC
	PointCloudUtils.hpp
	
PRIVATE

	PointCloudUtils.cpp
)

target_include_directories(pointcloud_io PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_include_directories(pointcloud_io PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET pointcloud_io PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


