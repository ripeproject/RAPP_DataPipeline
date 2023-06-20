


add_executable(CropOps)

set_target_properties(CropOps PROPERTIES LANGUAGE CXX)

target_compile_features(CropOps PRIVATE cxx_std_20)

target_sources(CropOps
PRIVATE

	main.cpp
)

target_include_directories(CropOps PRIVATE ${CMAKE_INSTALL_PREFIX}/include)


#
# Due to Qt's license, we must use the Qt DLLs.  For Windows, we must use MSVC dynamic runtime libraries
# Force use MultiThreadedDLL (-MD) and MultiThreadedDebugDLL (-MDd)
set_property(TARGET CropOps PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


#******************************************************************************
# Installer
#******************************************************************************


