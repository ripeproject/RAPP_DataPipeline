
# The plot configuration

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


#******************************************************************************
# PROJECT OPTIONS
#******************************************************************************

if (MSVC)
    option(MSVC_STATIC_RUNTIME "Link static runtime libraries" OFF)
endif()


#******************************************************************************
# PROJECT LANGUAGE SUPPORT
#******************************************************************************

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn off any compiler extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

#******************************************************************************
# PROJECT DEPENDENCIES
#******************************************************************************

# Find all the packages needed by this code

find_package(nlohmann_json REQUIRED)


# For testing
find_package(Catch2 QUIET)


add_library(plot_config)

set_target_properties(plot_config PROPERTIES LANGUAGE CXX)

target_compile_features(plot_config PRIVATE cxx_std_20)

target_sources(plot_config
PUBLIC
	PlotConfigFile.hpp
	PlotConfigOptions.hpp
	PlotConfigCorrections.hpp
	PlotConfigScan.hpp
	PlotConfigBoundary.hpp
	PlotConfigIsolationMethod.hpp

PRIVATE

	PlotConfigFile.cpp
	PlotConfigOptions.cpp
	PlotConfigCorrections.cpp
	PlotConfigScan.cpp
	PlotConfigBoundary.cpp
	PlotConfigIsolationMethod.cpp
)

target_include_directories(plot_config PRIVATE ${CMAKE_INSTALL_PREFIX}/include)

target_include_directories(plot_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)
target_include_directories(plot_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../PlotUtils)
target_include_directories(plot_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../StringUtils)

target_link_libraries(plot_config PRIVATE string_utils)
target_link_libraries(plot_config PRIVATE nlohmann_json::nlohmann_json)

#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET plot_config PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


