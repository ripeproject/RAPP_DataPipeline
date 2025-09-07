

#-------------------------------------------------------------------------------
# Creating a data storage support library
#-------------------------------------------------------------------------------

add_library(BlockDataFileRecovery_v1 STATIC)

set_target_properties(BlockDataFileRecovery_v1 PROPERTIES LANGUAGE CXX)

target_compile_features(BlockDataFileRecovery_v1 PRIVATE cxx_std_20)

target_sources(BlockDataFileRecovery_v1
PUBLIC

	BlockDataFileRecovery.hpp

PRIVATE

	BlockDataFileRecovery.cpp
)

target_include_directories(BlockDataFileRecovery_v1 PRIVATE ${CMAKE_INSTALL_PREFIX}/include)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET BlockDataFileRecovery_v1 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

