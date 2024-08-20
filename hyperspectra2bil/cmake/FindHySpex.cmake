# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindHySpex
----------

Find HySpex, which is the offical SDK from HySpex for their hyperspectal cameras.

IMPORTED Targets
^^^^^^^^^^^^^^^^

None

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables::

  HySpex_FOUND          - "True" if the HySpex SDK was found
  HySpex_INCLUDE_DIRS   - include directories for HySpex
  HySpex_LIBRARIES      - link against this library to use HySpex

The module will also define these cache variables::

  HySpex_INCLUDE_DIR    - the HySpex include directory
  HySpex_LIBRARY        - the path to the HySpex library

Hints
^^^^^

The ``HYSPEX_SDK`` environment variable optionally specifies the
location of the HySpex SDK root directory for the given architecture. 

#]=======================================================================]


if(WIN32)
  find_path(HySpex_INCLUDE_DIR
    NAMES hyspex/Camera.h Camera.h
    HINTS
      "$ENV{HYSPEX_SDK}/include"
    )

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    find_library(HySpex_LIBRARY
      NAMES HySpexLibrary
      HINTS
        "$ENV{HYSPEX_SDK}/lib/"
        "$ENV{HYSPEX_SDK}/bin/"
      )
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    find_library(HySpex_LIBRARY
      NAMES HySpexLibrary
      HINTS
        "$ENV{HYSPEX_SDK}/lib/"
        "$ENV{HYSPEX_SDK}/bin/"
      )
  endif()
else()
  find_path(HySpex_INCLUDE_DIR
    NAMES hyspex/Camera.h Camera.h
    HINTS
	  "$ENV{HYSPEX_SDK}/include"
	)
  find_library(HySpex_LIBRARY
    NAMES HySpexLibrary
    HINTS "$ENV{HYSPEX_SDK}/lib")
endif()

set(HySpex_LIBRARIES ${HySpex_LIBRARY})
set(HySpex_INCLUDE_DIRS ${HySpex_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HySpex
  DEFAULT_MSG
  HySpex_INCLUDE_DIR HySpex_LIBRARY)

mark_as_advanced(HySpex_INCLUDE_DIR HySpex_LIBRARY)

if(HySpex_FOUND AND NOT TARGET HySpex::HySpex)
  add_library(HySpex::HySpex UNKNOWN IMPORTED)
  set_target_properties(HySpex::HySpex PROPERTIES
    IMPORTED_LOCATION "${HySpex_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${HySpex_INCLUDE_DIRS}")
endif()

if(HySpex_FOUND AND NOT TARGET HySpex::Headers)
  add_library(HySpex::Headers INTERFACE IMPORTED)
  set_target_properties(HySpex::Headers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${HySpex_INCLUDE_DIRS}")
endif()


