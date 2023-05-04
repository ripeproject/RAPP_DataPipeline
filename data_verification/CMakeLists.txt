# The Data Verificator project will build an application for verification of 
# Ceres Block Data Format files

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

# Use integer versions instead of strings for easier handling if required
SET(DATA_VERIFICATOR_MAJOR 1)
SET(DATA_VERIFICATOR_MINOR 0)
SET(DATA_VERIFICATOR_PATCH 0)

ADD_DEFINITIONS(
     -DDATA_VERIFICATOR_MAJOR=${DATA_VERIFICATOR_MAJOR}
     -DDATA_VERIFICATOR_MINOR=${DATA_VERIFICATOR_MINOR}
     -DDATA_VERIFICATOR_PATCH=${DATA_VERIFICATOR_PATCH}
)

SET(DATA_VERIFICATOR_VERSION "${DATA_VERIFICATOR_MAJOR}.${DATA_VERIFICATOR_MINOR}.${DATA_VERIFICATOR_PATCH}")

# Public version has format YY.V, where:
# -- YY - two last digits of the year of release
# -- V  - version of the release
# Example: first release in year 2023 has version 1.23.0 and public version 23.1
#MATH(EXPR CPP_CBDF "${CPP_CBDF_PATCH} + 1" OUTPUT_FORMAT DECIMAL)
#SET(CPP_CBDF_PUBLIC_VERSION "1.0")
#SET(PACKAGE cppCBDF)
#STRING(TIMESTAMP CURRENTYEAR "%Y")
#SET(COPYRIGHT_YEARS "2000-${CURRENTYEAR}")



#******************************************************************************
# PROJECT LANGUAGE SUPPORT
#******************************************************************************

# We need a C and C++ compiler, so make sure project() test for it.
project(data_verificator VERSION "${DATA_VERIFICATOR_VERSION}" LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn off any compiler extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

#******************************************************************************
# PROJECT DIRECTORY SUPPORT
#******************************************************************************

# Automatically include our own cmake directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# This tells cmake to search lib64, lib, ... directories in linux systems
set(CMAKE_LIBRARY_ARCHITECTURE TRUE)

# Use GNUInstallDirst to get canonical paths
include(GNUInstallDirs)

# Control where libraries and executables are placed during the build.
# With the following settings executables are placed in <the top level of the
# build tree>/bin and libraries/archives in <top level of the build tree>/lib.
# This is particularly useful to run ctests on libraries built on Windows
# machines: tests, which are executables, are placed in the same folders of
# dlls, which are treated as executables as well, so that they can properly
# find the libraries to run. This is a because of missing RPATH on Windows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# The clang compiler does not automatically search the /usr/local directories
if (APPLE)
   include_directories(SYSTEM "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
   link_directories("${CMAKE_INSTALL_FULL_LIBDIR}")
endif()

if ((LOCAL_INSTALL_PATH) AND (NOT LOCAL_INSTALL_PATH STREQUAL ""))
  list(APPEND CMAKE_PREFIX_PATH ${LOCAL_INSTALL_PATH})
  list(APPEND CMAKE_PREFIX_PATH "${LOCAL_INSTALL_PATH}/lib/cmake")
  list(APPEND CMAKE_PREFIX_PATH "${LOCAL_INSTALL_PATH}/lib64/cmake")
endif()

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)


#******************************************************************************
# PROJECT DEPENDENCIES
#******************************************************************************

# Find all the packages needed by this code

find_package(lyra REQUIRED)
find_package(Catch2 QUIET)
find_package(nlohmann_json REQUIRED)

find_package(wxWidgets QUIET COMPONENTS core base aui)


# Find and include my libraries for data collection
find_package(CBDF REQUIRED cbdf sensors)
find_package(ouster REQUIRED)
find_package(ssnx REQUIRED)
#FIND_PACKAGE(ssnx2net REQUIRED)
#FIND_PACKAGE(spidercam REQUIRED)


#******************************************************************************
# BUILD TYPE AND CODE
#******************************************************************************

# to distinguish between debug and release lib
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Add postfix to target for Release build.")
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Add postfix to target for Debug build.")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "Add postfix to target for RelWithDebInfo build.")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "Add postfix to target for MinSizeRel build")


# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

message("CMake build is: ${CMAKE_BUILD_TYPE}")

if (MSVC)
  # Target Windows 10
  add_definitions(-D_WIN32_WINNT=0x0A00)
#  add_definitions(-D_WIN32_WINNT=0x0A00 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

if (APPLE)
  SET(CMAKE_INSTALL_NAME_DIR "@executable_path/../Libraries")
endif()


# Add the application/library source code directory
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)


#print_all_variables()
