# The plot_split project will build a C++ application to split the point cloud
# into a series of point clouds based on plot borders.

# Usage:
# cmake -G <generator> -D CMAKE_INSTALL_PREFIX=<path to support libraries>

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_minimum_required(VERSION 3.18)

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
cmake_policy(SET CMP0115 NEW)

#******************************************************************************
# PROJECT OPTIONS
#******************************************************************************

option(USE_EXTERNAL_JSON "Use an external JSON library" ON)


if (MSVC)
    option(MSVC_STATIC_RUNTIME "Link static runtime libraries" OFF)
endif()


#******************************************************************************
# PROJECT LANGUAGE SUPPORT
#******************************************************************************

# We need a C++ compiler, so make sure project() test for it.
project(plot_split VERSION 0.1 LANGUAGES CXX)

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
find_package(wxWidgets REQUIRED COMPONENTS core base aui)
find_package(CBDF REQUIRED cbdf info)
find_package(nlohmann_json REQUIRED)


#******************************************************************************
# BUILD TYPE AND CODE
#******************************************************************************

# to distinguish between debug and release lib
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Add postfix to target for Release build.")
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Add postfix to target for Debug build.")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "Add postfix to target for RelWithDebInfo build.")
set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "Add postfix to target for MinSizeRel build")


# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

message("CMake build is: ${CMAKE_BUILD_TYPE}")

if (MSVC)
  # Target Windows 10
  add_definitions(-D_WIN32_WINNT=0x0A00)
#  add_definitions(-D_WIN32_WINNT=0x0A00 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()


# Add the application/library source code directory
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)

# Add the support library source code directory
# Note: the support directory is a symlink 
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/support/PointCloud)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/support/StringUtils)

#print_all_variables()

#******************************************************************************
# P A C K A G E   G E N E R A T O R
#******************************************************************************
set(CPACK_PACKAGE_DESCRIPTION "Just another CPack example")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR "University of Illinois")
set(CPACK_PACKAGE_CONTACT "bfeddrsn@illinois.edu")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
include (CPack)